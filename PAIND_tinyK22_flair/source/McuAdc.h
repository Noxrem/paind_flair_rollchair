/*
 * McuAdc.h
 *
 *  Created on: 03.12.2021
 *      Author: Noxrem
 */

#ifndef MCUADC_H_
#define MCUADC_H_

#include <stdint.h>

/*
 * \brief The ADC channels to chose from.
 */
typedef enum ADC_Channel_e {
	ADC_ADC0_8,
	ADC_ADC0_9
} ADC_Channel_e;

/*
 * \brief Get the numeric ADC value of a specific ADC channel.
 * \param adcCh Chose the ADC channel
 * \param out The destination the value will be saved in
 * \return McuLib error code
 */
uint8_t McuAdc_Get_Value(ADC_Channel_e adcCh, uint16_t *val);

/*!
 * \brief Initialize a adc channel
 * \param adcCh Sets the channel from ADC_Channel_e to be set up
 */
void McuAdc_Channel_Init(ADC_Channel_e adcCh);

/*
 * \brief Initialize the ADC Module.
 */
void McuAdc_Init(void);
/*
 * \brief Deinitialize the ADC Module.
 */
void McuAdc_Deinit(void);

#endif /* MCUADC_H_ */
