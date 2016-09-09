
#include "CM1K.h"
#include "LCD.h"
//#include "stm32f4xx_hal.h"

extern I2C_HandleTypeDef hi2c1;

static uint8_t short_buffer[4];

struct 
{
	uint8_t context;
	uint8_t components[NEURON_MAX_COMPONENTS];
	uint16_t aif;
	uint16_t cat;
} Neuron_buffer;

//struct NeuronArray input_buf;

/*
	* @brief  Write CM1K register
  * Author  Andrew A. Ivanov 
	* 29/06/2016
  * @param  number - register address
						value - 
	* @retval rezult: 1 - ok, 0 - bad
*/
uint8_t WriteCogniRegister(uint8_t number, uint16_t value)
{
	
	return 1;
}

/*
	* @brief  Read CM1K register
  * Author  Andrew A. Ivanov 
	* 29/06/2016
  * @param  number - register address
	* @retval rezult: 1 - ok, 0 - bad
*/
uint16_t ReadCogniRegister(uint16_t number)
{
	return 1;
}

uint8_t CogniLearn(void)
{
	//HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12,GPIO_PIN_SET); 
	LCD_DisplayStringLine(100,100,"LEARN MODE..", LCD_COLOR_WHITE, LCD_COLOR_MAGENTA);
	HAL_Delay(2000);//
	//HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12,GPIO_PIN_RESET);
	return 1;
}

uint8_t CogniRecognize(void)
{
	//HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13,GPIO_PIN_SET);
	LCD_DisplayStringLine(100,100,"RECOGNIZE MODE..", LCD_COLOR_WHITE, LCD_COLOR_MAGENTA);	
	HAL_Delay(2000);//
	//HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13,GPIO_PIN_RESET);
	return 1;
}

uint8_t CogniSave(void)
{
	//HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14,GPIO_PIN_SET); 
	LCD_DisplayStringLine(100,100,"SAVE MODE..", LCD_COLOR_WHITE, LCD_COLOR_MAGENTA);
	HAL_Delay(2000);//
	//HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14,GPIO_PIN_RESET);
	return 1;
}

uint8_t CogniRestore(void)
{
	//HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15,GPIO_PIN_SET); 
	LCD_DisplayStringLine(100,100,"RESTORE MODE..", LCD_COLOR_WHITE, LCD_COLOR_MAGENTA);
	HAL_Delay(2000);//
	//HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15,GPIO_PIN_RESET);
	return 1;
}

uint8_t CogniTest(void)
{
	LCD_DisplayStringLine(100,100,"TEST MODE..", LCD_COLOR_WHITE, LCD_COLOR_MAGENTA);
	HAL_Delay(2000);//
	return 1;
}

/*
	
	@retval HAL status
*/
uint8_t CM1K_LoadContentToNeuron(uint8_t *buffer, uint8_t components, uint8_t context, uint8_t category, uint16_t aif)
{
	uint8_t result = 0;
	uint16_t lastNSR = 0;
	uint16_t lastGCR = 0;
	uint8_t i = 0;
	
	// Read NSR
	result = HAL_I2C_Mem_Read(&hi2c1, CM1K_ADDRESS, CM_NSR, 1, short_buffer, 2, 1000);
	if (result != HAL_OK)
		return 0;
	lastNSR = ((short_buffer[1] << 8)| short_buffer[0]);
	// Read GCR
	result = HAL_I2C_Mem_Read(&hi2c1, CM1K_ADDRESS, CM_GCR, 1, short_buffer, 2, 1000);
	if (result != HAL_OK)
		return 0;
	lastGCR = ((short_buffer[1] << 8)| short_buffer[0]);
	
	short_buffer[0] = 0x00; short_buffer[1] = 0x00;
	result = HAL_I2C_Mem_Write(&hi2c1, CM1K_ADDRESS, CM_FORGET, 1, short_buffer, 2, 1000);
	if (result != HAL_OK)
		return 0;
	short_buffer[0] = 0x00; short_buffer[1] = 0x10;
	result = HAL_I2C_Mem_Write(&hi2c1, CM1K_ADDRESS, CM_NSR, 1, short_buffer, 2, 1000);
	if (result != HAL_OK)
		return 0;
	short_buffer[0] = 0x00; short_buffer[1] = 0x00;
	result = HAL_I2C_Mem_Write(&hi2c1, CM1K_ADDRESS, CM_RESETCHAIN, 1, short_buffer, 2, 1000);
	if (result != HAL_OK)
		return 0;
	// Write component's values to neuron
	for (i = 0; i < components; i++)
	{
		result = HAL_I2C_Mem_Write(&hi2c1, CM1K_ADDRESS, CM_COMP, 1, buffer + i, 2, 1000);
		if (result != HAL_OK)
			return 0;
	}
	// Write context - NCR register
	short_buffer[0] = context & 0x7F; short_buffer[1] = 0x00;
	result = HAL_I2C_Mem_Write(&hi2c1, CM1K_ADDRESS, CM_NCR, 1, short_buffer, 1, 1000);
	if (result != HAL_OK)
		return 0;
	// Write AIF
	short_buffer[0] = (context >> 8); short_buffer[1] = context;
	result = HAL_I2C_Mem_Write(&hi2c1, CM1K_ADDRESS, CM_AIF, 1, short_buffer, 2, 1000);
	if (result != HAL_OK)
		return 0;
	// Write CAT
	short_buffer[0] = (category >> 8); short_buffer[1] = category;
	result = HAL_I2C_Mem_Write(&hi2c1, CM1K_ADDRESS, CM_CAT, 1, short_buffer, 2, 1000);
	if (result != HAL_OK)
		return 0;
	// Write NSR
	short_buffer[0] = (lastNSR >> 8); short_buffer[1] = lastNSR;
	result = HAL_I2C_Mem_Write(&hi2c1, CM1K_ADDRESS, CM_NSR, 1, short_buffer, 2, 1000);
	if (result != HAL_OK)
		return 0;
	// Write GCR
	short_buffer[0] = (lastGCR >> 8); short_buffer[1] = lastGCR;
	result = HAL_I2C_Mem_Write(&hi2c1, CM1K_ADDRESS, CM_GCR, 1, short_buffer, 2, 1000);
	if (result != HAL_OK)
		return 0;
		
	return 1;
}

