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
uint8_t buffer8[] = {0};
uint16_t buffer16[] = {0};
HAL_StatusTypeDef result  = HAL_OK;
extern I2C_HandleTypeDef hi2c1;
extern RTC_HandleTypeDef hrtc;
extern RTC_TimeTypeDef cTime;
extern RTC_DateTypeDef cDate;

#define TEST_WITHOUT_CRC  // Comment for normal work!

enum ModbusStates modbus_states;
enum ModbusErrors modbus_errors;

//static uint8_t state = INIT, _state = (INIT + 1), flag = 0;
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
	static uint8_t state = INIT, _state = (INIT + 1), flag = 0;
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
		case WAIT_MESSAGE:
			if (flag) 
			{
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
			else if (kop == WRITE_REGISTER)// Write one register
			{
				data_length = 4;
				crc_high_byte = rx_buffer[5];
				crc_low_byte = rx_buffer[4];
				crc = (crc_high_byte << 8) | crc_low_byte;
				_crc = CRCProcess(rx_buffer, data_length);
				#ifdef TEST_WITHOUT_CRC
				_crc = crc = 1;
				#warning No CRC control!!!!
				#endif
				if (_crc == crc)
				{
					number_register = rx_buffer[2];
					//data_register = rx_buffer[3];
					state = MODE_WRITE_CLOCK;
				}
				else
				{
					BufferClear(rx_buffer);
					state = WAIT_MESSAGE;
				}	
			}
			//
			else if (kop == READ_REGISTER)	// Read one register
				data_length = 4;
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
				RXbufferCopy();
				CDC_Transmit_FS(tx_buffer, sizeof(tx_buffer));
			}
			else
			{
				tx_buffer[ADDRESS_BYTE] = UNIT_ADDRESS;
				tx_buffer[KOP_BYTE] = 0x8F;
				tx_buffer[KOD_ERROR] = ModbusErrorRegister;
				crc = CRCProcess(tx_buffer, ERROR_MESSAGE_SIZE - 2);
				tx_buffer[KOD_ERROR + 1] = (uint8_t)(crc);//low CRC;
				tx_buffer[KOD_ERROR + 2] = (uint8_t)(crc >> 8);//high CRC;
				CDC_Transmit_FS(tx_buffer, ERROR_MESSAGE_SIZE);
			}				
			state = WAIT_MESSAGE;	
		break;
		case MODE_READ_CLOCK:
			// Read data from RTC registers
			//res = HAL_I2C_Mem_Read(&hi2c1, I2C_ADDRESS, number_register, 1, buffer8, 1, 1000);
			BufferClear(tx_buffer);
			res = RTC_GetDateTimeToArray(tx_buffer);
			if (res == 0)
			{
				tx_buffer[0] = UNIT_ADDRESS;
				tx_buffer[1] = READ_REGISTER;
				tx_buffer[2] = number_register;
				tx_buffer[3] = buffer8[0];
				tx_buffer[4] = 0x00;
				tx_buffer[5] = 0x00;
				//CRC16();
				CDC_Transmit_FS(tx_buffer, 6);
			}
			else
			{
				
			}
			state = WAIT_MESSAGE;	
		break;
		case MODE_WRITE_CM1K:
			
		break;
		case MODE_READ_CM1K:
			
		break;
		case MODE_LEARN:
			CogniLearn();
			CDC_Transmit_FS(Msg1,strlen(Msg1));
			state = WAIT_MESSAGE;			
		break;
		case MODE_RECOGNIZE:
			CogniRecognize();
			CDC_Transmit_FS(Msg2,strlen(Msg2));
			state = WAIT_MESSAGE;			
		break;
		case MODE_SAVE:
			CogniSave();
			CDC_Transmit_FS(Msg3,strlen(Msg3));
			state = WAIT_MESSAGE;				
		break;
		case MODE_RESTORE:
			CogniRestore();
			CDC_Transmit_FS(Msg4,strlen(Msg4));
			state = WAIT_MESSAGE;			
		break;
		case MODE_TEST:
			CogniTest();
			CDC_Transmit_FS(Msg5,strlen(Msg5));
			state = WAIT_MESSAGE;
		break;
		/*
		case MODE_WRITE_TIME:
			buffer_clock[0] = 0;
			buffer_clock[1] = 0;
			buffer_clock[2] = 0;
			uint8_t result = HAL_I2C_Mem_Write(&hi2c1, I2C_ADDRESS, 0x00, 1, buffer_clock, 3, 1000);
			if (result == 0)
			{
				if (kop == WRITE_SINGLE_REGISTER)
				{
					
					CDC_Transmit_FS(tx_buffer, WRITE_SINGLE_REGISTER_BYTES);
				}
				else if (kop == WRITE_MULTIPLE_REGISTERS)
				{
					
					CDC_Transmit_FS(tx_buffer, WRITE_SINGLE_REGISTER_BYTES);
				}
			}
			else
			{
				
			}
		break;
		*/	
		case SEND_MESSAGE:
			RXbufferCopy();
			CDC_Transmit_FS(tx_buffer, sizeof(tx_buffer));
			//----------------------------------------------------
			state = WAIT_MESSAGE;
		break;
		case SEND_ERROR:
			tx_buffer[ADDRESS_BYTE] = UNIT_ADDRESS;
			tx_buffer[KOP_BYTE] = 0x8F;
			tx_buffer[KOD_ERROR] = ModbusErrorRegister;
			crc = CRCProcess(tx_buffer, ERROR_MESSAGE_SIZE - 2);
			tx_buffer[KOD_ERROR + 1] = (uint8_t)(crc);//low CRC;
			tx_buffer[KOD_ERROR + 2] = (uint8_t)(crc >> 8);//high CRC;
			CDC_Transmit_FS(tx_buffer, ERROR_MESSAGE_SIZE);
			//----------------------------------------------------
			state = WAIT_MESSAGE;
		break;
		case WAIT_RESET:
			
		break;
		default:
		{
			
		}
	}
}

/*

*/
void ModbusPack(void)
{

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
void RXbufferCopy(void)
{
	for (uint8_t temp = 0; temp < RX_BUFFER_SIZE; temp++)
	{
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
