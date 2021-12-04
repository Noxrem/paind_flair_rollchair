/*
 * fsr_sensor.c
 *
 *  Created on: 04.12.2021
 *      Author: Noxrem
 */

#include "fsr_sensor.h"
#include "McuLib.h"
#include "McuAdc.h"

/*******************************************************************************
 * Defines
 */
#define FSR_R_ADC_CHANNEL		ADC_ADC0_8	// set right trigger to adc0 channel 8
#define FSR_L_ADC_CHANNEL		ADC_ADC0_9	// set left trigger to adc0 channel 9

/*******************************************************************************
 * Variables
 */
/* FSR triggers */
ADC_Channel_e FSR_R;	// right trigger
ADC_Channel_e FSR_L;	// left trigger

/*******************************************************************************
 * Methods
 */
uint8_t FSR_Sensor_Right_Get_Value(uint16_t *val)
{
	return McuAdc_Get_Value(FSR_R, val);
}

uint8_t FSR_Sensor_Left_Get_Value(uint16_t *val)
{
	return McuAdc_Get_Value(FSR_L, val);
}

void FSR_Sensor_Init(void)
{
	FSR_R = FSR_R_ADC_CHANNEL;
	FSR_L = FSR_L_ADC_CHANNEL;

	McuAdc_Channel_Init(FSR_R);
	McuAdc_Channel_Init(FSR_L);
}

void FSR_Sensor_Deinit(void)
{

}

