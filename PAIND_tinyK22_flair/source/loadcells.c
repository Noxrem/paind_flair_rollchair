/**
 *--------------------------------------------------------------------
 *					PAIND SAW Deployment								  
 *--------------------------------------------------------------------
 * 
 * \brief         This is the source file containing everything
 * 					concerning reading and transmitting of load cell data.
 * \file		  loadcells.c
 * \author        GabrielGertsch
 * \date          Oct 12, 2020
 * \copyright     Copyright 2020 Gabriel Gertsch. 
 *                This file is released under the 3-clause BSD license.
 *
 *--------------------------------------------------------------------
 */


#include "McuRTOS.h"
#include "McuGPIO.h"
#include "McuLED.h"
#include "McuWait.h"
#include "fsl_port.h"
#include <stdbool.h>
#include "loadcells.h"
//#include "gateway.h"
#include "stdio.h"
#include "McuCriticalSection.h"
//#include "motors.h"
#include "McuUtility.h"

#define LOWTIME_NS		250
#define HIGHTIME_NS		250

#define HIGHLIMIT		1400
#define LOWLIMIT		-HIGHLIMIT
#define PWMPERMIL		200
#define DATASIZE		sizeof(int32_t)

static McuGPIO_Handle_t loadcell1_Data;
static McuGPIO_Handle_t loadcell1_Clk;
static McuLED_Handle_t led_Blue;
static TaskHandle_t loadCell_TaskHandle;
QueueHandle_t loadCell1_QueueHandle;
QueueHandle_t printerQueue_Handle;


int32_t Control_Hysteresis(int32_t error, int32_t amplitude, int32_t limit){
	if(error>limit){
		return -amplitude;
	}else if(error<(-limit)){
		return amplitude;
	}else{
		return 0;
	}
}

int32_t Control_Proportional(int32_t error, int32_t numerator, int32_t denumerator){
	return -error*numerator/denumerator;
}


//Controller which uses loadcell data.
static void Controler(void *pv) {

	BaseType_t result;
	int32_t offset = 0;
	int32_t oldOffset = 0;
	int32_t data = 0;

	Utils_SendStringToCharQueue(hostTxQueue, (const unsigned char *)"Start calibration\r\n");
	for(uint8_t i = 0; i<10;){
		oldOffset = offset;
		result = xQueueReceive(loadCell1_QueueHandle, &offset, pdMS_TO_TICKS(1000));
		if(result == errQUEUE_EMPTY){
			i = 0;
			offset = 0;
			Utils_SendStringToCharQueue(hostTxQueue, (const unsigned char *)"Timeout: Calibration restarted\r\n");
		}
		/// \todo Check if old offset and new one are in a plausible range of each other (stable value).
		if(1){
			i++;
		}
	}
	uint8_t str[35];
	str[0] = '\0';
	McuUtility_strcat(str, 35, (const unsigned char *)"Calibration offset: ");
	McuUtility_strcatNum32s(str, 35, offset);
	McuUtility_strcat(str, 35, (const unsigned char *)"\n\r");

	Utils_SendStringToCharQueue(hostTxQueue, (const unsigned char *)str);

	for(;;){
		result = xQueueReceive(loadCell1_QueueHandle, &data, pdMS_TO_TICKS(200));
		if(result != pdPASS){
			motorSetPwmA(0);
		}else if(result == pdPASS){
			//motorSetPwmA(Control_Hysteresis((offset-data), 250, 4000));
			//motorSetPwmA(Control_Proportional((offset-data), 1,50));
					// Send data to the queue.
//					char str[20];
//					sprintf(str, "%d\r\n", (offset-data));
//
//					/// todo Check return and do error handling.
//					Utils_SendStringToCharQueue(hostTxQueue, (const unsigned char *)str);
		}
	}
}

//Sends loadcell data to hostTxQueue
static void LoadCellPrinter_Task(void *pv){
	uint16_t samples = (uint16_t)(pv);
	int32_t data = 0;
	BaseType_t result = pdPASS;

	//empty queue.
	while(result != errQUEUE_EMPTY){
		result = xQueueReceive(printerQueue_Handle, &data, 0);
	}


	for(uint16_t i = 0; i<samples; i++){
		xQueueReceive(printerQueue_Handle, &data, portMAX_DELAY );
		char str[20];
		sprintf(str, "%d\r\n", data);
		Utils_SendStringToCharQueue(hostTxQueue, (const unsigned char*)str);
	}
	vTaskDelete(NULL);
}


