/*
 * McuAdc.h
 *
 *  Created on: 03.12.2021
 *      Author: Noxrem
 */

#ifndef MCUADC_H_
#define MCUADC_H_

#include <stdint.h>

typedef enum ADC_Channel_e {
	ADC_ADC0_8,
	ADC_ADC0_9
} ADC_Channel_e;

/*!
 * \brief Initialize a adc channel
 * \param adcCh Sets the channel from ADC_Channel_e to be set up
 */
uint8_t McuAdc_Get_Value(ADC_Channel_e adcCh, uint16_t *val);
void McuAdc_Channel_Init(ADC_Channel_e adcCh);
void McuAdc_Init(void);
void McuAdc_Deinit(void);

#endif /* MCUADC_H_ */
