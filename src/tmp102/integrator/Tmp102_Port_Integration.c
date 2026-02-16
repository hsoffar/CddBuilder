#include "../Tmp102_Port.h"
#include "../../common/DiagPort.h"
#include "../../common/TimePort.h"
#include "Det.h"

/*
 * Integrator-owned file.
 * Keep this outside generator-owned edits.
 * Bind to project-specific AUTOSAR I2C + OS tick APIs.
 */

Std_ReturnType Tmp102_Port_I2cReadTempRaw(uint8 channel, uint8 addr, uint16* raw)
{
  (void)channel;
  (void)addr;
  (void)raw;
  /* TODO Integrator: implement using AUTOSAR I2C stack */
  return E_NOT_OK;
}

Std_ReturnType Tmp102_Port_I2cWriteThresholds(uint8 channel, uint8 addr, uint16 low, uint16 high)
{
  (void)channel;
  (void)addr;
  (void)low;
  (void)high;
  /* TODO Integrator: implement using AUTOSAR I2C stack */
  return E_NOT_OK;
}

Std_ReturnType Dgf_Bus_I2cRead16(const Dgf_I2cRead16RequestType* req, Dgf_BusResultType* result)
{
  Std_ReturnType ret;

  if ((req == NULL_PTR) || (req->Data == NULL_PTR)) {
    if (result != NULL_PTR) {
      *result = DGF_BUS_RESULT_ERROR;
    }
    return E_NOT_OK;
  }

  if (req->RegisterAddress == TMP102_REG_TEMP) {
    ret = Tmp102_Port_I2cReadTempRaw(req->BusId, req->DeviceAddress, req->Data);
  } else {
    /* TODO Integrator: extend for generic register-based I2C read */
    ret = E_NOT_OK;
  }

  if (result != NULL_PTR) {
    *result = (ret == E_OK) ? DGF_BUS_RESULT_OK : DGF_BUS_RESULT_ERROR;
  }

  return ret;
}

Std_ReturnType Dgf_Bus_I2cWrite16(const Dgf_I2cWrite16RequestType* req, Dgf_BusResultType* result)
{
  static uint16 s_lastTLow = 0u;
  static uint16 s_lastTHigh = 0u;
  Std_ReturnType ret;

  if (req == NULL_PTR) {
    if (result != NULL_PTR) {
      *result = DGF_BUS_RESULT_ERROR;
    }
    return E_NOT_OK;
  }

  if (req->RegisterAddress == TMP102_REG_TLOW) {
    s_lastTLow = req->Data;
    ret = Tmp102_Port_I2cWriteThresholds(req->BusId, req->DeviceAddress, s_lastTLow, s_lastTHigh);
  } else if (req->RegisterAddress == TMP102_REG_THIGH) {
    s_lastTHigh = req->Data;
    ret = Tmp102_Port_I2cWriteThresholds(req->BusId, req->DeviceAddress, s_lastTLow, s_lastTHigh);
  } else {
    /* TODO Integrator: extend for generic register-based I2C write */
    ret = E_NOT_OK;
  }

  if (result != NULL_PTR) {
    *result = (ret == E_OK) ? DGF_BUS_RESULT_OK : DGF_BUS_RESULT_ERROR;
  }

  return ret;
}

Dgf_TimeMsType Dgf_Time_GetMonotonicMs(void)
{
  /* TODO Integrator: map to OS/GPT monotonic tick source */
  return 0u;
}

void Dgf_Diag_ReportDevError(uint16 moduleId, uint8 instanceId, uint8 apiId, uint8 errorId)
{
  (void)Det_ReportError(moduleId, instanceId, apiId, errorId);
}
