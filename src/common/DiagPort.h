#ifndef DGF_DIAG_PORT_H
#define DGF_DIAG_PORT_H

#include "Std_Types.h"

/*
 * Generic Driver Framework (DGF) diagnostic abstraction.
 * DEV events route to DET; RUNTIME events route to DEM-capable path.
 */

typedef enum {
  DGF_DIAG_CLASS_DEV = 0,
  DGF_DIAG_CLASS_RUNTIME
} Dgf_DiagEventClassType;

typedef struct {
  uint16 ModuleId;
  uint8 InstanceId;
  uint8 ApiId;
  uint8 ErrorId;
  Dgf_DiagEventClassType EventClass;
} Dgf_DiagEventType;

void Dgf_Diag_ReportEvent(const Dgf_DiagEventType* event);

/* Backward-compatible helper for transition window. */
static inline void Dgf_Diag_ReportDevError(uint16 moduleId, uint8 instanceId, uint8 apiId, uint8 errorId)
{
  Dgf_DiagEventType event;
  event.ModuleId = moduleId;
  event.InstanceId = instanceId;
  event.ApiId = apiId;
  event.ErrorId = errorId;
  event.EventClass = DGF_DIAG_CLASS_DEV;
  Dgf_Diag_ReportEvent(&event);
}

#endif /* DGF_DIAG_PORT_H */
