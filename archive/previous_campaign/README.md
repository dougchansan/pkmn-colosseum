# Previous Campaign Archive

This directory preserves the pre-reset decompilation campaign material for
research only.

Contents include old agent/fleet tooling, PC-port experiments, custom report
generators, duplicate config files, old notes, and unconfigured source files
that were not carried into the active dtk-template project.

The active project root should use the standard dtk-template pipeline:

```bash
python configure.py
ninja
python configure.py progress
```

Do not treat anything in this archive as active progress unless it is moved back
through `config/GC6E01/splits.txt` and `configure.py`, then verified by the
generated dtk/objdiff report.
