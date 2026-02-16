# Generic Driver Framework v1

## Purpose
Define a reusable architecture so generated drivers stay platform-agnostic while allowing AUTOSAR project integration through explicit ports. TMP102 remains the reference implementation.

## Layered Architecture

### 1) Core Driver Logic (generator-owned)
- Device state machine, sampling/polling logic, retries/backoff, conversion, limits.
- Device register semantics and policy decisions.
- No direct AUTOSAR BSW, no vendor HAL, no OS API calls.

### 2) Integrator Port Layer (integrator-owned)
- Concrete bindings for bus transactions, timing source, diagnostics and optional health monitors.
- Maps generic contracts to AUTOSAR services (I2C/SPI stacks, OS counters, DET/DEM, watchdog hooks) or test doubles.
- Owns target-specific includes and adaptation logic.

### 3) AUTOSAR Adapter Layer (project-owned)
- RTE-facing API, SchM scheduling hooks, MemMap sections, ECUC wiring.
- Bridges driver core outputs to SWC/BSW integration style used by project.

## Reusable Interfaces

### Bus
- `BusPort` contract for read/write primitives.
- v1 baseline: I2C 16-bit register read/write.
- Future-proof via transport-neutral naming and result classification (OK/NACK/TIMEOUT/ERROR).

### Time
- `TimePort` monotonic millisecond source.
- Enables deterministic policy behavior and testability.

### Diagnostics
- `DiagPort` development/runtime reporting contract.
- Integrators map to DET/DEM conventions.

### Health
- Reserved hook family for health reporting (error streaks, degraded operation, stale data alarms).
- v1 keeps this optional to avoid over-coupling.

### Quality
- Shared quality primitives (`INVALID/VALID/STALE/DEGRADED`) and timestamped state updates.
- Prevents per-driver divergence in quality handling semantics.

## Device Profile Concept
A **device profile** packages what varies by device while keeping framework behavior stable:
- Register map metadata (addresses, width, access mode)
- Conversion rules (raw -> engineering units, sign extension, scaling)
- Policy defaults (poll period, retry backoff, stale timeout, thresholds)
- Capability flags (threshold support, alert pin support, interrupt mode)

Profile data should be declarative so new devices reuse framework logic with minimal custom code.

## Extension Strategy (I2C/SPI/UART)
- Keep core logic transport-agnostic.
- Add protocol-specific request structs in `BusPort` while preserving common result model.
- Incremental path:
  1. I2C baseline (TMP102)
  2. SPI requests (full-duplex + chip-select semantics)
  3. UART framed transactions (request/response + timeout policy)
- Do not embed protocol-specific APIs into core driver files.

## Platform-Agnostic Generator Boundaries
Generator output must:
- Depend only on framework interfaces (`src/common/*`) and device-local headers.
- Avoid target compiler pragmas, vendor headers, or MCU register access.
- Keep MemMap/RTE/OS/BSW includes in integrator/adapter layers.
- Use explicit port API extensions when new platform needs appear.

## Locked Defaults (2026-02-16)
- AUTOSAR baseline: 4.4 (with adapter-level 4.3 compatibility notes).
- Diagnostics: DEV class -> DET, RUNTIME class -> DEM-capable path.
- Canonical internal time unit: monotonic milliseconds.
- Migration window: legacy per-device port shims remain for two milestones.

## TMP102 as Reference
TMP102 demonstrates:
- Device-specific conversion + polling policy in core module.
- Integrator-owned bus/diag/time mappings behind generic ports.
- Backward-compatible wrapper strategy for migration from device-local ports.
