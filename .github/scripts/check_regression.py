#!/usr/bin/env python3
"""Fail if any function's objdiff fuzzy_match_percent regressed base->head."""

import argparse
import json
import sys

# Objdiff reports percentages as floats. Treat changes below one hundredth of
# a percentage point as report-level drift rather than actionable regression.
EPSILON = 0.01

def fmap(path):
    with open(path, encoding="utf-8") as stream:
        r = json.load(stream)
    m = {}
    for u in r.get("units", []):
        fns = list(u.get("functions", []))
        for s in u.get("sections", []):
            fns += s.get("functions", [])
        for f in fns:
            n = f.get("name")
            score = f.get("fuzzy_match_percent")
            if n and score is not None:
                m[n] = float(score)
    return m


def cleanup_allowances(path, base_sha):
    if not path or not base_sha:
        return {}
    with open(path, encoding="utf-8") as stream:
        manifest = json.load(stream)
    if set(manifest) != {"base_sha", "regressions"}:
        raise ValueError("cleanup manifest must contain only base_sha and regressions")
    if manifest["base_sha"] != base_sha:
        return {}
    allowances = {}
    for name, scores in manifest["regressions"].items():
        if set(scores) != {"base", "head"}:
            raise ValueError(f"invalid cleanup score entry for {name}")
        allowances[name] = (float(scores["base"]), float(scores["head"]))
    return allowances


def check(base_path, head_path, cleanup_manifest=None, base_sha=None):
    base, head = fmap(base_path), fmap(head_path)
    regs = [
        (n, base[n], head.get(n, 0.0))
        for n in base
        if head.get(n, base[n]) < base[n] - EPSILON
    ]
    gains = [n for n in head if head[n] >= 100 > base.get(n, 0)]
    allowances = cleanup_allowances(cleanup_manifest, base_sha)
    allowed = []
    blocked = []
    for name, base_score, head_score in regs:
        expected = allowances.get(name)
        if (
            expected is not None
            and abs(base_score - expected[0]) <= EPSILON
            and abs(head_score - expected[1]) <= EPSILON
        ):
            allowed.append((name, base_score, head_score))
        else:
            blocked.append((name, base_score, head_score))

    for name, base_score, head_score in sorted(allowed):
        print(
            f"Policy cleanup allowed: {name}: "
            f"{base_score:.5f}% -> {head_score:.5f}%"
        )
    if blocked:
        print(f"::error::{len(blocked)} function(s) regressed:")
        for name, base_score, head_score in sorted(
            blocked, key=lambda item: item[1] - item[2], reverse=True
        ):
            print(f"::error::  {name}: {base_score:.2f}% -> {head_score:.2f}%")
        print(f"\n(+{len(gains)} new 100% matches, but regressions block merge)")
        return 1
    print(
        f"No unapproved regressions. +{len(gains)} new byte-exact matches; "
        f"{len(allowed)} policy cleanup(s)."
    )
    return 0


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("base_report")
    parser.add_argument("head_report")
    parser.add_argument("--cleanup-manifest")
    parser.add_argument("--base-sha")
    args = parser.parse_args()
    try:
        return check(
            args.base_report,
            args.head_report,
            args.cleanup_manifest,
            args.base_sha,
        )
    except (OSError, TypeError, ValueError, json.JSONDecodeError) as error:
        print(f"::error::Invalid regression cleanup configuration: {error}")
        return 2


if __name__ == "__main__":
    sys.exit(main())
