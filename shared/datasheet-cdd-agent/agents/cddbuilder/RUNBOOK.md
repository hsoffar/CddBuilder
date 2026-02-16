# CddBuilder Runbook

## OpenClaw Session Invocation
From repository root:
```bash
python3 tools/cddbuilder_run.py \
  --pdf <datasheet.pdf> \
  --profile <profile.md> \
  --rules <rules1.md> --rules <rules2.md> \
  --device <DEVICE> \
  --out <output_dir> \
  --dry-run
```

## Expected Result
The script emits a structured package and validation report. Use `report/summary.md` as handoff summary to main agent.

## Safety
- Keep `--out` within project workspace.
- Do not add platform/vendor APIs into generated core files.