/*
	@brief Reading the contents of the neurons
	@retval HAL status
*/
uint8_t CM1K_ReadAllOfNeurons(uint8_t *buffer, uint8_t components, uint8_t context, uint8_t category, uint16_t aif)
{
	return 1;
}

/*
	@brief Reading the content of a single specific neuron
	@retval HAL status
*/
uint8_t CM1K_ReadSpecificNeuron(uint8_t *buffer, uint8_t index, uint8_t components)
{
	uint8_t result = 0;
	
	// Write NSR = 16 (0x0010)
	short_buffer[0] = 0x00; short_buffer[1] = 0x10;
	result = HAL_I2C_Mem_Write(&hi2c1, CM1K_ADDRESS, CM_NSR, 1, short_buffer, 2, 1000);
	if (result != HAL_OK)
		return 0;
	// Write RESETCAIN = 0
	short_buffer[0] = 0x00; short_buffer[1] = 0x00;
	result = HAL_I2C_Mem_Write(&hi2c1, CM1K_ADDRESS, CM_RESETCHAIN, 1, short_buffer, 2, 1000);
	if (result != HAL_OK)
		return 0;
	// Read neuron's 
	for (uint8_t x = 0; x < index - 1; x++)
	{
		result = HAL_I2C_Mem_Read(&hi2c1, CM1K_ADDRESS, CM_CAT, 1, short_buffer, 2, 1000);
	}
	// Read context of index neuron
	result = HAL_I2C_Mem_Read(&hi2c1, CM1K_ADDRESS, CM_NCR, 1, short_buffer, 2, 1000);
	Neuron_buffer.context = short_buffer[1] & 0x7F;
	// Read components of neuron
	for (uint8_t x = 0; x < components; x++)
	{
		result = HAL_I2C_Mem_Read(&hi2c1, CM1K_ADDRESS, CM_COMP, 1, Neuron_buffer.components + x, 1, 1000);
	}
	// Read AIF
	result = HAL_I2C_Mem_Read(&hi2c1, CM1K_ADDRESS, CM_AIF, 1, short_buffer, 2, 1000);
	Neuron_buffer.aif = (short_buffer[0] << 8)|short_buffer[1];
	// Read CAT
	result = HAL_I2C_Mem_Read(&hi2c1, CM1K_ADDRESS, CM_CAT, 1, short_buffer, 2, 1000);
	Neuron_buffer.cat = (short_buffer[0] << 8) | short_buffer[1];
	// Write NSR = 0
	short_buffer[0] = 0x00; short_buffer[1] = 0x00;
	result = HAL_I2C_Mem_Write(&hi2c1, CM1K_ADDRESS, CM_NSR, 1, short_buffer, 2, 1000);
	if (result != HAL_OK)
		return 0;
	
	return 1;
}

/*
	@brief Reading the number of committed neurons
	@retval Number of committed neurons
*/
uint16_t ReadNumberCommittedNeurons(void)
{
	uint16_t CommittedNeurons = 0, Cat = 0;
	uint8_t result = 0;
	
	// Write NSR = 16 (0x0010)
	short_buffer[0] = 0x00; short_buffer[1] = 0x10;
	result = HAL_I2C_Mem_Write(&hi2c1, CM1K_ADDRESS, CM_NSR, 1, short_buffer, 2, 1000);
	if (result != HAL_OK)
		return 0;
	// Write RESETCAIN = 0
	short_buffer[0] = 0x00; short_buffer[1] = 0x00;
	result = HAL_I2C_Mem_Write(&hi2c1, CM1K_ADDRESS, CM_RESETCHAIN, 1, short_buffer, 2, 1000);
	if (result != HAL_OK)
		return 0;
	// Read CM_CAT registrs
	do
	{
		result = HAL_I2C_Mem_Read(&hi2c1, CM1K_ADDRESS, CM_CAT, 1, short_buffer, 2, 1000);
		Cat = (short_buffer[0] << 8) | short_buffer[1];
		if (Cat != 0) CommittedNeurons++;
	}
	while (Cat != 0);
	// Write NSR = 0
	short_buffer[0] = 0x00; short_buffer[1] = 0x00;
	result = HAL_I2C_Mem_Write(&hi2c1, CM1K_ADDRESS, CM_NSR, 1, short_buffer, 2, 1000);
	if (result != HAL_OK)
		return 0;
	
	return CommittedNeurons;
}




