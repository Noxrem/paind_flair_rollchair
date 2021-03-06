/*
 * application.c
 *
 *  Created on: 21.10.2021
 *      Author: Noxrem
 */

#include "application.h"
#include "platform.h"
#include <stdio.h>
#include <stdint.h>
#include "McuRTOS.h"
#include "myTasks.h"
#include "Shell.h"


void APP_Run(void)
{
	printf("Welcome to FLAIR!\n");	// semihosting
	PL_Init();			// Initialize the platform

	//MyTasks_Init();		// creates task(s) and runs scheduler

	SHELL_Init();	// Initialize the shell

	// Start RTOS
	vTaskStartScheduler();
	// does not get here
	while(1){}	// (endless loop)
}
