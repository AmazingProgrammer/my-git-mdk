/*

*/

//#include "stm32f4xx_hal.h"
#include "user_functions.h"
#include "modbus.h"
#include "timers.h"
#include "LCD.h"

uint16_t SystemStateRegister = 0;
uint16_t SystemControlRegister = 0;

RTC_TimeTypeDef cTime;
RTC_DateTypeDef cDate;

extern I2C_HandleTypeDef hi2c1;
extern RTC_HandleTypeDef hrtc;
//#define CLOCK_BUFFER_SIZE (18)
//#define I2C_ADDRESS (0xD0)
uint8_t buffer_clock[CLOCK_BUFFER_SIZE] = {0};
uint8_t seconds = 0, minutes = 0, hours = 0, day = 0, date = 0, month = 0, year = 0;
uint8_t x = 0;
char timestr[40];


void LedControl(uint8_t led)
{
	if (led == 0x00)
		return;
	if (led & 0x01)
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12,GPIO_PIN_SET); 
	else
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12,GPIO_PIN_RESET);
	if (led & 0x02)
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13,GPIO_PIN_SET); 
	else
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13,GPIO_PIN_RESET);
	if (led & 0x04)
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14,GPIO_PIN_SET); 
	else
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14,GPIO_PIN_RESET);
	if (led & 0x08)
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15,GPIO_PIN_SET); 
	else
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15,GPIO_PIN_RESET);
}

/*
void TimeReadProcess(void)
{
	static uint8_t flag_begin = 0;
	uint8_t mask = 0x0F;
	
	if (flag_begin == 0)
	{
		for (uint8_t x = 0; x < CLOCK_BUFFER_SIZE; x++)
		{
			buffer_clock[x] = 0;
		}
		flag_begin = 1;
	}
	else
	{
		if (GetSysTimer(LTIMER_SYS) >= 500)// Timer updates in 1 ms
		{
			ResetSysTimer(LTIMER_SYS);
			result = HAL_I2C_Mem_Read(&hi2c1, I2C_ADDRESS, 0x00, 1, buffer_clock, 7, 1000);
			//uint8_t HiMsg[]="Hello!!\r\n";
			//CDC_Transmit_FS(HiMsg,strlen(HiMsg));
			if (result == HAL_OK)
			{
				//seconds = buffer_clock[0] & 0x7F;
				seconds = (buffer_clock[0] >> 4) * 10;
				seconds += (buffer_clock[0] & 0x0F);
				minutes = (buffer_clock[1] >> 4) * 10;
				minutes += (buffer_clock[1] & 0x0F);
				hours = (buffer_clock[2] >> 4) * 10;
				hours += (buffer_clock[2] & 0x0F);
				date = (buffer_clock[4] >> 4) * 10;
				date += (buffer_clock[4] & 0x0F);
				month = (buffer_clock[5] >> 4) * 10;
				month += (buffer_clock[5] & 0x0F);
				year = (buffer_clock[6] >> 4) * 10;
				year += (buffer_clock[6] & mask);
				//uint8_t HiMsg[]="Hello!!\r\n";
				//CDC_Transmit_FS(HiMsg,strlen(HiMsg));
			}
			else
			{
				for (uint8_t x = 0; x < CLOCK_BUFFER_SIZE; x++)
				{
					buffer_clock[x] = 0;
				}
				seconds = 0; minutes = 0; hours = 0; year = 0;
			}
		}
	}
}
*/

/*

*/
void RTC_GetDateTime(void)
{
	
	if (GetSysTimer(LTIMER_SYS) >= 500)// Timer updates in 1 ms
	{
		ResetSysTimer(LTIMER_SYS);
		if ((HAL_RTC_GetTime(&hrtc, &cTime, RTC_FORMAT_BIN) == HAL_OK)&&(HAL_RTC_GetDate(&hrtc, &cDate, RTC_FORMAT_BIN) == HAL_OK))
		{
			sprintf(timestr, "Date: %02d/%02d/%04d - Time: %02d:%02d:%02d", cDate.Date, cDate.Month, cDate.Year + 2000, cTime.Hours, cTime.Minutes, cTime.Seconds);
			LCD_DisplayStringLine(20,20, timestr, LCD_COLOR_WHITE, LCD_COLOR_MAGENTA);
		}
	}
}

/*

*/
HAL_StatusTypeDef RTC_GetDateTimeToArray(uint8_t *ptr)
{
	
	if ((HAL_RTC_GetTime(&hrtc, &cTime, RTC_FORMAT_BIN) == HAL_OK)&&(HAL_RTC_GetDate(&hrtc, &cDate, RTC_FORMAT_BIN) == HAL_OK))
	{
		sprintf(timestr, "Date: %02d/%02d/%04d - Time: %02d:%02d:%02d", cDate.Date, cDate.Month, cDate.Year + 2000, cTime.Hours, cTime.Minutes, cTime.Seconds);
		*(ptr + AMOUNT_REGISTERS_BYTE + 1) = cTime.Seconds;
		*(ptr + AMOUNT_REGISTERS_BYTE + 3) = cTime.Minutes;
		*(ptr + AMOUNT_REGISTERS_BYTE + 5) = cTime.Hours;
		*(ptr + AMOUNT_REGISTERS_BYTE + 7) = cDate.Date;
		*(ptr + AMOUNT_REGISTERS_BYTE + 5) = cDate.Month;
		*(ptr + AMOUNT_REGISTERS_BYTE + 5) = cDate.Year;
		return HAL_OK;
	}
	return HAL_ERROR;
}

/*

*/
HAL_StatusTypeDef RTC_SetDateTime(uint8_t sec,uint8_t min,uint8_t hour,uint8_t date,uint8_t month,uint8_t year) 
{
	uint8_t res = 0;
	// get the date
	cDate.Date = date;
	cDate.Month = month;
	cDate.Year = year;
	cDate.WeekDay = RTC_WEEKDAY_MONDAY;

	// get the time
	cTime.Hours = hour;
	cTime.Minutes = min;
	cTime.Seconds = sec;

	// Set the Time and Date
	cTime.TimeFormat = RTC_HOURFORMAT_24;
	cTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	cTime.StoreOperation = RTC_STOREOPERATION_RESET;
	if (HAL_RTC_SetTime(&hrtc, &cTime, FORMAT_BIN) == HAL_OK)
		res++;
	if (HAL_RTC_SetDate(&hrtc, &cDate, FORMAT_BIN) == HAL_OK)
		res++;
	if (res == 2)
		return HAL_OK;
	else 
		return HAL_ERROR;
}

