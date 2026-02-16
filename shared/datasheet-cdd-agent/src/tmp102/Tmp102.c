#include "Tmp102.h"
#include "Tmp102_Port.h"      /* Integrator-owned hooks */
#include "../common/BusPort.h"
#include "../common/DiagPort.h"
#include "../common/TimePort.h"
#include "../common/QualityPolicy.h"

/* ===== Internal state ===== */
typedef struct {
  boolean Initialized;
  Tmp102_StatusType Status;
  Tmp102_TempType LastTemp;
  Dgf_QualityStateType QualityState;
  uint32 ElapsedPollMs;
  uint32 ElapsedRetryMs;
  uint32 UptimeMs;
  const Tmp102_ConfigType* Cfg;
} Tmp102_RuntimeType;

static Tmp102_RuntimeType Tmp102_Runtime = {
  .Initialized = FALSE,
  .Status = { TMP102_STATUS_UNINIT, 0u, 0u },
  .LastTemp = { 0, TMP102_QUALITY_INVALID },
  .QualityState = { DGF_QUALITY_INVALID, 0u },
  .ElapsedPollMs = 0u,
  .ElapsedRetryMs = 0u,
  .UptimeMs = 0u,
  .Cfg = NULL_PTR
};

static void Tmp102_ReportDet(uint8 sid, uint8 err)
{
  Dgf_Diag_ReportDevError(TMP102_MODULE_ID, TMP102_INSTANCE_ID, sid, err);
}

static void Tmp102_ApplyQuality(Dgf_QualityType quality, uint32 nowMs)
{
  Dgf_Quality_Mark(&Tmp102_Runtime.QualityState, quality, nowMs);

  switch (quality) {
    case DGF_QUALITY_VALID:
      Tmp102_Runtime.LastTemp.Quality = TMP102_QUALITY_VALID;
      break;
    case DGF_QUALITY_STALE:
    case DGF_QUALITY_DEGRADED:
      Tmp102_Runtime.LastTemp.Quality = TMP102_QUALITY_STALE;
      break;
    case DGF_QUALITY_INVALID:
    default:
      Tmp102_Runtime.LastTemp.Quality = TMP102_QUALITY_INVALID;
      break;
  }
}

/* TMP102 12-bit (typical): LSB = 0.0625°C => 6.25 centi-deg C */
static Tmp102_TempCentiDegCType Tmp102_ConvertRawToCentiDeg(uint16 raw)
{
  sint16 signed12 = (sint16)(raw >> 4);
  if ((signed12 & 0x0800) != 0) {
    signed12 |= (sint16)0xF000;
  }
  return (Tmp102_TempCentiDegCType)(signed12 * 625 / 100);
}

void Tmp102_Init(const Tmp102_ConfigType* ConfigPtr)
{
  if ((ConfigPtr == NULL_PTR) || (ConfigPtr->ChannelCfg == NULL_PTR)) {
    Tmp102_ReportDet(TMP102_SID_INIT, TMP102_E_PARAM_CONFIG);
    return;
  }

  if ((ConfigPtr->ChannelCfg->MainFunctionPeriodMs == 0u) ||
      (ConfigPtr->ChannelCfg->PollingPeriodMs == 0u)) {
    Tmp102_ReportDet(TMP102_SID_INIT, TMP102_E_PARAM_CONFIG);
    return;
  }

  Tmp102_Runtime.Cfg = ConfigPtr;
  Tmp102_Runtime.Initialized = TRUE;
  Tmp102_Runtime.Status.State = TMP102_STATUS_READY;
  Tmp102_Runtime.Status.ConsecutiveErrors = 0u;
  Tmp102_Runtime.LastTemp.Value = 0;
  Tmp102_ApplyQuality(DGF_QUALITY_INVALID, 0u);

  if (ConfigPtr->ChannelCfg->EnableThresholdSupport == TRUE) {
    (void)Tmp102_Port_I2cWriteThresholds(
      ConfigPtr->ChannelCfg->I2cChannelId,
      ConfigPtr->ChannelCfg->I2cAddress,
      ConfigPtr->ChannelCfg->TLowRaw,
      ConfigPtr->ChannelCfg->THighRaw);
  }

  Tmp102_Runtime.ElapsedPollMs = 0u;
  Tmp102_Runtime.ElapsedRetryMs = 0u;
  Tmp102_Runtime.UptimeMs = 0u;
  Tmp102_Runtime.Status.LastSampleTick = 0u;
}

