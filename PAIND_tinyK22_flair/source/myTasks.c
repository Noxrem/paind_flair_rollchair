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
#include "LED.h"

static uint32_t blinkyTaskParam = 5;
static TaskHandle_t blinkyTaskHndl = NULL;

static void blinkyTask(void *pv)
{
	while(1)
	{
		LED_On();
		mySystemView_Print("Blue LED on\n\r");
		vTaskDelay(pdMS_TO_TICKS(500));		// wait for 500 ms
		LED_Off();
		mySystemView_Print("Blue LED off\n\r");
		vTaskDelay(pdMS_TO_TICKS(500));		// wait for 500 ms
	}
}

void MyTasks_Init(void)
{
	McuRTOS_Init();	// Initialize RTOS
	mySystemView_Init();	// Initialize system view
	LED_Init();

	/* myTask1 */
	BaseType_t res;
	res = xTaskCreate(blinkyTask, "blinkyTask", 650/sizeof(StackType_t), &blinkyTaskParam, tskIDLE_PRIORITY+1, &blinkyTaskHndl);
	if(res!=pdPASS)
	{
		/* error */
		while(1){}
	}

}
