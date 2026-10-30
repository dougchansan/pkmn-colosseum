#!/usr/bin/env python3
"""Compare isolated library cleanup variants without changing the game build.

Compiler commands and flags come from generated build.ninja. The control and
variant retain the source basename, including __FILE__ strings. Only scores,
sizes, symbol layouts, hashes and direct-call counts leave temporary storage;
original objects, instruction text and raw objdiff output are never exported.
This diagnostic grants no matching credit and never replaces active objects.
"""
import argparse
from collections import Counter
import hashlib
import json
import math
from pathlib import Path
import re
import shlex
import struct
import subprocess
import sys
import tempfile


def sha(blob):
    return hashlib.sha256(blob).hexdigest()


def recipe(root, build_dir, source):
    output = build_dir / 'src' / Path(source).relative_to('src').with_suffix('.o')
    ninja = re.sub(r'\$\r?\n[ \t]*', ' ', (root / 'build.ninja').read_text())
    match = re.search(r'(?ms)^build ' + re.escape(output.as_posix())
                      + r': (\w+) .*?(?=^build |\Z)', ninja)
    if not match:
        raise ValueError('missing_generated_compile_rule')
    rule = match.group(1)
    if rule not in ('mwcc', 'mwcc_sjis'):
        raise ValueError('unsupported_compile_postprocessing')
    values = dict(re.findall(r'(?m)^  (\w+) = (.*)$', match.group()))
    values.update({'in': source, 'out': output.as_posix()})
    rule_match = re.search(r'(?m)^rule ' + re.escape(rule) + r'\n  command = (.*)$', ninja)
    if not rule_match:
        raise ValueError('missing_generated_compiler_command')
    command = rule_match.group(1).split(' && ', 1)[0]
    for _ in range(5):
        if not re.search(r'\$[A-Za-z_{]', command):
            break
        command = re.sub(r'\$(?:\{(\w+)\}|(\w+))',
                         lambda m: values[m.group(1) or m.group(2)], command)
    if '$' in command:
        raise ValueError('unexpanded_ninja_variable')
    args = shlex.split(command)
    compilers = [i for i, value in enumerate(args) if Path(value).name == 'mwcceppc.exe']
    if len(compilers) != 1 or args.count('-c') != 1 or args.count('-o') != 1:
        raise ValueError('unrecognized_compiler_recipe')
    return {'argv': args, 'compiler_version': values['mw_version'], 'rule': rule,
            'source_object': output, 'target_object': build_dir / 'obj' / Path(source).relative_to('src').with_suffix('.o')}


def transform(source, entry):
    if sha(source.encode()) != entry['source_sha256']:
        raise ValueError('baseline_source_hash_changed')
    for edit in entry.get('replacements', []):
        if source.count(edit['old']) != edit.get('count', 1):
            raise ValueError('source_edit_anchor_count_changed')
        source = source.replace(edit['old'], edit['new'], edit.get('count', 1))
    if entry.get('variant_sha256') and sha(source.encode()) != entry['variant_sha256']:
        raise ValueError('variant_source_hash_changed')
    return source


def command_for(record, input_path, output_dir, entry=None):
    args = list(record['argv'])
    if entry:
        for edit in entry.get('flag_replacements', []):
            positions = [i for i, token in enumerate(args[:-1]) if token == edit['option']]
            if not positions or edit.get('occurrence') != 'last':
                raise ValueError('unsupported_flag_edit')
            at = positions[-1] + 1
            if args[at] != edit['old']:
                raise ValueError('compiler_flag_anchor_changed')
            args[at] = edit['new']
    args[args.index('-c') + 1] = str(input_path)
    args[args.index('-o') + 1] = str(output_dir)
    return args


def percentage(value):
    if value is None:
        return None
    if isinstance(value, bool) or not isinstance(value, (int, float)) or not math.isfinite(value) or not 0 <= value <= 100:
        raise ValueError('invalid_objdiff_percentage')
    return value


