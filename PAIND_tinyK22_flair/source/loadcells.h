/**
 *--------------------------------------------------------------------
 *					PAIND SAW Deployment								  
 *--------------------------------------------------------------------
 * 
 * \brief         This is the header file acting as the interface of
 * 					the load cell module.
 * \file		  loadcells.h
 * \author        GabrielGertsch
 * \date          Oct 12, 2020
 * \copyright     Copyright 2020 Gabriel Gertsch. 
 *                This file is released under the 3-clause BSD license.
 *
 *--------------------------------------------------------------------
 */


#ifndef LOADCELLS_H_
#define LOADCELLS_H_

#define LOADCELL1_D_GPIO			GPIOB
#define LOADCELL1_D_PORT			PORTB
#define LOADCELL1_D_PIN				17

#define LOADCELL_D_IRQ_HANDELER	PORTB_IRQHandler


#define LOADCELL1_CLK_GPIO			GPIOC
#define LOADCELL1_CLK_PORT			PORTC
#define LOADCELL1_CLK_PIN			8

#define BLUE_LED_GPIO				GPIOC
#define BLUE_LED_PORT				PORTC
#define BLUE_LED_PIN				2

extern QueueHandle_t loadCell1_QueueHandle;

void LoadCell_Init(void);
void LoadCell_Deinit(void);
void LoadCell_Start(void);
void LoadCell_Stop(void);
void LoadCell_PrintLoad(uint16_t samples);
//void LoadCell_UpdateControl(int32_t Limit, int16_t PwmPermil);



#endif /* LOADCELLS_H_ */
