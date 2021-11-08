/*
 * Copyright (c) 2021, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#if defined(CPU_LPC845M301JBD48)
#include "ctimer.h"
#include "fsl_ctimer.h"

#define CTIMER            CTIMER0         /* Timer 0 */
#define CTIMER_MAT_OUT    kCTIMER_Match_1 /* Match output 1 */
#define CTIMER_CLK_FREQ   CLOCK_GetFreq(kCLOCK_CoreSysClk)

static void InitTimer(void) {
  ctimer_config_t config;
  ctimer_match_config_t matchConfig;

  CTIMER_GetDefaultConfig(&config);
  CTIMER_Init(CTIMER, &config);
  matchConfig.enableCounterReset = true;
  matchConfig.enableCounterStop  = false;
  matchConfig.matchValue         = CTIMER_CLK_FREQ;
  matchConfig.outControl         = kCTIMER_Output_NoAction;
  matchConfig.outPinInitState    = false;
  matchConfig.enableInterrupt    = false;
  CTIMER_SetupMatch(CTIMER, CTIMER_MAT_OUT, &matchConfig);
  CTIMER_StartTimer(CTIMER);
}

void CT_Init(void) {
  InitTimer();
}

#endif /* #if LPC */