def object_metadata(path, Elf):
    obj = Elf(path.read_bytes())
    sections, functions, symbols = [], {}, []
    for index in obj.allocated.values():
        section = obj.sections[index]
        sections.append({'name': obj.names[index], 'size': section[5], 'alignment': section[8],
                         'nobits': section[1] == 8,
                         'sha256': None if section[1] == 8 else sha(obj.payload(index))})
    for name, symbol in obj.symbols:
        if not name or symbol[5] not in obj.allocated.values() or not symbol[2] or symbol[3] & 15 == 3:
            continue
        row = {'name': name, 'section': obj.names[symbol[5]], 'offset': symbol[1],
               'size': symbol[2], 'type': symbol[3] & 15, 'binding': symbol[3] >> 4,
               'visibility': symbol[4]}
        symbols.append(row)
        if row['type'] == 2 and not symbol[4] & 2:
            if name in functions:
                raise ValueError('duplicate_function_name')
            functions[name] = (symbol[5], symbol[1], symbol[2])

    def containing(section, offset):
        hits = [(name, offset - start) for name, (sec, start, size) in functions.items()
                if sec == section and start <= offset < start + size]
        return hits[0] if len(hits) == 1 else None

    relocs = {}
    for index, section in enumerate(obj.sections):
        if section[1] == 9 and section[5]:
            raise ValueError('implicit_relocation_unsupported')
        if section[1] != 4 or not section[5]:
            continue
        if section[9] != 12 or section[5] % 12 or section[6] != obj.symtab \
                or section[7] not in obj.allocated.values():
            raise ValueError('malformed_relocation_table')
        for offset, info, addend in struct.iter_unpack('>IIi', obj.payload(index)):
            if info & 255 != 10:
                continue
            if not 0 < info >> 8 < len(obj.symbols) or offset % 4 \
                    or offset + 4 > obj.sections[section[7]][5] \
                    or (section[7], offset) in relocs:
                raise ValueError('malformed_call_relocation')
            name, symbol = obj.symbols[info >> 8]
            identity = containing(symbol[5], symbol[1] + addend)
            relocs[(section[7], offset)] = identity or (name, addend)
    calls = {}
    for name, (index, start, size) in functions.items():
        counts = Counter()
        sites = {}
        payload = obj.payload(index)
        for at in range(start, start + size, 4):
            word = struct.unpack_from('>I', payload, at)[0]
            if word >> 26 != 18 or not word & 1:
                continue
            identity = relocs.get((index, at))
            if identity is None:
                displacement = word & 0x03FFFFFC
                if displacement & 0x02000000:
                    displacement -= 0x04000000
                identity = containing(index, at + displacement) if not word & 2 else None
            identity = identity or ('<unresolved_direct_call>', 0)
            counts[identity] += 1
            sites.setdefault(identity, []).append(at - start)
        calls[name] = [{'target': target, 'target_offset': offset, 'count': count,
                        'site_offsets': sites[(target, offset)]}
                       for (target, offset), count in sorted(counts.items())]
    return {'sections': sections, 'symbols': symbols, 'functions': functions,
            'direct_calls': calls}


def requirements(metadata, entry):
    """Check only explicit probe invariants; never manufacture missing symbols."""
    failures = []
    for wanted in entry.get('required_functions', []):
        found = [row for row in metadata['symbols'] if row['name'] == wanted['name']]
        if len(found) != 1 or any(found[0].get(key) != value for key, value in wanted.items()):
            failures.append({'kind': 'required_function', 'name': wanted['name']})
    for wanted in entry.get('required_direct_calls', []):
        found = [row for row in metadata['direct_calls'].get(wanted['caller'], [])
                 if row['target'] == wanted['target']
                 and row['target_offset'] == wanted.get('target_offset', 0)]
        expected = {key: value for key, value in wanted.items() if key != 'caller'}
        if len(found) != 1 or any(found[0].get(key) != value for key, value in expected.items()):
            failures.append({'kind': 'required_direct_call', 'caller': wanted['caller'],
                             'target': wanted['target']})
    for forbidden in entry.get('forbidden_direct_calls', []):
        for caller in forbidden['callers']:
            if caller not in metadata['functions']:
                failures.append({'kind': 'missing_caller', 'caller': caller})
                continue
            for call in metadata['direct_calls'].get(caller, []):
                if call['target'] in forbidden['targets']:
                    failures.append({'kind': 'forbidden_direct_call', 'caller': caller,
                                     'target': call['target']})
    return {'passed': not failures, 'failures': failures}


