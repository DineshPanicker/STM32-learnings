#include "main.h"
#include "bsp.h"
#include <stdbool.h>

#define LOOP_DURATION (1000 - 1) //first number is milliseconds

uint32_t Timeslice = LOOP_DURATION;

void UserInit(void)
{
	BoardInit(true);
}


void UserLoop(void)
{
	while(Timeslice > 0);
	LEDToggle(LED_GREEN_RIGHT);
	Timeslice = LOOP_DURATION;
}


//this function gets called every 1ms by the HAL
void HAL_SYSTICK_Callback(void)
{
	Timeslice--;
}
