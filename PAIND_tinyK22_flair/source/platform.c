/*
 * platform.c
 *
 *  Created on: 03.12.2021
 *      Author: Noxrem
 */

#include "platform.h"

#include "McuLib.h"
#include "McuRTT.h"
#include "McuSystemView.h"
#include "McuRTOS.h"
#include "McuAdc.h"
#include "McuShellUart.h"
#include "fsr_sensor.h"


void PL_Init(void)
{
	McuLib_Init();
	#if configUSE_SEGGER_SYSTEM_VIEWER_HOOKS
	  McuRTT_Init();
	  McuSystemView_Init();
	#endif
	McuRTOS_Init();
	McuAdc_Init();
	McuShellUart_Init();
	FSR_Sensor_Init();
}

void PL_Deinit(void)
{
	FSR_Sensor_Deinit();
	McuShellUart_Init();
	McuAdc_Deinit();
	McuRTOS_Deinit();
	#if configUSE_SEGGER_SYSTEM_VIEWER_HOOKS
		McuSystemView_Deinit();
		McuRTT_Deinit();
	#endif
	McuLib_Deinit();
}
