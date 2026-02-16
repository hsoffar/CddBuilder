#ifndef DGF_TIME_PORT_H
#define DGF_TIME_PORT_H

#include "Std_Types.h"

/*
 * Generic Driver Framework (DGF) time abstraction.
 * Implementation can be wired to OS counter, GPT, STM, or virtual test clock.
 */

typedef uint32 Dgf_TimeMsType;

Dgf_TimeMsType Dgf_Time_GetMonotonicMs(void);

#endif /* DGF_TIME_PORT_H */
