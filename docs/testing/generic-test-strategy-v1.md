# Generic Test Strategy v1

## Objective
Define reusable validation for current TMP102 and future driver families with consistent quality, diagnostics, and integration gates.

## Test Pyramid

### Unit (largest volume)
- Pure conversion logic (raw decode, sign extension, scaling)
- State machine transitions (READY/ERROR/recovery)
- Retry/backoff calculations and stale-data policy
- Quality policy primitives

### Component
- Core driver + mocked `BusPort/TimePort/DiagPort`
- Deterministic scheduling of `MainFunction`
- Error burst handling and recovery behavior

### Integration
- Integrator layer bindings to AUTOSAR services (I2C/SPI/UART, DET/DEM, OS tick)
- Build/link checks with project headers, MemMap, and module IDs
- Multi-module interaction sanity checks

### HIL (top of pyramid)
- Real ECU + real device communication
- Timing under load, bus disturbance behavior, thermal/environmental drift checks where relevant

## Port Contract Tests
For each `Port` implementation, run reusable contract tests:
- Correct parameter validation and error return behavior
- Bus result mapping (NACK, timeout, generic error)
- Monotonic time behavior (no regressions in elapsed calculations)
- Diagnostic forwarding with expected IDs

## Reusable Test Vectors
Maintain a common vector set consumed by each device profile:
- Conversion vectors (min/nominal/max/negative edge cases)
- Limits/threshold vectors
- Timeout and backoff vectors
- Diagnostic emission vectors
- Quality transition vectors (VALID -> STALE -> VALID)

## Fault-Injection Matrix
Minimum matrix to execute at component + integration levels:
- NACK on read/write
- Bus timeout
- Stale data (no successful samples inside policy window)
- Invalid config at init
- Repeated transient failures followed by recovery

Track expected outcomes:
- status state
- quality state
- consecutive error counter
- diag events

## Acceptance Gates for New Driver Family Onboarding
A new driver family is accepted only if it passes:
1. Build gate: compiles in reference environment and target AUTOSAR project.
2. Unit gate: conversion/policy vector suite green.
3. Contract gate: all required Port contract tests green.
4. Fault gate: mandatory matrix executed with expected behavior.
5. Integration gate: diag wiring and scheduler integration verified.
6. HIL smoke gate: at least one real-device read/write cycle validated.

## CI Recommendations (v1)
- Run unit + component tests on each change.
- Run integration tests on merge requests.
- Run HIL smoke nightly or on release candidate branches.
