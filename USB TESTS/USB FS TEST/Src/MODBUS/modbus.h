/*
File "modbus.h"

*/

#ifndef __MODBUS_H
#define __MODBUS_H

#include "stm32f4xx_hal.h"

#define RX_BUFFER_SIZE (64)
#define TX_BUFFER_SIZE (64)
#define ERROR_MESSAGE_SIZE (5)
#define UNIT_ADDRESS (0x01)

// Codes functions
#define READ_REGISTER 	(0x03)
#define WRITE_REGISTER 	(0x06)
#define WRITE_REGISTERS (0x10)

#define ADDRESS_BYTE (0)
#define KOP_BYTE (1)
#define START_REGISTER_H_BYTE (2)
#define START_REGISTER_L_BYTE (3)
#define AMOUNT_REGISTERS_BYTE (4)
#define DATA_H_BYTE (4)
#define DATA_L_BYTE (5)
#define CRC_H_BYTE (7)
#define CRC_L_BYTE (6)
#define KOD_ERROR (2)

#define WRITE_SINGLE_REGISTER_BYTES	(8)

#define CONTROL_REGISTER_ADDRESS 		(0x0100)
#define STATE_REGISTER_ADDRESS 			(0x0200)

//
#define MASK_CM1K_REGISTERS		(0x0100)
#define MASK_CLOCK_REGISTERS	(0x0200)

// Clock registers
#define SEC_REGISTER_ADDRESS 				(0x0200)
#define MINUTES_REGISTER_ADDRESS 		(0x0201)
#define HOURS_REGISTER_ADDRESS 			(0x0202)

// Command's codes
#define COMMAND_RESET				(0x000F)
#define COMMAND_LEARN				(0x0001)
#define COMMAND_RECOGNIZE		(0x0002)
#define COMMAND_SAVE				(0x0004)
#define COMMAND_RESTORE			(0x0008)
#define COMMAND_TEST				(0x0010)
#define COMMAND_WRITE_TIME	(0x0020)
#define COMMAND_READ_TIME		(0x0040)

enum ModbusStates {INIT = 0,WAIT_MESSAGE,ADDRESS_CONTROL,CRC_CONTROL,UNPACK_MESSAGE,
									SEND_MESSAGE,SEND_ERROR,WAIT_RESET, MODE_LEARN, MODE_RECOGNIZE, MODE_SAVE, 
									MODE_RESTORE, MODE_TEST, MODE_WRITE_CLOCK, MODE_READ_CLOCK, MODE_WRITE_CM1K, MODE_READ_CM1K};
enum ModbusErrors {NO_ERROR = 0, ILLEGAL_FUNCTION, ILLEGAL_DATA_ADDRESS, ILLEGAL_DATA_VALUE};

extern uint8_t rx_buffer[RX_BUFFER_SIZE];
extern uint8_t tx_buffer[TX_BUFFER_SIZE];
extern uint8_t FlagModbusRecieveData;

/* Private function prototypes -----------------------------------------------*/
void ModbusProcess(void);
void ModbusUnpack(void);
void ModbusPack(void);
void BufferClear(uint8_t *ptr);
//void TXbufferClear(void);
uint16_t CRCProcess(uint8_t *ptr, uint8_t length);
void RXbufferCopy(void);

#endif
