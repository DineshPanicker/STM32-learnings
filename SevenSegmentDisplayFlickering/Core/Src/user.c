#include <stdbool.h>
#include "stm32f4xx.h"
#include "user.h"
#include "bsp.h"
#include "scheduler.h"


//value to show on display
uint32_t DisplayValue = 1234;
uint16_t DisplayTimeMultiplier = 1;
bool DigitOrderShuffled = true;

//Counter for milliseconds elapsed since start
uint32_t CounterMs = 0;

struct {
	uint32_t TimeSliceElapsed:1;
	uint32_t DoReadKeys:1;
} ProgramFlags;


//These are our tasks
void ReadKeys(void)
{
	if (KeyRead(KEY_BLUE) == true) DisplayTimeMultiplier = 1;
	if (KeyRead(KEY_RED) == true) DisplayTimeMultiplier = 3;
	if (KeyRead(KEY_GREEN) == true) DisplayTimeMultiplier = 5;
	if (KeyRead(KEY_WHITE) == true) DisplayTimeMultiplier = 1000;
//	if (KeyRead(KEY_JOYSTICK) == true)
//		{
//		DigitOrderShuffled = !DigitOrderShuffled;
//		}
}


void PrintDisplay(void)
{
	static uint8_t DisplayCounter = 1;

	if((DisplayCounter % DisplayTimeMultiplier) == 0)
	{
		if(DigitOrderShuffled == true) UpdateDisplayArbitrarySequence((uint16_t)DisplayValue);
		else UpdateDisplay((uint16_t)DisplayValue);
		DisplayCounter = 1;
	}
	else DisplayCounter++;
}
//end of tasks

void UserInit(void) //this gets called only once
{
	//Enable cycle counter
	DWT->CTRL |= 1;
	BoardInit(false);

    //Init the scheduler and add tasks as required
	sInit(); //scheduler init
	sAddTask(PrintDisplay, 0, 1);
	sAddTask(ReadKeys, 1, 50);
}


void UserLoop(void) //this gets called repeatedly
{
	if(ProgramFlags.TimeSliceElapsed == true)
	{
		ProgramFlags.TimeSliceElapsed = false;
		sDispatchTasks(); //dispatch tasks
	}

}

//this function gets called every 1ms by the HAL
void HAL_SYSTICK_Callback(void)
{
//	UpdateDisplay((uint16_t)DisplayValue); //if TimeSliceMultiplier = 10
	CounterMs++;
	sUpdate(); //update scheduler
	ProgramFlags.TimeSliceElapsed = true;
}
