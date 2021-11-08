/*
 * Copyright (c) 2021, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef CTIMER_H_
#define CTIMER_H_

#if defined(CPU_LPC845M301JBD48)
#include "fsl_ctimer.h"

#define CTIMER                  CTIMER0         /* Timer 0 */
#define CT_TIMER_GET_VALUE()    CTIMER_GetTimerCountValue(CTIMER)
#define CT_TIMER_RESET()        CTIMER_Reset(CTIMER)

void CT_Test(void);

void CT_Init(void);
#endif /* LPC */

#endif /* CTIMER_H_ */
