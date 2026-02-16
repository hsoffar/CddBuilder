#ifndef DGF_BUS_PORT_H
#define DGF_BUS_PORT_H

#include "Std_Types.h"

/*
 * Generic Driver Framework (DGF) Bus abstraction.
 * Platform/vendor specific code must implement these contracts in integrator layer.
 */

typedef enum {
  DGF_BUS_RESULT_OK = 0,
  DGF_BUS_RESULT_NACK,
  DGF_BUS_RESULT_TIMEOUT,
  DGF_BUS_RESULT_ERROR
} Dgf_BusResultType;

typedef struct {
  uint8 BusId;
  uint8 DeviceAddress;
  uint8 RegisterAddress;
  uint16* Data;
} Dgf_I2cRead16RequestType;

typedef struct {
  uint8 BusId;
  uint8 DeviceAddress;
  uint8 RegisterAddress;
  uint16 Data;
} Dgf_I2cWrite16RequestType;

Std_ReturnType Dgf_Bus_I2cRead16(const Dgf_I2cRead16RequestType* req, Dgf_BusResultType* result);
Std_ReturnType Dgf_Bus_I2cWrite16(const Dgf_I2cWrite16RequestType* req, Dgf_BusResultType* result);

#endif /* DGF_BUS_PORT_H */
