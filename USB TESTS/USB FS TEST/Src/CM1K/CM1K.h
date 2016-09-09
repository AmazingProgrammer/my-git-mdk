/*
	File "CM1K.h"
*/

#ifndef __COGNIMEM_H
#define __COGNIMEM_H

#include "stm32f4xx_hal.h"

#define CM1K_ADDRESS 			(0x4A)	//CM1K I2C chip's address
#define NEURON_MAX_COMPONENTS (256)
// CM1K neuron registers addresses
#define CM_NCR 				(0x00)	// Neuron Context Register
#define CM_COMP 			(0x01)	// Component
#define CM_LCOMP 			(0x02)	// Last Component
#define CM_INDEXCOMP 	(0x03)	// Component index
#define CM_DIST 			(0x03)	// Distance register
#define CM_CAT 				(0x04)	// Category register
#define CM_AIF 				(0x05)	// Active Influence Field
#define CM_MINIF 			(0x06)	// Minimum Influence Field
#define CM_MAXIF 			(0x07)	// Maximum Influence Field
#define CM_TESTCOMP 	(0x08) 	// Test Component 
#define CM_TESTCAT 		(0x09)	// Test Category

#define CM_NID 				(0x0A)	// Neuron Identifier
#define CM_GCR 				(0x0B)	// Global Control Register
#define CM_RESETCHAIN (0x0C)	// Points to the first neuron of the chain
#define CM_NSR 				(0x0D)	// Network Status Register
#define CM_POWERSAVE 	(0x0E)	// Dummy register
#define CM_FORGET 		(0x0F)	// Clear the neuron’s category register
#define CM_NCOUNT 		(0x0F)	// NR - Number of committed neurons //SR - Index of the neuron pointed in the chain

#define CM_TOP 				(0x11)	// Top corner of the ROI in pixels
#define CM_LEFT 			(0x12)	// Left corner of the ROI in pixels
#define CM_WIDTH 			(0x13)	// Nominal width of the ROI in pixels
#define CM_HEIGH 			(0x14)	// Nominal height of the ROI in pixels
#define CM_BWIDTH 		(0x15)	// Width of a primitive block in pixels
#define CM_BHEIGH 		(0x16)	// Height of a primitive block in pixels
#define CM_RSR 				(0x1C)	// Recognition Status Register	
#define CM_RTDIST 		(0x1D)	// Real-time distance, or distance of top firing neuron
#define CM_RTCAT 			(0x1E)	// Real-time category, or category of top firing neuron
#define CM_ROINIT 		(0x1F)	// Reset the ROI to default											

uint8_t WriteCogniRegister(uint8_t number, uint16_t value);
uint16_t ReadCogniRegister(uint16_t number);
uint8_t CogniLearn(void);
uint8_t CogniRecognize(void);
uint8_t CogniSave(void);
uint8_t CogniRestore(void);
uint8_t CogniTest(void);
uint8_t CM1K_LoadContentToNeuron(uint8_t *buffer, uint8_t components, uint8_t context, uint8_t category, uint16_t aif);
uint8_t CM1K_ReadAllOfNeurons(uint8_t *buffer, uint8_t components, uint8_t context, uint8_t category, uint16_t aif);
uint8_t CM1K_ReadSpecificNeuron(uint8_t *buffer, uint8_t index, uint8_t components);
uint16_t ReadNumberCommittedNeurons(void);

#endif
