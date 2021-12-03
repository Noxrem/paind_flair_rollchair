/*
 * McuAdc.c
 *
 *  Created on: 03.12.2021
 *      Author: Noxrem
 */

#include "McuAdc.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_adc16.h"
#include <stdio.h>
#include "McuShell.h"

/*******************************************************************************
 * Definitions
 */
/* FSR (force sensing resistor) trigger right */
#define TRIGR_ADC16_BASE          ADC0
#define TRIGR_ADC16_CHANNEL_GROUP 0U
#define TRIGR_ADC16_USER_CHANNEL  8U

/* FSR (force sensing resistor) trigger left */
#define TRIGL_ADC16_BASE          ADC0
#define TRIGL_ADC16_CHANNEL_GROUP 0U
#define TRIGL_ADC16_USER_CHANNEL  9U

/*******************************************************************************
 * Variables
 */
const uint32_t g_Adc16_12bitFullRange = 4096U;

adc16_channel_config_t adc16ChannelConfigStructTrigR;
adc16_channel_config_t adc16ChannelConfigStructTrigL;

/*******************************************************************************
 * Methods
 */
uint8_t McuAdc_Get_Value(ADC_Channel_e adcCh, uint16_t *val)
{
	ADC_Type base;
	uint32_t chGroup;
	adc16_channel_config_t config;

	switch(adcCh)
	{
		case ADC_TRIG_R:
			base = TRIGR_ADC16_BASE;
			chGroup = TRIGR_ADC16_CHANNEL_GROUP;
			config = adc16ChannelConfigStructTrigR;
			break;
		case ADC_TRIG_L:
			base = TRIGL_ADC16_BASE;
			chGroup = TRIGL_ADC16_CHANNEL_GROUP;
			config = adc16ChannelConfigStructTrigL;
			break;
		default:
			return ERR_FAILED;
			break;
	}
	ADC16_SetChannelConfig(base, chGroup, &config);
	while (0U == (kADC16_ChannelConversionDoneFlag &
				  ADC16_GetChannelStatusFlags(base, chGroup)))
	{
	}
	*val = ADC16_GetChannelConversionValue(base, chGroup);	// return the adc conversion value
	return ERR_OK;
}

void McuAdc_Channel_Init(ADC_Channel_e adcCh)
{
	uint32_t chNum;	// channel number of the adc

	switch(adcCh)
	{
		case ADC_TRIG_R:
			chNum = TRIGR_ADC16_USER_CHANNEL;
			break;
		case ADC_TRIG_L:
			chNum = TRIGL_ADC16_USER_CHANNEL;
			break;
		default:
			for(;;) {} /* error */
			break;
	}
    adc16ChannelConfigStruct.channelNumber                        = chNum;
    adc16ChannelConfigStruct.enableInterruptOnConversionCompleted = false;
	#if defined(FSL_FEATURE_ADC16_HAS_DIFF_MODE) && FSL_FEATURE_ADC16_HAS_DIFF_MODE
		adc16ChannelConfigStruct.enableDifferentialConversion = false;
	#endif /* FSL_FEATURE_ADC16_HAS_DIFF_MODE */
}

void McuAdc_Init(void)
{
	adc16_config_t adc16ConfigStruct;

	ADC16_GetDefaultConfig(&adc16ConfigStruct);
	#ifdef BOARD_ADC_USE_ALT_VREF
		adc16ConfigStruct.referenceVoltageSource = kADC16_ReferenceVoltageSourceValt;
	#endif
	ADC16_Init(MCUADC_ADC16_BASE, &adc16ConfigStruct);
	ADC16_EnableHardwareTrigger(MCUADC_ADC16_BASE, false);	// Make sure the software trigger is used
	#if defined(FSL_FEATURE_ADC16_HAS_CALIBRATION) && FSL_FEATURE_ADC16_HAS_CALIBRATION
		if (kStatus_Success == ADC16_DoAutoCalibration(MCUADC_ADC16_BASE))
		{
			printf("ADC16_DoAutoCalibration() Done.\r\n");
		}
		else
		{
			printf("ADC16_DoAutoCalibration() Failed.\r\n");
			for(;;) {}	// error
		}
	#endif // FSL_FEATURE_ADC16_HAS_CALIBRATION
	printf("ADC Full Range: %d\r\n", g_Adc16_12bitFullRange);

	adc16ChannelConfigStruct.channelNumber = TRIGR_ADC16_USER_CHANNEL;

}

void McuAdc_Deinit(void)
{

}
