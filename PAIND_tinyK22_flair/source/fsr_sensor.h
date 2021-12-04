/*
 * fsr_sensor.h
 *
 *  Created on: 04.12.2021
 *      Author: Noxrem
 */

#ifndef FSR_SENSOR_H_
#define FSR_SENSOR_H_
#include <stdint.h>

uint8_t FSR_Sensor_Right_Get_Value(uint16_t *val);
uint8_t FSR_Sensor_Left_Get_Value(uint16_t *val);
void FSR_Sensor_Init(void);
void FSR_Sensor_Deinit(void);

#endif /* FSR_SENSOR_H_ */