def summarize_diff(raw, target, source, entry):
    left_symbols = (raw.get('left') or {}).get('symbols', [])
    right_symbols = (raw.get('right') or {}).get('symbols', [])

    def find_symbol(records, name, location):
        found = [item for item in records if item['name'] == name]
        if len(found) != 1 or location is None:
            return None
        item = found[0]
        # objdiff's protobuf JSON omits a zero address. Symbol indices refer
        # to its internal table, not the filtered JSON symbol array.
        if int(item.get('address', 0)) != location[1] \
                or int(item.get('size', -1)) != location[2]:
            raise ValueError('objdiff_symbol_layout_disagrees')
        return item

    rows = []
    for name, (_, offset, size) in target['functions'].items():
        actual = source['functions'].get(name)
        left = find_symbol(left_symbols, name, target['functions'][name])
        right = find_symbol(right_symbols, name, actual)
        identity = left is not None and right is not None
        rows.append({'name': name, 'target_offset': offset, 'target_size': size,
                     'source_offset': actual[1] if actual else None,
                     'source_size': actual[2] if actual else None,
                     'match_identity_verified': identity,
                     'match_percent': percentage(left.get('match_percent')) if left else None,
                     'source_match_percent': percentage(right.get('match_percent')) if right else None,
                     'target_direct_calls': target['direct_calls'].get(name, []),
                     'source_direct_calls': source['direct_calls'].get(name, [])})
    wanted_names = set(target['functions'])
    target_requirements = requirements(target, entry)
    source_requirements = requirements(source, entry)
    expected_target = entry.get('expected_target_functions', [])
    actual_target = [{key: item[key] for key in ('name', 'section', 'offset', 'size', 'type', 'binding')}
                     for item in target['symbols'] if item['name'] in target['functions']]
    shape = (len(rows) == entry['expected_functions']
             and sum(row['target_size'] for row in rows) == entry['expected_code_bytes']
             and bool(expected_target)
             and sorted(actual_target, key=lambda row: row['name'])
                 == sorted(expected_target, key=lambda row: row['name'])
             and target_requirements['passed'])
    return {'target_scope_as_expected': shape,
            'target_requirements': target_requirements,
            'source_requirements': source_requirements,
            'all_target_functions_score_exact': shape and all(
                row['match_identity_verified'] and row['source_size'] == row['target_size']
                and row['match_percent'] == 100 for row in rows),
            'functions': rows,
            'extra_source_functions': sorted(set(source['functions']) - wanted_names),
            'section_scores': {side: [{'name': s['name'], 'size': s.get('size'),
                                      'match_percent': percentage(s.get('match_percent'))}
                                     for s in (raw.get(side) or {}).get('sections', [])]
                               for side in ('left', 'right')},
            'target_sections': target['sections'], 'source_sections': source['sections'],
            'target_symbol_layouts': target['symbols'], 'source_symbol_layouts': source['symbols']}


