# CddBuilder CLI v1

## Dry-run command (deterministic)
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

## OpenAI adapter-path skeleton command (no external call in v1)
```bash
OPENAI_API_KEY=dummy \
python3 tools/cddbuilder_run.py \
  --pdf examples/datasheet.pdf \
  --profile prompts/system.md \
  --rules prompts/review-loop.md \
  --device TMP102 \
  --out out/tmp102_openai \
  --adapter openai \
  --checklist agents/cddbuilder/schema/output-checklist-v1.json
```

## Arguments
- `--pdf`: datasheet reference path
- `--profile`: profile markdown
- `--rules`: one or more rules markdown files
- `--device`: device name
- `--out`: output directory
- `--adapter`: adapter selector (`stub` or `openai`)
- `--checklist`: output checklist JSON used by validation gate
- `--dry-run`: deterministic local workflow (forces stub adapter)

## Expected outputs
The command creates:
- `src/<device>_driver_stub.c`
- `docs/<device>_cdd_outline.md`
- `tests/<device>_test_plan.md`
- `report/summary.md`
- `report/prompt.md`
- `report/validation.json`

`report/validation.json` includes machine-readable failure details:
```json
{
  "pass": false,
  "failureReasons": [
    {
      "code": "MISSING_REQUIRED_OUTPUT",
      "detail": "required artifact not generated",
      "path": "..."
    }
  ]
}
```
