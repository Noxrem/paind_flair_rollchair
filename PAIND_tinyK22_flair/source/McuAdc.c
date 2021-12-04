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
/* ADC0 base adress */
#define ADC16_ADC0_BASE			  ADC0

/* ADC0 channel 8  at PTB0 */
#define ADC16_ADC0_8_CHANNEL_GROUP 0U
#define ADC16_ADC0_8_USER_CHANNEL  8U

/* ACD0 channel 9 at PTB1 */
#define ADC16_ADC0_9_CHANNEL_GROUP 0U
#define ADC16_ADC0_9_USER_CHANNEL  9U

/*******************************************************************************
 * Variables
 */
const uint32_t g_Adc16_12bitFullRange = 4096U;

adc16_channel_config_t adc16ChannelConfigStructCh8;
adc16_channel_config_t adc16ChannelConfigStructCh9;

/*******************************************************************************
 * Methods
 */
uint8_t McuAdc_Get_Value(ADC_Channel_e adcCh, uint16_t *val)
{
	ADC_Type *base;
	uint32_t chGroup;
	adc16_channel_config_t config;

	switch(adcCh)
	{
		case ADC_ADC0_8:
			base = ADC16_ADC0_BASE;
			chGroup = ADC16_ADC0_8_CHANNEL_GROUP;
			config = adc16ChannelConfigStructCh8;
			break;
		case ADC_ADC0_9:
			base = ADC16_ADC0_BASE;
			chGroup = ADC16_ADC0_9_CHANNEL_GROUP;
			config = adc16ChannelConfigStructCh9;
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
	switch(adcCh)
	{
		case ADC_ADC0_8:
		    adc16ChannelConfigStructCh8.channelNumber                        = ADC16_ADC0_8_USER_CHANNEL;
		    adc16ChannelConfigStructCh8.enableInterruptOnConversionCompleted = false;
			#if defined(FSL_FEATURE_ADC16_HAS_DIFF_MODE) && FSL_FEATURE_ADC16_HAS_DIFF_MODE
				adc16ChannelConfigStructCh8.enableDifferentialConversion = false;
			#endif /* FSL_FEATURE_ADC16_HAS_DIFF_MODE */
			break;
		case ADC_ADC0_9:
		    adc16ChannelConfigStructCh9.channelNumber                        = ADC16_ADC0_9_USER_CHANNEL;
		    adc16ChannelConfigStructCh9.enableInterruptOnConversionCompleted = false;
			#if defined(FSL_FEATURE_ADC16_HAS_DIFF_MODE) && FSL_FEATURE_ADC16_HAS_DIFF_MODE
				adc16ChannelConfigStructCh9.enableDifferentialConversion = false;
			#endif /* FSL_FEATURE_ADC16_HAS_DIFF_MODE */
			break;
		default:
			for(;;) {} /* error */
			break;
	}
}

void McuAdc_Init(void)
{
	adc16_config_t adc16ConfigStruct;

	ADC16_GetDefaultConfig(&adc16ConfigStruct);
	#ifdef BOARD_ADC_USE_ALT_VREF
		adc16ConfigStruct.referenceVoltageSource = kADC16_ReferenceVoltageSourceValt;
	#endif
	ADC16_Init(ADC16_ADC0_BASE, &adc16ConfigStruct);
	ADC16_EnableHardwareTrigger(ADC16_ADC0_BASE, false);	// Make sure the software trigger is used
	#if defined(FSL_FEATURE_ADC16_HAS_CALIBRATION) && FSL_FEATURE_ADC16_HAS_CALIBRATION
		if (kStatus_Success == ADC16_DoAutoCalibration(ADC16_ADC0_BASE))
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
}

void McuAdc_Deinit(void)
{

}