def run_case(root, directory, text, record, entry, variant, Elf, objdiff, target):
    directory.mkdir()
    src = directory / Path(entry['source']).name
    src.write_text(text)
    out = directory / 'obj'
    out.mkdir()
    command = command_for(record, src, out, entry if variant else None)
    result = {'source_sha256': sha(text.encode())}
    compiled = subprocess.run(command, cwd=root, capture_output=True, timeout=60)
    result['compiler_returncode'] = compiled.returncode
    output = out / src.with_suffix('.o').name
    if compiled.returncode or not output.is_file():
        result['status'] = 'compile_failed'
        return result
    metadata = object_metadata(output, Elf)
    process = subprocess.run([str(objdiff), 'diff', '-1', str(root / record['target_object']),
                              '-2', str(output), '-o', '-', '--format', 'json'],
                             cwd=root, capture_output=True, timeout=60)
    if process.returncode:
        return {**result, 'status': 'objdiff_failed', 'objdiff_returncode': process.returncode}
    result.update(summarize_diff(json.loads(process.stdout), target, metadata, entry))
    if not result['target_scope_as_expected']:
        result['status'] = 'target_scope_failed'
    elif not result['source_requirements']['passed']:
        result['status'] = 'requirements_failed'
    elif not result['all_target_functions_score_exact'] or result['extra_source_functions']:
        result['status'] = 'code_mismatch'
    else:
        result['status'] = 'code_scores_exact_pending_full_validation'
    active = root / record['source_object']
    if not variant and active.is_file():
        active_meta = object_metadata(active, Elf)
        result['control_matches_active_section_payloads'] = metadata['sections'] == active_meta['sections']
        result['control_matches_active_symbol_layouts'] = metadata['symbols'] == active_meta['symbols']
        result['control_matches_active_direct_calls'] = metadata['direct_calls'] == active_meta['direct_calls']
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=Path.cwd())
    parser.add_argument('--build-dir', type=Path, default=Path('build/GC6E01'))
    parser.add_argument('--manifest', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--prepare-only', action='store_true')
    args = parser.parse_args()
    root = args.repo.resolve()
    sys.path.insert(0, str(root))
    from tools.source_object_metadata import Elf
    manifest = json.loads(args.manifest.read_text())
    if manifest.get('schema_version') != 1:
        raise ValueError('unknown_manifest_version')
    results = []
    for entry in manifest['probes']:
        row = {'id': entry['id'], 'unit': entry['unit'], 'source': entry['source']}
        results.append(row)
        try:
            source = (root / entry['source']).read_text()
            variant = transform(source, entry)
            record = recipe(root, args.build_dir, entry['source'])
            command_for(record, Path('probe.c'), Path('probe-obj'), entry)
            row.update({'compiler_version': record['compiler_version'], 'compiler_recipe': record['argv'],
                        'flag_replacements': entry.get('flag_replacements', []),
                        'baseline_source_sha256': sha(source.encode()), 'variant_source_sha256': sha(variant.encode())})
            if args.prepare_only:
                row['status'] = 'prepared'
                continue
            target = object_metadata(root / record['target_object'], Elf)
            with tempfile.TemporaryDirectory(prefix='library-cleanup-') as temporary:
                scratch = Path(temporary)
                for label, text, changed in [('control', source, False), ('variant', variant, True)]:
                    row[label] = run_case(root, scratch / label, text, record, entry, changed,
                                          Elf, root / 'build/tools/objdiff-cli', target)
            row['status'] = 'completed'
        except (OSError, ValueError, KeyError, IndexError, struct.error, subprocess.TimeoutExpired) as error:
            row['status'] = 'diagnostic_error'
            row['error_type'] = type(error).__name__
            # These identifiers are generated by this script, never compiler
            # diagnostics or objdiff data that might contain original bytes.
            if isinstance(error, ValueError) and re.fullmatch(r'[a-z_]+', str(error)):
                row['error_code'] = str(error)
    result = {'schema_version': 1, 'metadata_only': True, 'diagnostic_only': True,
              'active_build_objects_modified': False, 'matching_credit_added': 0,
              'prepare_only': args.prepare_only, 'probes': results}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({'output': str(args.output), 'probes': [
        {'id': row['id'], 'status': row['status']} for row in results]}))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
