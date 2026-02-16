# CddBuilder CLI v1

## Command
```bash
python3 tools/cddbuilder_run.py \
  --pdf examples/datasheet.pdf \
  --profile prompts/system.md \
  --rules prompts/review-loop.md \
  --device TMP102 \
  --out out/tmp102 \
  --adapter stub \
  --checklist agents/cddbuilder/schema/output-checklist-v1.json \
  --dry-run
```

## Arguments
- `--pdf`: datasheet reference path
- `--profile`: profile markdown
- `--rules`: one or more rules markdown files
- `--device`: device name
- `--out`: output directory
- `--adapter`: adapter selector (`stub` or `openai` placeholder)
- `--checklist`: output checklist JSON used by validation gate
- `--dry-run`: deterministic local workflow (forces stub adapter)

## Outputs
The command creates:
- `src/`
- `docs/`
- `tests/`
- `report/`

Validation summary is written to `report/validation.json`.
