#ifndef DGF_BUS_PORT_H
#define DGF_BUS_PORT_H

#include "Std_Types.h"

/*
 * Generic Driver Framework (DGF) transaction-centric bus abstraction.
 * Integrator layer maps these contracts to AUTOSAR or platform services.
 */

typedef enum {
  DGF_BUS_PROTO_I2C = 0,
  DGF_BUS_PROTO_SPI,
  DGF_BUS_PROTO_UART
} Dgf_BusProtocolType;

typedef enum {
  DGF_BUS_DIR_READ = 0,
  DGF_BUS_DIR_WRITE
} Dgf_BusDirectionType;

typedef enum {
  DGF_BUS_ERR_NONE = 0,
  DGF_BUS_ERR_NACK,
  DGF_BUS_ERR_TIMEOUT,
  DGF_BUS_ERR_BUS,
  DGF_BUS_ERR_CRC,
  DGF_BUS_ERR_PARAM,
  DGF_BUS_ERR_UNSUPPORTED
} Dgf_BusErrorType;

typedef struct {
  Dgf_BusProtocolType Protocol;
  uint8 BusId;
  uint8 DeviceAddress;
  Dgf_BusDirectionType Direction;
  const uint8* TxData;
  uint16 TxLength;
  uint8* RxData;
  uint16 RxLength;
  uint32 TimeoutMs;
} Dgf_BusTransactionType;

Std_ReturnType Dgf_Bus_Transaction(const Dgf_BusTransactionType* transaction, Dgf_BusErrorType* error);

/* -------------------- Compatibility wrappers (transition window) -------------------- */

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

static inline Std_ReturnType Dgf_Bus_I2cRead16(const Dgf_I2cRead16RequestType* req, Dgf_BusErrorType* error)
{
  uint8 tx[1];
  uint8 rx[2];
  Dgf_BusTransactionType tr;
  Std_ReturnType ret;

  if ((req == NULL_PTR) || (req->Data == NULL_PTR)) {
    if (error != NULL_PTR) {
      *error = DGF_BUS_ERR_PARAM;
    }
    return E_NOT_OK;
  }

  tx[0] = req->RegisterAddress;
  tr.Protocol = DGF_BUS_PROTO_I2C;
  tr.BusId = req->BusId;
  tr.DeviceAddress = req->DeviceAddress;
  tr.Direction = DGF_BUS_DIR_READ;
  tr.TxData = tx;
  tr.TxLength = 1u;
  tr.RxData = rx;
  tr.RxLength = 2u;
  tr.TimeoutMs = 0u;

  ret = Dgf_Bus_Transaction(&tr, error);
  if (ret == E_OK) {
    *req->Data = (uint16)(((uint16)rx[0] << 8) | (uint16)rx[1]);
  }
  return ret;
}

static inline Std_ReturnType Dgf_Bus_I2cWrite16(const Dgf_I2cWrite16RequestType* req, Dgf_BusErrorType* error)
{
  uint8 tx[3];
  Dgf_BusTransactionType tr;

  if (req == NULL_PTR) {
    if (error != NULL_PTR) {
      *error = DGF_BUS_ERR_PARAM;
    }
    return E_NOT_OK;
  }

  tx[0] = req->RegisterAddress;
  tx[1] = (uint8)((req->Data >> 8) & 0xFFu);
  tx[2] = (uint8)(req->Data & 0xFFu);

  tr.Protocol = DGF_BUS_PROTO_I2C;
  tr.BusId = req->BusId;
  tr.DeviceAddress = req->DeviceAddress;
  tr.Direction = DGF_BUS_DIR_WRITE;
  tr.TxData = tx;
  tr.TxLength = 3u;
  tr.RxData = NULL_PTR;
  tr.RxLength = 0u;
  tr.TimeoutMs = 0u;

  return Dgf_Bus_Transaction(&tr, error);
}

#endif /* DGF_BUS_PORT_H */
