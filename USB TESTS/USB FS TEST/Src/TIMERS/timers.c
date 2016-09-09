/*
	User timers control functions
*/

#include "timers.h"

#ifdef __USE_LOCAL_TIMERS
// Local timers array
#if (SIZE_LTIMERS == 32)
uint32_t timers[MAX_TIMERS];
#elif (SIZE_LTIMERS == 16)
uint16_t timers[MAX_TIMERS];
#elif (SIZE_LTIMERS == 8)
uint8_t timers[MAX_TIMERS];
#endif

/*
  ***********************************************************************************
  * @brief  Initialization of local timers
  * @author Andrew A. Ivanov
  * @version V1.01
  * @date 16/10/2011
  * @param  None
  * @retval None
  ***********************************************************************************
*/
void InitSysTimers(void)
{
  for (uint8_t i = 0; i < MAX_TIMERS; i++) timers[i] = 0;
}

/*
  ***********************************************************************************
  * @brief  Get value of local timer
  * @author Andrew A. Ivanov
  * @version V1.01
  * @date 16/10/2011
  * @param  Timer - name (number) of local timer
  * @retval Value of local timer
  ***********************************************************************************
*/
#if (SIZE_LTIMERS == 32)
uint32_t GetSysTimer(uint8_t Timer)
#elif (SIZE_LTIMERS == 16)
uint16_t GetSysTimer(uint8_t Timer)
#elif (SIZE_LTIMERS == 8)
uint8_t GetSysTimer(uint8_t Timer)
#endif
{
  return timers[Timer];
}

/*
  ***********************************************************************************
  * @brief  Reset local timer
  * @author Andrew A. Ivanov
  * @version V1.01
  * @date 16/10/2011
  * @param  Timer - name (number) of local timer
  * @retval None
  ***********************************************************************************
*/
void ResetSysTimer(uint8_t Timer)
{
  timers[Timer] = 0;
}

/*
  ***********************************************************************************
  * @brief  Increment values of local timers
  * @author Andrew A. Ivanov
  * @version V1.01
  * @date 13/05/2016
  * @param  None
  * @retval None
  ***********************************************************************************
*/
void IncSysTimer(void)
{
	for (uint8_t i = 0; i < MAX_TIMERS; i++)
	{
		timers[i]++;
	}
}
#endif

#ifdef __USE_GLOBAL_TIMERS
// Global timers array
uint32_t GTimers[MAX_GTIMERS];
// Global timers states array
uint8_t  GTStates[MAX_GTIMERS];

/*
  ***********************************************************************************
  * @brief  Initialization of global timers
  * @author Andrew A. Ivanov
  * @version V1.01
  * @date 16/10/2011
  * @param  None
  * @retval None
  ***********************************************************************************
*/
void InitGTimers(void)
{
  for (uint8_t i = 0; i < MAX_GTIMERS; i++) GTStates[i] = TIMER_STOPPED;
}

/*
  ***********************************************************************************
  * @brief  Start global timer
  * @author Andrew A. Ivanov
  * @version V1.01
  * @date 16/10/2011
  * @param  GTimerID - name (number) of global timer
  * @retval None
  ***********************************************************************************
*/
void StartGTimer(uint8_t GTimerID)
{
  if (GTStates[GTimerID] == TIMER_STOPPED)
	{
	  GTimers[GTimerID] = 0;
		GTStates[GTimerID] = TIMER_RUNNING;
	}
}

/*
  ***********************************************************************************
  * @brief  Stop global timer
  * @author Andrew A. Ivanov
  * @version V1.01
  * @date 16/10/2011
  * @param  GTimerID - name (number) of global timer
  * @retval None
  ***********************************************************************************
*/
void StopGTimer(uint8_t GTimerID)
{
	GTStates[GTimerID] = TIMER_STOPPED;
}

/*
  ***********************************************************************************
  * @brief  Pause global timer
  * @author Andrew A. Ivanov
  * @version V1.01
  * @date 16/10/2011
  * @param  GTimerID - name (number) of global timer
  * @retval None
  ***********************************************************************************
*/
void PauseGTimer(uint8_t GTimerID)
{
  if (GTStates[GTimerID] == TIMER_RUNNING)
	{
	  GTStates[GTimerID] = TIMER_PAUSED;
	}
}

/*
  ***********************************************************************************
  * @brief  Release global timer
  * @author Andrew A. Ivanov
  * @version V1.01
  * @date 16/10/2011
  * @param  GTimerID - name (number) of global timer
  * @retval None
  ***********************************************************************************
*/
void ReleaseGTimer(uint8_t GTimerID)
{
  if (GTStates[GTimerID] == TIMER_PAUSED)
	{
	  	GTStates[GTimerID] = TIMER_RUNNING;
	}
}

/*
  ***********************************************************************************
  * @brief  Get value of global timer
  * @author Andrew A. Ivanov
  * @version V1.01
  * @date 16/10/2011
  * @param  GTimerID - name (number) of global timer
  * @retval Value of global timer
  ***********************************************************************************
*/
uint32_t GetGTimer(uint8_t GTimerID)
{
  	return GTimers[GTimerID];
}

/*
  ***********************************************************************************
  * @brief  Reset global timer
  * @author Andrew A. Ivanov
  * @version V1.01
  * @date 16/10/2011
  * @param  GTimerID - name (number) of global timer
  * @retval None
  ***********************************************************************************
*/
void ResetGTimer(uint8_t GTimerID)
{
  	GTimers[GTimerID] = 0;
}

/*
  ***********************************************************************************
  * @brief  Get state of global timer
  * @author Andrew A. Ivanov
  * @version V1.00
  * @date 05/12/2013
  * @param  GTimerID - name (number) of global timer
  * @retval State of global timer - TIMER_STOPPED,TIMER_RUNNING,TIMER_PAUSED
  ***********************************************************************************
*/
uint8_t	GetStateGTimer(uint8_t GTimerID)
{
	return GTStates[GTimerID];
}

/*
  ***********************************************************************************
  * @brief  Increment values of global timers
  * @author Andrew A. Ivanov
  * @version V1.01
  * @date 13/05/2016
  * @param  None
  * @retval None
  ***********************************************************************************
*/
void IncGTimer(void)
{
	for (uint8_t i = 0; i < MAX_GTIMERS; i++)
	{
		GTimers[i]++;
	}
}
#endif

