/*
 * application.c
 *
 *  Created on: 21.10.2021
 *      Author: Noxrem
 */

#include "application.h"
#include <stdio.h>
#include "McuRTOS.h"
#include "myTasks.h"


void APP_Run(void)
{
	printf("Welcome to FLAIR!\n");	// semihosting


	MyTasks_Init();		// creates task(s) and runs scheduler

	// Start RTOS
	vTaskStartScheduler();
	// does not get here
	while(1){}	// (endless loop)
}