void LOADCELL_D_IRQ_HANDELER() {
	GPIO_PortClearInterruptFlags(LOADCELL1_D_GPIO, 1 << LOADCELL1_D_PIN);

	BaseType_t xHigherPriorityTaskWoken;

	/// \todo make sure data line is stable in low state.
	if(McuGPIO_IsLow(loadcell1_Data)){
		// Notify load cell task that the data line was pulled low but only if the task is not suspended.
			xTaskNotifyFromISR(loadCell_TaskHandle, 0, eNoAction, &xHigherPriorityTaskWoken);
	}

	// If the load cell task has a better priority than the currently running task then yield.
	if (xHigherPriorityTaskWoken != pdFALSE) {
		vPortYieldFromISR();
	}

	__DSB();
}

/**
 * \brief This function is a FreeRTOS task. It handles the reception of data from the load cells.
 * \remark If more load cells are attached then one should do all conversions simultaneously to save dead time.
 * \param [in] pv is not used but has to be present for FreeRTOS.
 */
static void LoadCell_Task(void *pv) {

	status_t interruptResult = kStatus_Fail;
	int32_t data = 0;

	McuCriticalSection_CriticalVariable();

	for (;;) {
		//Wait until data line is pulled low (notification from isr).
		xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);

		//Should not be disturbed during this.
		McuCriticalSection_EnterCritical();

		//Wait 100ns before starting read cycle (from data sheet of hx711).
		McuWait_Waitns(HIGHTIME_NS);
		//Disable PORTB IRQ because data line is used for data now.
		interruptResult = DisableIRQ(PORTB_IRQn);
		if(interruptResult != kStatus_Success){
			__asm("BKPT");
		}


		//For loop to receive data, which takes approximately 24*(hightime + lowtime) = 24*0.4us = 9.6us.
		for (int8_t i = 23; i >= 0; i--) {
			//Set clock line high to indicate I am ready to read the current bit.
			McuGPIO_SetHigh(loadcell1_Clk);

			//High time is 200ns, so wait that (from data sheet of hx711).
			McuWait_Waitns(150);

			//Read data and set the corresponding bit of data.
			data |= McuGPIO_IsHigh(loadcell1_Data) << i;

			//Set clock line low to be able to set it high afterwards.
			McuGPIO_SetLow(loadcell1_Clk);

			//Low time is 200ns, so wait that (from data sheet of hx711).
			McuWait_Waitns(LOWTIME_NS);
		}

		if(data & (1<<23)){
			data |= 0xFF000000;
		}

		//Send one more pulse of the clock line to control the gain of the next conversion.
		//1 pulse: Channel A and gain 128
		//2 pulses: Channel B and gain 32
		//3 pulses: Channel A and gain 64
		McuGPIO_SetHigh(loadcell1_Clk);
		McuWait_Waitns(HIGHTIME_NS);
		McuGPIO_SetLow(loadcell1_Clk);
		McuWait_Waitns(LOWTIME_NS);
		McuGPIO_SetHigh(loadcell1_Clk);
		McuWait_Waitns(200);
		McuGPIO_SetLow(loadcell1_Clk);
		McuWait_Waitns(200);
		McuGPIO_SetHigh(loadcell1_Clk);
		McuWait_Waitns(200);
		McuGPIO_SetLow(loadcell1_Clk);
		McuWait_Waitns(200);

//		if(i==100){
//			__asm("BKPT");
//		}
//		d[i] = data;
//		i++;

		McuCriticalSection_ExitCritical();
//
		/// todo Check return and do error handling.

		//Send data to data queue which is used by the controler.
		xQueueSend(loadCell1_QueueHandle, &data, portMAX_DELAY);
		xQueueSend(printerQueue_Handle, &data, 0);

		data = 0;


		//Clear interrupt before enabling it again (not sure if isr flag is set while it is disabled).
		GPIO_PortClearInterruptFlags(LOADCELL1_D_GPIO, 1 << LOADCELL1_D_PIN);

		interruptResult = EnableIRQ(PORTB_IRQn);
		if(interruptResult != kStatus_Success){
			__asm("BKPT");
		}
	}
}

/**
 * \brief  \todo Write stuff.
 */
