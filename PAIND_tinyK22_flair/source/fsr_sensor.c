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
#include "McuUtility.h"

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
//static McuShell_ConstStdIOType fsrSensorIOTaskParam;	// task param. that specifies to which io the data is being sent
static TaskHandle_t fsrSensorTaskHndl = NULL;
bool sendFSRValuesOverUart = false;				// if true the fsr values are continuously sent over the UART shell

/*******************************************************************************
 * Methods
 */

static uint8_t PrintStatus(McuShell_ConstStdIOType *io) {
	uint8_t buf[32];
	buf[0] = '\0';				// clear buffer
	uint16_t fsrRightValue;
	uint16_t fsrLeftValue;



	McuShell_SendStatusStr((const unsigned char*) "fsr",
			(const unsigned char*) "FSR sensor module status\r\n", io->stdOut);
	/* right fsr sensor value */
	//McuUtility_strcat(buf, sizeof(buf), (unsigned char*) "right: ");
	FSR_Sensor_Right_Get_Value(&fsrRightValue);						// get the values from the fsr sensors by reference
	McuUtility_strcatNum16u(buf, sizeof(buf), fsrRightValue);
	McuUtility_strcat(buf, sizeof(buf), (unsigned char*)"\r\n");

	McuShell_SendStatusStr((const unsigned char*) " right ", (const unsigned char*)buf, io->stdOut);
	buf[0] = '\0';													// clear buffer
	/* left fsr sensor value */
	FSR_Sensor_Left_Get_Value(&fsrLeftValue);						// get the values from the fsr sensors by reference
	McuUtility_strcatNum16u(buf, sizeof(buf), fsrLeftValue);
	McuUtility_strcat(buf, sizeof(buf), (unsigned char*)"\r\n");

	McuShell_SendStatusStr((const unsigned char*) " left ", (const unsigned char*)buf, io->stdOut);
	return ERR_OK;
}

static uint8_t PrintHelp(McuShell_ConstStdIOType *io) {
	McuShell_SendHelpStr((unsigned char*) "fsr",
			(const unsigned char*) "Group of FSR sensor commands\r\n", io->stdOut);
	McuShell_SendHelpStr((unsigned char*) "  help|status",
			(const unsigned char*) "Print help or status information\r\n",io->stdOut);
	McuShell_SendHelpStr((unsigned char*) "  value on|off",
			(const unsigned char*) "Get right and left FSR sensor values\r\n", io->stdOut);
	return ERR_OK;
}

uint8_t FSR_Sensor_ParseCommand(const uint8_t *cmd, bool *handled,
		McuShell_ConstStdIOType *io) {
	if (McuUtility_strcmp((char*)cmd, McuShell_CMD_HELP) == 0
			|| McuUtility_strcmp((char*)cmd, "fsr help") == 0) {
		*handled = TRUE;
		return PrintHelp(io);
	}
	else if ((McuUtility_strcmp((char*)cmd, McuShell_CMD_STATUS) == 0)
			|| (McuUtility_strcmp((char*)cmd, "fsr status") == 0)) {
		*handled = TRUE;
		return PrintStatus(io);
	}
	else if (McuUtility_strcmp((char*)cmd, "fsr value on") == 0) {
		*handled = TRUE;
		sendFSRValuesOverUart = true;	// send continuous fsr values over UART shell
		return ERR_OK;
	}
	else if (McuUtility_strcmp((char*)cmd, "fsr value off") == 0) {
		*handled = TRUE;
		sendFSRValuesOverUart = false;	// stop sending continuous fsr values
		return ERR_OK;
	}
	return ERR_OK; /* no error */
}

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

	uint8_t buf[42]; 	// buffer needs to hold max. : fsr sensor right: 65555 left: 65555\r\n\0
	while(1)
	{
		FSR_Sensor_Right_Get_Value(&valueRight);
		FSR_Sensor_Left_Get_Value(&valueLeft);
		/* send continuous fsr sensor values to the UART shell if enabled */
		if(sendFSRValuesOverUart)
		{
			McuUtility_strcpy(buf, sizeof(buf), (const unsigned char*)"fsr sensor right: ");
			McuUtility_strcatNum16u(buf, sizeof(buf), valueRight);
			McuUtility_strcat(buf, sizeof(buf), (const unsigned char*)" left: ");
			McuUtility_strcatNum16u(buf, sizeof(buf), valueLeft);
			McuUtility_strcat(buf, sizeof(buf), (unsigned char*)"\r\n");
			McuShell_SendStr(buf, McuShell_GetStdio()->stdOut);		// send sensor values to stdOut
		}
		//printf("FSR Sensor Right: %4u Left: %4u\r\n", valueRight, valueLeft);	// format the values that they are preceded with blanks
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
	res = xTaskCreate(fsrSensorTask, "fsrSensorTask", 700/sizeof(StackType_t), NULL, tskIDLE_PRIORITY+1, &fsrSensorTaskHndl);
	if(res!=pdPASS)
	{
		/* error */
		while(1){}
	}
}

void FSR_Sensor_Deinit(void)
{

}

