/*
 * myTasks.c
 *
 *  Created on: 21.10.2021
 *      Author: Noxrem
 */

#include "myTasks.h"
#include "McuRTOS.h"
#include <stdint.h>
#include <stdio.h>
#include "mySystemView.h"
#include "fsr_sensor.h"

static uint32_t fsrSensorTaskParam = 5;
static TaskHandle_t fsrSensorTaskHndl = NULL;

static void fsrSensorTask(void *pv)
{
	uint16_t valueRight;
	uint16_t valueLeft;
	while(1)
	{
		FSR_Sensor_Right_Get_Value(&valueRight);
		FSR_Sensor_Left_Get_Value(&valueLeft);
		printf("FSR Sensor Right: %d\n\r", valueRight);
		printf("FSR Sensor Left: %d\n\r", valueLeft);
		vTaskDelay(pdMS_TO_TICKS(10));		// wait for 500 ms
	}
}

void MyTasks_Init(void)
{
	/* myTask1 */
	BaseType_t res;
	res = xTaskCreate(fsrSensorTask, "fsrSensorTask", 700/sizeof(StackType_t), &fsrSensorTaskParam, tskIDLE_PRIORITY+1, &fsrSensorTaskHndl);
	if(res!=pdPASS)
	{
		/* error */
		while(1){}
	}

}