void LoadCell_Init(void){

	/* Port B Clock Gate Control: Clock enabled */
	CLOCK_EnableClock(kCLOCK_PortB);
	/* Port C Clock Gate Control: Clock enabled */
	CLOCK_EnableClock(kCLOCK_PortC);

	McuGPIO_Init();
	McuLED_Init();
	McuWait_Init();
	McuCriticalSection_Init();
	McuUtility_Init();

	McuGPIO_Config_t gpio_config;

	McuGPIO_GetDefaultConfig(&gpio_config);

	// Configure data line of load cell 1;
	gpio_config.isInput = true;
	gpio_config.hw.gpio = LOADCELL1_D_GPIO;
	gpio_config.hw.port = LOADCELL1_D_PORT;
	gpio_config.hw.pin = LOADCELL1_D_PIN;
	loadcell1_Data = McuGPIO_InitGPIO(&gpio_config);

	// Configure clock line of load cell 1.
	gpio_config.isInput = false;
	gpio_config.hw.gpio = LOADCELL1_CLK_GPIO;
	gpio_config.hw.port = LOADCELL1_CLK_PORT;
	gpio_config.hw.pin = LOADCELL1_CLK_PIN;
	loadcell1_Clk = McuGPIO_InitGPIO(&gpio_config);

	McuLED_Config_t led_config;

	McuLED_GetDefaultConfig(&led_config);
	led_config.isLowActive = true;
	led_config.isOnInit = false;
	led_config.hw.gpio = BLUE_LED_GPIO;
	led_config.hw.port = BLUE_LED_PORT;
	led_config.hw.pin = BLUE_LED_PIN;
	led_Blue = McuLED_InitLed(&led_config);

	if (loadcell1_Clk == NULL || loadcell1_Data == NULL || led_Blue == NULL) {
		/*error handling */
		for (;;) {
		}
	}

	//Set pull-resistor after error handling.
	McuGPIO_SetPullResistor(loadcell1_Data, McuGPIO_PULL_UP);

	// Configure interrupt of data line of load cell 1.
	PORT_SetPinInterruptConfig(LOADCELL1_D_PORT, LOADCELL1_D_PIN, kPORT_InterruptFallingEdge);
	/// \bug If i disabled the load cell module and then enable it again, I get a hardfault here.
	EnableIRQ(PORTB_IRQn);
	NVIC_SetPriority(PORTB_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY+1);

	//Initiate load cell 1 data queue.
	loadCell1_QueueHandle = xQueueCreate(100, DATASIZE);
	if(loadCell1_QueueHandle == NULL){
		for(;;){
			//I'm trapped.
		}
	}
	vQueueAddToRegistry(loadCell1_QueueHandle, (const char*)"Loadcell1Queue");


	printerQueue_Handle = xQueueCreate(500, DATASIZE);
	if(printerQueue_Handle == NULL){
		for(;;){
			//I'm trapped.
		}
	}
	vQueueAddToRegistry(printerQueue_Handle, (const char*)"printerQueue");

	// Initiate load cell task.
	BaseType_t result;
	result = xTaskCreate(LoadCell_Task, "LoadCell_Task", 500/sizeof(StackType_t), NULL, tskIDLE_PRIORITY+1, &loadCell_TaskHandle);
	if(result!=pdPASS){
		for(;;){
			//I'm trapped.
		}
	}
	vTaskSuspend(loadCell_TaskHandle);


	result = xTaskCreate(Controler, "Controler", 500/sizeof(StackType_t), NULL, tskIDLE_PRIORITY+1, NULL);
	if(result!=pdPASS){
		for(;;){
			//I'm trapped.
		}
	}
}


void LoadCell_Deinit(void){
	vTaskDelete(loadCell_TaskHandle);

	/// \remark Hmm, should I disable the IRQ but what if another module uses the same IRQ?
	DisableIRQ(PORTB_IRQn);

	McuGPIO_SetPullResistor(loadcell1_Data, McuGPIO_PULL_DISABLE);

	McuGPIO_DeinitGPIO(loadcell1_Data);
	McuGPIO_DeinitGPIO(loadcell1_Clk);
//	McuGPIO_Deinit();
//	McuUtility_Deinit();

	McuLED_DeinitLed(led_Blue);
//	McuLED_Deinit();

	/// \remark Hmm, should I disable the clocks to the ports but what if another module uses the same ports?

}

void LoadCell_Stop(void){
	vTaskSuspend(loadCell_TaskHandle);
}

void LoadCell_Start(void){
	vTaskResume(loadCell_TaskHandle);
}

void LoadCell_PrintLoad(uint16_t samples){
	BaseType_t result;
	result = xTaskCreate(LoadCellPrinter_Task, "LoadPrinter", 500/sizeof(StackType_t), (void *)samples, tskIDLE_PRIORITY+1, NULL);
	if(result!=pdPASS){
		for(;;){
			//I'm trapped.
		}
	}
}



