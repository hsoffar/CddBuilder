#ifndef TMP102_PORT_H
#define TMP102_PORT_H

#include "Std_Types.h"
#include "../common/BusPort.h"

/*
 * Integrator-owned interface.
 * This file is stable and consumed by Tmp102.c.
 */

#define TMP102_REG_TEMP   (0x00u)
#define TMP102_REG_TLOW   (0x02u)
#define TMP102_REG_THIGH  (0x03u)

/* Legacy TMP102-specific hooks kept for backward compatibility. */
Std_ReturnType Tmp102_Port_I2cReadTempRaw(uint8 channel, uint8 addr, uint16* raw);
Std_ReturnType Tmp102_Port_I2cWriteThresholds(uint8 channel, uint8 addr, uint16 low, uint16 high);

#endif /* TMP102_PORT_H */
