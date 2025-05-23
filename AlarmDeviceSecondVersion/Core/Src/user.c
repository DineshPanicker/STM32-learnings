#include <stdbool.h>
#include "stm32f4xx.h"
#include "user.h"
#include "bsp.h"

//redefine keys from bsp.h
#define KEY_ALARM KEY_RED
#define KEY_RESET KEY_GREEN

//functions private to user.c, hence do not add to user.h
static void ProcessKeys(bool *AlarmTripped);
static void ProcessBuzzer(bool AlarmTripped);
static void Beep(uint32_t Periods);
static void DelayMs(uint32_t Milliseconds);

void UserInit(void) //this gets called only once
{
	BoardInit(true); //initialize buzzer pin also
}


void UserLoop(void) //this gets called repeatedly
{
	static bool AlarmTripped = false;
	ProcessKeys(&AlarmTripped);
	ProcessBuzzer(AlarmTripped);
}


void ProcessKeys(bool *AlarmTripped)
{
	if((KeyRead(KEY_ALARM) == true) && (*AlarmTripped == false))
	{
		//Alarm has been tripped (and was not tripped before)
		*AlarmTripped = true;
	}
	if((KeyRead(KEY_RESET) == true) && (*AlarmTripped == true))
	{
		//Alarm should be reset (and was not reset before)
		*AlarmTripped = false;
	}

}

void ProcessBuzzer(bool AlarmTripped)
{
	static bool BeepPause = true;
	if(AlarmTripped == true) //alarm is active
	{
		if(BeepPause == false) //buzzer shall be sounded
			{
			Beep(100); //this takes 200ms
			BeepPause = true;
			}
		else //buzzer shall not be sounded (pause)
		{
			HAL_Delay(200);
			BeepPause = false;
		}
	}
	else //alarm is not active
	{
		//no action required, just wait 200ms
		HAL_Delay(200);
	}
}

//Beep for number Periods times. One beep is comprised of turning the buzzer on for 1ms and off for 1ms
void Beep(uint32_t Periods)
{
	uint32_t LoopIndex;
	for(LoopIndex=0; LoopIndex <= Periods; LoopIndex++)
	{
		BuzzerOn();
		DelayMs(1);
		BuzzerOff();
		DelayMs(1);
	}
}


//We still need this function since HAL_Delay() is imprecise with small delays
void DelayMs(uint32_t Milliseconds)
//Simple delay loop which takes clock frequency into consideration
{
	uint32_t LoopIndex;
	uint32_t Cycles = (Milliseconds * SystemCoreClock) / (13 * 1000);
//	DWT->CYCCNT = 0; //reset cycle counter
	for(LoopIndex=0; LoopIndex <= Cycles; LoopIndex++); //one empty loop cycle is approx. 13 cycles
//	temp = DWT->CYCCNT - 1; //How many cycles were taken by the for loop?
}