void Tmp102_MainFunction(void)
{
  uint16 raw = 0u;
  Std_ReturnType ret;
  uint16 baseTickMs;
  uint32 nowMs;
  Dgf_I2cRead16RequestType readReq;
  Dgf_BusErrorType busError;

  if ((Tmp102_Runtime.Initialized == FALSE) || (Tmp102_Runtime.Cfg == NULL_PTR)) {
    Tmp102_ReportDet(TMP102_SID_MAINFUNCTION, TMP102_E_UNINIT);
    return;
  }

  baseTickMs = Tmp102_Runtime.Cfg->ChannelCfg->MainFunctionPeriodMs;
  Tmp102_Runtime.UptimeMs += baseTickMs;
  Tmp102_Runtime.ElapsedPollMs += baseTickMs;
  Tmp102_Runtime.ElapsedRetryMs += baseTickMs;

  if (Tmp102_Runtime.Status.State == TMP102_STATUS_ERROR) {
    if (Tmp102_Runtime.ElapsedRetryMs < Tmp102_Runtime.Cfg->ChannelCfg->RetryBackoffMs) {
      return;
    }
  }

  if (Tmp102_Runtime.ElapsedPollMs < Tmp102_Runtime.Cfg->ChannelCfg->PollingPeriodMs) {
    return;
  }

  readReq.BusId = Tmp102_Runtime.Cfg->ChannelCfg->I2cChannelId;
  readReq.DeviceAddress = Tmp102_Runtime.Cfg->ChannelCfg->I2cAddress;
  readReq.RegisterAddress = TMP102_REG_TEMP;
  readReq.Data = &raw;
  ret = Dgf_Bus_I2cRead16(&readReq, &busError);
  (void)busError; /* reserved for normalized error-specific handling */

  nowMs = Dgf_Time_GetMonotonicMs();
  if (nowMs == 0u) {
    nowMs = Tmp102_Runtime.UptimeMs;
  }

  if (ret == E_OK) {
    Tmp102_Runtime.LastTemp.Value = Tmp102_ConvertRawToCentiDeg(raw);
    Tmp102_ApplyQuality(DGF_QUALITY_VALID, nowMs);
    Tmp102_Runtime.Status.State = TMP102_STATUS_READY;
    Tmp102_Runtime.Status.ConsecutiveErrors = 0u;
    Tmp102_Runtime.Status.LastSampleTick = nowMs;
    Tmp102_Runtime.ElapsedPollMs = 0u;
    Tmp102_Runtime.ElapsedRetryMs = 0u;
  } else {
    Tmp102_Runtime.Status.State = TMP102_STATUS_ERROR;
    Tmp102_Runtime.Status.ConsecutiveErrors++;
    Tmp102_ApplyQuality(DGF_QUALITY_STALE, nowMs);
    Tmp102_Runtime.ElapsedRetryMs = 0u;
    Tmp102_ReportDet(TMP102_SID_MAINFUNCTION, TMP102_E_I2C_TRANSFER);
  }
}

Std_ReturnType Tmp102_GetTemperature(Tmp102_TempType* TempPtr)
{
  if (Tmp102_Runtime.Initialized == FALSE) {
    Tmp102_ReportDet(TMP102_SID_GETTEMPERATURE, TMP102_E_UNINIT);
    return E_NOT_OK;
  }
  if (TempPtr == NULL_PTR) {
    Tmp102_ReportDet(TMP102_SID_GETTEMPERATURE, TMP102_E_PARAM_POINTER);
    return E_NOT_OK;
  }

  *TempPtr = Tmp102_Runtime.LastTemp; /* cached-only policy */
  return E_OK;
}

Std_ReturnType Tmp102_GetStatus(Tmp102_StatusType* StatusPtr)
{
  if (Tmp102_Runtime.Initialized == FALSE) {
    Tmp102_ReportDet(TMP102_SID_GETSTATUS, TMP102_E_UNINIT);
    return E_NOT_OK;
  }
  if (StatusPtr == NULL_PTR) {
    Tmp102_ReportDet(TMP102_SID_GETSTATUS, TMP102_E_PARAM_POINTER);
    return E_NOT_OK;
  }

  *StatusPtr = Tmp102_Runtime.Status;
  return E_OK;
}
