/*
	File "user_functions.h"
*/

#ifndef __USER_FUNCTION_H
#define __USER_FUNCTION_H

#include "stm32f4xx_hal.h"

extern uint16_t SystemStateRegister;
extern uint16_t SystemControlRegister;
extern uint8_t buffer_clock[];

#define BIT_READ_REGISTER 	0x01
#define BIT_WRITE_REGISTER 	0x02
#define BIT_RECIEVE_MESSAGE 0x04

#define CLOCK_BUFFER_SIZE (18)
#define I2C_ADDRESS (0xD0)

void LedControl(uint8_t led);
HAL_StatusTypeDef RTC_SetDateTime(uint8_t sec,uint8_t min,uint8_t hour,uint8_t date,uint8_t month,uint8_t year) ;
HAL_StatusTypeDef RTC_GetDateTimeToArray(uint8_t *ptr);
void RTC_GetDateTime(void);

#endif
