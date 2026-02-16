# Device Profile JSON -> C Flow (v1 draft)

## Scope
This document defines the minimal dual-source flow for device profiles:
- Source of truth: JSON profile files (`examples/profiles/*.profile.json`)
- Compile target: generated C structs (`src/generated/profiles/*`) in future step

## Current Status
- Schema available: `schemas/device_profile.schema.json`
- Reference profiles:
  - `examples/profiles/tmp102.profile.json`
  - `examples/profiles/lm75a.profile.json`
- Generator script is not implemented yet (planned).

## Intended Flow
1. Validate `*.profile.json` against schema.
2. Normalize fields (default policies, canonical register ordering).
3. Emit deterministic C artifacts:
   - `<device>_Profile.h`
   - `<device>_Profile.c`
4. Driver core consumes generated, typed profile structs.

## Compatibility Notes
- AUTOSAR baseline is 4.4 by default.
- 4.3 compatibility is handled through adapter config (not by changing profile semantics).

## Planned Script Interface (placeholder)
```bash
# planned
python3 tools/profile_codegen.py \
  --schema schemas/device_profile.schema.json \
  --input examples/profiles/tmp102.profile.json \
  --out-dir src/generated/profiles
```

## Constraints
- No platform/vendor APIs in generated C profile artifacts.
- Profile files must remain declarative and reusable across integrators.
