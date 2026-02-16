#ifndef DGF_DIAG_PORT_H
#define DGF_DIAG_PORT_H

#include "Std_Types.h"

/*
 * Generic Driver Framework (DGF) diagnostic abstraction.
 * Typical integration maps this to DET/DEM project conventions.
 */

void Dgf_Diag_ReportDevError(uint16 moduleId, uint8 instanceId, uint8 apiId, uint8 errorId);

#endif /* DGF_DIAG_PORT_H */
