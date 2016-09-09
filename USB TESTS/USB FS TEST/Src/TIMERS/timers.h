/*
		@file "timers.h"
		@author Andrew A/ Ivanov
*/

/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __TIMERS_H
#define __TIMERS_H

// Uncomment the required grup of timers!!!!!
#define __USE_LOCAL_TIMERS
//#define __USE_GLOBAL_TIMERS

#include "stm32f4xx_hal.h"

#ifdef __USE_LOCAL_TIMERS
//extern uint32_t timers[];
#define SIZE_LTIMERS  (16)//(32)//(8)
#define MAX_TIMERS   (2)
// Local timers list
#define LTIMER_SYS         (0)		
#define LTIMER_X           (1)	
// Local Timers Control functions
void InitSysTimers(void);
void ResetSysTimer(uint8_t Timer);
void IncSysTimer(void);
#if (SIZE_LTIMERS == 32)
uint32_t GetSysTimer(uint8_t Timer);
#elif (SIZE_LTIMERS == 16)
uint16_t GetSysTimer(uint8_t Timer);
#elif (SIZE_LTIMERS == 8)
uint8_t GetSysTimer(uint8_t Timer);
#endif

#endif	/* __USE_LOCAL_TIMERS */

#ifdef __USE_GLOBAL_TIMERS
//extern uint32_t GTimers[];
//extern uint8_t  GTStates[];
#define SIZE_GTIMERS  (16)//(32)//(8)
#define MAX_GTIMERS  (2)
// Global timers list
#define GTIMER_MOVE  						(0)		
#define GTIMER_MOVE_PUSK				(1)		
//GTimer states
#define TIMER_STOPPED 	(0)
#define TIMER_RUNNING 	(1)
#define TIMER_PAUSED  	(2)
// GTimers Control functions
void InitGTimers(void);
void StartGTimer(uint8_t GTimerID);		
void StopGTimer(uint8_t GTimerID);			
void PauseGTimer(uint8_t GTimerID);		
void ReleaseGTimer(uint8_t GTimerID);	
uint32_t  GetGTimer(uint8_t GTimerID);			
void ResetGTimer(uint8_t GTimerID);		
uint8_t   GetStateGTimer(uint8_t GTimerID);	
void IncGTimer(void);

#endif	/* __USE_GLOBAL_TIMERS */


#endif	/* __TIMERS_H */
