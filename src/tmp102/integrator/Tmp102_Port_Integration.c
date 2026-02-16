#include "../Tmp102_Port.h"
#include "../../common/BusPort.h"
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

static Std_ReturnType Dgf_Bus_TransactionTmp102Compat(const Dgf_BusTransactionType* transaction, Dgf_BusErrorType* error)
{
  Std_ReturnType ret = E_NOT_OK;

  if ((transaction == NULL_PTR) || (error == NULL_PTR)) {
    if (error != NULL_PTR) {
      *error = DGF_BUS_ERR_PARAM;
    }
    return E_NOT_OK;
  }

  *error = DGF_BUS_ERR_UNSUPPORTED;

  if ((transaction->Protocol != DGF_BUS_PROTO_I2C) ||
      (transaction->TxData == NULL_PTR) ||
      (transaction->TxLength < 1u)) {
    *error = DGF_BUS_ERR_PARAM;
    return E_NOT_OK;
  }

  if ((transaction->Direction == DGF_BUS_DIR_READ) &&
      (transaction->RxData != NULL_PTR) &&
      (transaction->RxLength == 2u) &&
      (transaction->TxData[0] == TMP102_REG_TEMP)) {
    uint16 raw = 0u;
    ret = Tmp102_Port_I2cReadTempRaw(transaction->BusId, transaction->DeviceAddress, &raw);
    if (ret == E_OK) {
      transaction->RxData[0] = (uint8)((raw >> 8) & 0xFFu);
      transaction->RxData[1] = (uint8)(raw & 0xFFu);
      *error = DGF_BUS_ERR_NONE;
    } else {
      *error = DGF_BUS_ERR_BUS;
    }
    return ret;
  }

  if ((transaction->Direction == DGF_BUS_DIR_WRITE) &&
      (transaction->TxLength == 3u) &&
      ((transaction->TxData[0] == TMP102_REG_TLOW) ||
       (transaction->TxData[0] == TMP102_REG_THIGH))) {
    static uint16 s_lastTLow = 0u;
    static uint16 s_lastTHigh = 0u;
    uint16 data = (uint16)(((uint16)transaction->TxData[1] << 8) | (uint16)transaction->TxData[2]);

    if (transaction->TxData[0] == TMP102_REG_TLOW) {
      s_lastTLow = data;
    } else {
      s_lastTHigh = data;
    }

    ret = Tmp102_Port_I2cWriteThresholds(transaction->BusId, transaction->DeviceAddress, s_lastTLow, s_lastTHigh);
    *error = (ret == E_OK) ? DGF_BUS_ERR_NONE : DGF_BUS_ERR_BUS;
    return ret;
  }

  return E_NOT_OK;
}

Std_ReturnType Dgf_Bus_Transaction(const Dgf_BusTransactionType* transaction, Dgf_BusErrorType* error)
{
  return Dgf_Bus_TransactionTmp102Compat(transaction, error);
}

Dgf_TimeMsType Dgf_Time_GetMonotonicMs(void)
{
  /* TODO Integrator: map to OS/GPT monotonic tick source in milliseconds */
  return 0u;
}

void Dgf_Diag_ReportEvent(const Dgf_DiagEventType* event)
{
  if (event == NULL_PTR) {
    return;
  }

  if (event->EventClass == DGF_DIAG_CLASS_DEV) {
    (void)Det_ReportError(event->ModuleId, event->InstanceId, event->ApiId, event->ErrorId);
  } else {
    /* TODO Integrator: map to DEM event reporting path for runtime diagnostics */
  }
}
