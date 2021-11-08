/*
 * LED.c
 *
 *  Created on: 17.10.2021
 *      Author: Noxrem
 */

#include "LED.h"
#include "McuLib.h"
#include "IncludeMcuLibConfig.h"
#include "McuLED.h"
#include "McuWait.h"
#include "pin_mux.h"

/* Data */
McuLED_Handle_t led;

/* Methods */
void LED_Init(void)
{
	McuWait_Init();

	#if McuLib_CONFIG_CPU_IS_KINETIS
		//CLOCK_EnableClock(kClock_PortC);	// On Kinesis the Clock for the PortB needs to be enabled
	#elif McuLib_CONFIG_CPU_IS_LPC
		GPIO_PortInit(GPIO, 1);				// On LPC845 the Port needs to be initialized
	#endif

	McuLED_Config_t config;
	McuLED_GetDefaultConfig(&config);		// Get the default LED configuration

	#if McuLib_CONFIG_CPU_IS_KINETIS
		config.isLowActive = true;
		config.hw.gpio = BOARD_INITPINS_LED_BLUE_GPIO;
		config.hw.port = BOARD_INITPINS_LED_BLUE_PORT;				// LED at PTC2
		config.hw.pin = BOARD_INITPINS_LED_BLUE_PIN;
	#elif McuLib_CONFIG_CPU_IS_LPC
		config.isLowActive = true;
		config.hw.gpio = BOARD_INITPINS_LED_BLUE_GPIO;
		config.hw.port = BOARD_INITPINS_LED_BLUE_PORT;				// LED at PIO1_1
		config.hw.pin = BOARD_INITPINS_LED_BLUE_PIN;
		config.hw.iocon = IOCON_INDEX_PIO1_1;						// I/O connection used for muxing
	#endif

	led = McuLED_InitLed(&config);
	if(led == NULL)	// check if the handle has been created
	{
		while(1);	/* Error */
	}
}

void LED_Deinit(void)
{
	led = McuLED_DeinitLed(led);	// deinitializes the led handle and returns NULL pointer
	McuWait_Deinit();
}

void LED_On(void)
{
	McuLED_On(led);
}

void LED_Off(void)
{
	McuLED_Off(led);
}

void LED_Toggle(void)
{
	McuLED_Toggle(led);
}

// Lets the LED blink in an endless loop
void LED_Blinky(void)
{
	while(1)
	{
		LED_On();
		McuWait_Waitms(1000);	// Wait 1 s
		LED_Off();
		McuWait_Waitms(1000);	// Wait 1 s
	}
}
