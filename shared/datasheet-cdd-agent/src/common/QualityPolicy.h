#ifndef DGF_QUALITY_POLICY_H
#define DGF_QUALITY_POLICY_H

#include "Std_Types.h"

/*
 * Generic quality primitives to keep core driver state and data-quality handling
 * consistent across sensor/actuator families.
 */

typedef enum {
  DGF_QUALITY_INVALID = 0,
  DGF_QUALITY_VALID,
  DGF_QUALITY_STALE,
  DGF_QUALITY_DEGRADED
} Dgf_QualityType;

typedef struct {
  Dgf_QualityType Value;
  uint32 LastUpdateMs;
} Dgf_QualityStateType;

static inline void Dgf_Quality_Init(Dgf_QualityStateType* state)
{
  if (state != NULL_PTR) {
    state->Value = DGF_QUALITY_INVALID;
    state->LastUpdateMs = 0u;
  }
}

static inline void Dgf_Quality_Mark(Dgf_QualityStateType* state, Dgf_QualityType quality, uint32 nowMs)
{
  if (state != NULL_PTR) {
    state->Value = quality;
    state->LastUpdateMs = nowMs;
  }
}

#endif /* DGF_QUALITY_POLICY_H */
