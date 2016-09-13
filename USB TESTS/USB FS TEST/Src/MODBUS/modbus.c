/*


*/

#include "modbus.h"
#include "usbd_cdc_if.h"
#include "user_functions.h"
#include "CM1K.h"
#include "LCD.h"
#include "stm32f4xx_hal_crc.h"

uint8_t rx_buffer[RX_BUFFER_SIZE];
uint8_t tx_buffer[TX_BUFFER_SIZE];	
uint16_t ModbusErrorRegister = 0;
uint8_t FlagModbusRecieveData = 0;
uint8_t Msg1[]="Function LEARN was done!!\r\n";
uint8_t Msg2[]="Function RECOGNIZE was done!!\r\n";
uint8_t Msg3[]="Function SAVE was done!!\r\n";
uint8_t Msg4[]="Function RESTORE was done!!\r\n";
uint8_t Msg5[]="Function TEST was done!!\r\n";
uint8_t buffer8[] = {0, 0};
uint16_t buffer16[] = {0};
HAL_StatusTypeDef result  = HAL_OK;
extern I2C_HandleTypeDef hi2c1;
extern RTC_HandleTypeDef hrtc;
extern RTC_TimeTypeDef cTime;
extern RTC_DateTypeDef cDate;

#define TEST_WITHOUT_CRC  // Comment for normal work!
#define CM1K_INTERFACE_SERIAL
//#define CM1K_INTERFACE_PARALLEL

enum ModbusStates mobus_states;
enum ModbusErrors modbus_errors;

//uint8_t state = 0, _state = 1, flag = 0;
//uint8_t crc_high_byte = 0, crc_low_byte = 0;
//uint8_t kop = 0;// Code of function
//uint16_t number_register = 0;
//uint16_t data = 0, data_length = 0;
//uint16_t crc = 0, _crc = 0;

