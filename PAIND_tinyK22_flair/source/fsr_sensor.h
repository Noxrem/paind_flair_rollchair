/*
 * fsr_sensor.h
 *
 *  Created on: 04.12.2021
 *      Author: Noxrem
 */

#ifndef FSR_SENSOR_H_
#define FSR_SENSOR_H_
#include <stdint.h>
#include <stdbool.h>
#include "McuShell.h"

/*
 * \brief Parse the commands handed by the shell.
 * \param cmd Pointer to command string
 * \param handled Pointer to variable to indicate if the command has been handled
 * \param io Pointer to I/O callbacks
 * \return Error code
 */
uint8_t FSR_Sensor_ParseCommand(const uint8_t *cmd, bool *handled, McuShell_ConstStdIOType *io);

/*
 * \brief Gets the value of the right FSR
 * \param val Pointer to the destination where the value should be saved
 * \return Error code
 */
uint8_t FSR_Sensor_Right_Get_Value(uint16_t *val);

/*
 * \brief Gets the value of the left FSR
 * \param val Pointer to the destination where the value should be saved
 * \return Error code
 */
uint8_t FSR_Sensor_Left_Get_Value(uint16_t *val);

void FSR_Sensor_Init(void);
void FSR_Sensor_Deinit(void);

#endif /* FSR_SENSOR_H_ */
