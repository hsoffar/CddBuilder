# OpenClaw CddBuilder Agent v1

## Purpose
CddBuilder is a dedicated OpenClaw agent responsible for turning device documentation context into a starter AUTOSAR CDD package.

Primary flow:
- Inputs: datasheet PDF path + profile/rules markdown context
- Processing: guided prompt assembly + dry/LLM execution path
- Outputs: structured package (`src/`, `docs/`, `tests/`, `report/`)

## Responsibilities
- Parse and combine project guidance markdown files into actionable generation context.
- Produce deterministic dry-run package output when external LLM invocation is disabled/unavailable.
- Emit explicit assumptions, gaps, and acceptance status in a summary report.
- Keep generator-owned output platform-agnostic (integrator-specific bindings separated).

## Input/Output Contract

### Inputs
- `--pdf <path>`: path to datasheet PDF (v1 records reference only; parsing can be expanded later)
- `--profile <md>`: primary project profile markdown
- `--rules <md>` (repeatable): policy/rules markdown files
- `--device <name>`: target device name
- `--out <dir>`: output directory
- `--dry-run`: force deterministic local generation with no external LLM call

### Outputs
Under `--out`:
- `src/<device>_driver_stub.c`
- `docs/<device>_cdd_outline.md`
- `tests/<device>_test_plan.md`
- `report/summary.md`
- `report/prompt.md`
- `report/validation.json`

## Tool Policy + Safety Boundaries
- No destructive filesystem actions outside explicit `--out` target.
- No network call in `--dry-run` mode.
- No platform/vendor-specific API insertion in generator-owned files.
- External messaging/actions remain in main agent scope; CddBuilder focuses on artifact generation.

## Collaboration Model (Main + Embedded)
- Main agent: user-facing communication, requirement locking, review gate.
- Embedded agent (implementation owner): design + implementation commits for CddBuilder package.
- Handoff contract: each batch reports changed files, rationale, run commands, and high-impact blockers only.

## Execution Modes
1. **Chat-trigger (default OpenClaw mode):** main/embedded session invokes CddBuilder script and returns artifacts.
2. **Script-trigger (automation mode):** direct local command execution in CI or cron-like flows.
3. **Future web UI:** invoke same backend script through UI wrapper with same input contract.

## v1 Non-goals
- Full PDF semantic extraction
- Live multi-provider orchestration
- Full AUTOSAR exporter integration

These are intentionally deferred to keep v1 deterministic and reviewable.