/*
	* @brief  Process automat with Modbus control
  * Author  Andrew A. Ivanov 
	* 28/06/2016
  * @param  None
  * @retval None
*/
void ModbusProcess(void)
{
	static uint8_t state = INIT, _state = INIT+1, flag = 0;
	uint8_t crc_high_byte = 0, crc_low_byte = 0;
	uint8_t res = 0;
	static uint8_t kop = 0;// Code of function
	static uint16_t number_register = 0;
	//static uint8_t data_register = 0;
	uint16_t data_length = 0;
	uint16_t crc = 0, _crc = 0;
	
	if (_state != state) {_state = state; flag = 1;} else flag = 0;
	switch(state)
	{
		case INIT:
			BufferClear(rx_buffer);
			BufferClear(tx_buffer);
			//----------------------------------------------
			state = WAIT_MESSAGE;
		break;
		case 1:
			if (flag) 
			{
				LedControl(0);
				FlagModbusRecieveData = 0;
				LCD_DisplayStringLine(100,100,"WAIT COMMAND..", LCD_COLOR_WHITE, LCD_COLOR_BLUE);
			}
			//----------------------------------------------
			//if (RECIEVE_MESSAGE == BIT_RECIEVE_MESSAGE)
			if (FlagModbusRecieveData)
			{
				state = ADDRESS_CONTROL;
			}
		break;
		case ADDRESS_CONTROL:
			if (flag) LedControl(1);
			if (rx_buffer[ADDRESS_BYTE] == UNIT_ADDRESS)
				state = CRC_CONTROL;
			else 
			{
				BufferClear(rx_buffer);
				state = WAIT_MESSAGE;
			}
		break;
		case CRC_CONTROL://-------------------------------------------------------------------------------------
			kop = rx_buffer[KOP_BYTE];
			//
			if (kop == WRITE_REGISTERS) // Write a few registers
			{
				data_length = AMOUNT_REGISTERS_BYTE + 1 + rx_buffer[AMOUNT_REGISTERS_BYTE]*2;
				crc_low_byte = rx_buffer[data_length];
				crc_high_byte = rx_buffer[crc_low_byte + 1];
				crc = (crc_high_byte << 8) | crc_low_byte;
				_crc = CRCProcess(rx_buffer, data_length);
				#ifdef TEST_WITHOUT_CRC
				_crc = crc = 1;
				#warning No CRC control!!!!
				#endif
				if (_crc == crc)
				{
					if (rx_buffer[START_REGISTER_H_BYTE] == GRUP_CLOCK_REGISTERS) // Write to clock registers
					{
						state = MODE_WRITE_CLOCK;
					}
					else if (rx_buffer[START_REGISTER_H_BYTE] == GRUP_CM1K_REGISTERS) // Write to CM1K registers
					{
						state = MODE_WRITE_CM1K;
					}
					else if (rx_buffer[START_REGISTER_H_BYTE] == GRUP_PARAM_REGISTERS) // Write to parameter registers
					{
						state = MODE_WRITE_PARAM;
					}
					else
					{
						BufferClear(rx_buffer);
						state = WAIT_MESSAGE;
					}
				}
				else
				{
					BufferClear(rx_buffer);
					state = WAIT_MESSAGE;
				}	
			}
			else if (kop == WRITE_REGISTER)// Write one register
			{
				data_length = AMOUNT_REGISTERS_BYTE + 1 + rx_buffer[AMOUNT_REGISTERS_BYTE]*2;
				crc_high_byte = rx_buffer[8];
				crc_low_byte 	= rx_buffer[7];
				crc = (crc_high_byte << 8) | crc_low_byte;
				_crc = CRCProcess(rx_buffer, data_length);
				#ifdef TEST_WITHOUT_CRC
				_crc = crc = 1;
				#warning No CRC control!!!!
				#endif
				if (_crc == crc)
				{
					if (rx_buffer[START_REGISTER_H_BYTE] == 0x01) // Write to clock registers
					{
						state = MODE_WRITE_CLOCK;
					}
					else if (rx_buffer[START_REGISTER_H_BYTE] == 0x00) // Write to CM1K registers
					{
						state = MODE_WRITE_CM1K;
					}
					else
					{
						BufferClear(rx_buffer);
						state = WAIT_MESSAGE;
					}
				}
				else
				{
					BufferClear(rx_buffer);
					state = WAIT_MESSAGE;
				}	
			}
			//
			else if (kop == READ_REGISTER)	// Read one register
			{
				data_length = 4;
				state = MODE_READ_CM1K;
			}
			//
			else
			{
				BufferClear(rx_buffer);
				state = WAIT_MESSAGE;
			}
		break;
		case MODE_WRITE_CLOCK://-------------------------------------------------------------------------------------
			// Write data to RTC registers
			res = RTC_SetDateTime(rx_buffer[AMOUNT_REGISTERS_BYTE+2],rx_buffer[AMOUNT_REGISTERS_BYTE+4],
														rx_buffer[AMOUNT_REGISTERS_BYTE+6],rx_buffer[AMOUNT_REGISTERS_BYTE+8],
														rx_buffer[AMOUNT_REGISTERS_BYTE+10],rx_buffer[AMOUNT_REGISTERS_BYTE+12]);
			if (res == HAL_OK)
			{
				LedControl(3);
				RXbufferCopy(19);
				CDC_Transmit_FS(tx_buffer, 19);
			}
			else
			{
				FormErrorMessage(tx_buffer, 0);
				CDC_Transmit_FS(tx_buffer, ERROR_MESSAGE_SIZE);
			}		
			//FlagModbusRecieveData = 0;			
			state = WAIT_MESSAGE;	
		break;
		case MODE_READ_CLOCK:
			// Read data from RTC registers
			BufferClear(tx_buffer);
			res = RTC_GetDateTimeToArray(tx_buffer);
			if (res == HAL_OK)
			{
				LedControl(5);
				tx_buffer[0] = UNIT_ADDRESS;
				tx_buffer[1] = READ_REGISTER;
				tx_buffer[2] = 0x01;
				tx_buffer[3] = number_register;
				tx_buffer[4] = 0x06;
				// Process CRC
				#ifdef TEST_WITHOUT_CRC
				_crc = CRCProcess(tx_buffer, 17);
				tx_buffer[17] = (uint8_t)(_crc);
				tx_buffer[18] = (uint8_t)(_crc >> 8);
				#else
				tx_buffer[17] = 0; tx_buffer[18] = 0;
				#endif
				// Send data
				CDC_Transmit_FS(tx_buffer, 19);
			}
			else
			{
				// Send Error array
				FormErrorMessage(tx_buffer, 0);
				CDC_Transmit_FS(tx_buffer, ERROR_MESSAGE_SIZE);
			}
			state = WAIT_MESSAGE;	
		break;
		case MODE_WRITE_CM1K:
			// Write data to CM1K register
			number_register = rx_buffer[3];
			buffer8[1] = rx_buffer[5];
			buffer8[0] = rx_buffer[6];
			#ifdef CM1K_INTERFACE_SERIAL
			res = HAL_I2C_Mem_Write(&hi2c1, I2C_ADDRESS, number_register, 1, buffer8, 2, 1000);
			#else
			res = HAL_OK;
			#endif
			if (res == HAL_OK)
			{
				LedControl(3);
				RXbufferCopy(9);
				// Send received array
				CDC_Transmit_FS(tx_buffer, 9);
			}
			else
			{
				// Send Error array
				FormErrorMessage(tx_buffer, 0);
				CDC_Transmit_FS(tx_buffer, ERROR_MESSAGE_SIZE);
			}
			state = WAIT_MESSAGE;
		break;
		case MODE_READ_CM1K:
			// Read data from RTC registers
			number_register = rx_buffer[3];
			#ifdef CM1K_INTERFACE_SERIAL
			res = HAL_I2C_Mem_Read(&hi2c1, I2C_ADDRESS, number_register, 1, buffer8, 2, 1000);
			#else
			res = HAL_OK;
			buffer8[1] = 0xAB;
			buffer8[0] = 0xCD;
			#endif
			if (res == HAL_OK)
			{
				LedControl(5);
				tx_buffer[0] = UNIT_ADDRESS;
				tx_buffer[1] = READ_REGISTER;
				tx_buffer[2] = 0x00;
				tx_buffer[3] = number_register;
				tx_buffer[4] = 0x01;
				// Prepare data from register
				tx_buffer[5] = buffer8[1];
				tx_buffer[6] = buffer8[0];
				// Process CRC
				#ifdef TEST_WITHOUT_CRC
				_crc = CRCProcess(tx_buffer, 7);
				tx_buffer[7] = (uint8_t)(_crc);
				tx_buffer[8] = (uint8_t)(_crc >> 8);
				#else
				tx_buffer[7] = 0; tx_buffer[8] = 0;				
				#endif
				// Send data
				CDC_Transmit_FS(tx_buffer, 9);
			}
			else
			{
				// Send Error array
				FormErrorMessage(tx_buffer, 0);
				CDC_Transmit_FS(tx_buffer, ERROR_MESSAGE_SIZE);
			}
			state = WAIT_MESSAGE;
		break;
		case MODE_WRITE_PARAM:
			state = WAIT_MESSAGE;
		break;
		default:
		{
			state = INIT;
		}
	}
}

