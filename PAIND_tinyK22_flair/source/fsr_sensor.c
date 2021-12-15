/*
 * fsr_sensor.c
 *
 *  Created on: 04.12.2021
 *      Author: Noxrem
 */

#include "fsr_sensor.h"
#include "McuLib.h"
#include "McuAdc.h"
#include "McuRTOS.h"
#include <stdint.h>
#include <stdio.h>
#include "mySystemView.h"

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

/* Task Variables */
static uint32_t fsrSensorTaskParam = 5;
static TaskHandle_t fsrSensorTaskHndl = NULL;

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

static void fsrSensorTask(void *pv)
{
	uint16_t valueRight;
	uint16_t valueLeft;
	while(1)
	{
		FSR_Sensor_Right_Get_Value(&valueRight);
		FSR_Sensor_Left_Get_Value(&valueLeft);
		printf("FSR Sensor Right: %4u Left: %4u\n\r", valueRight, valueLeft);	// format the values that they are preceded with blanks
		vTaskDelay(pdMS_TO_TICKS(10));		// wait for 500 ms
	}
}

void FSR_Sensor_Init(void)
{
	FSR_R = FSR_R_ADC_CHANNEL;
	FSR_L = FSR_L_ADC_CHANNEL;

	McuAdc_Channel_Init(FSR_R);
	McuAdc_Channel_Init(FSR_L);

	/* myTask1 */
	BaseType_t res;
	res = xTaskCreate(fsrSensorTask, "fsrSensorTask", 700/sizeof(StackType_t), &fsrSensorTaskParam, tskIDLE_PRIORITY+1, &fsrSensorTaskHndl);
	if(res!=pdPASS)
	{
		/* error */
		while(1){}
	}
}

void FSR_Sensor_Deinit(void)
{

}