/*
	* @brief  Buffer clear
  * Author  Andrew A. Ivanov 
	* 29/06/2016
  * @param  ptr - pointer to data buffer
  * @retval None
*/
void BufferClear(uint8_t *ptr)
{
	uint8_t temp = 0;
	uint8_t array_length = 0;
	
	array_length = sizeof(ptr);
	for (temp = 0; temp < array_length; temp++)
	{
		*(ptr + temp) = 0;
	}
}

/*

*/
void RXbufferCopy(uint8_t amount)
{
	uint8_t limit = 0;
	
	if (amount < RX_BUFFER_SIZE)
		limit = amount;
	else
		limit = RX_BUFFER_SIZE;
	
	for (uint8_t temp = 0; temp < limit; temp++)
	{
		tx_buffer[temp] = 0;
		tx_buffer[temp] = rx_buffer[temp];
	}
}

/*
	* @brief  Calculation CRC for Modbus frame
  * Author  Andrew A. Ivanov 
	* 29/06/2016
  * @param  ptr - pointer to data buffer
						length - number of bytes 
  * @retval CRC value
*/
uint16_t CRCProcess(uint8_t *ptr, uint8_t length)
{
	uint16_t crc_value = 0xFFFF;
	uint8_t i, x;
	
	for (i = 0; i < length; i++)
	{
		crc_value ^= (uint16_t)*(ptr + i);
		for (x = 0; x < 8; x++)
		{
			if ((crc_value & 0x0001) > 0)
			{
				crc_value = (crc_value >> 1) ^ (0xA001);
			}
			else
			{
				crc_value = (crc_value >> 1);
			}
		}
	}
	return crc_value;
}

/*
	* @brief  Create error message for Modbus frame
  * Author  Andrew A. Ivanov 
	* 12/09/2016
  * @param  ptr - pointer to data buffer
						kod_err - number of error 
  * @retval None
*/
void FormErrorMessage(uint8_t *ptr, uint8_t kod_err)
{
	uint8_t crc = 0;
	
	*(ptr + ADDRESS_BYTE) = UNIT_ADDRESS;
	*(ptr + KOP_BYTE) = 0x8F;
	*(ptr + KOD_ERROR) = kod_err;
	crc = CRCProcess(tx_buffer, ERROR_MESSAGE_SIZE - 2);
	*(ptr + KOD_ERROR + 1) = (uint8_t)(crc);//low CRC;
	*(ptr + KOD_ERROR + 2) = (uint8_t)(crc >> 8);//high CRC;
}

