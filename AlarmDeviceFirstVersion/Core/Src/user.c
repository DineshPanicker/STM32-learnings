//file user.c
#include <stdbool.h>
#include "stm32f4xx.h"
#include "user.h"

//internal functions, hence do not add to user.h
static void DelayMs(uint32_t Milliseconds);
static void Beep(uint32_t Periods);

void UserInit(void) //this gets called only once
{
	//Enable clock for GPIOA, GPIOB and GPIOD
	RCC->AHB1ENR |= 1;
	RCC->AHB1ENR |= (1 << 1);
	RCC->AHB1ENR |= (1 << 3);

	//Configure PD2 and PB15 as inputs with pull ups enabled
	//These are the buttons red and green buttons respectively
	GPIOD->MODER &= ~(1 << 4); //clear bit 4
	GPIOD->MODER &= ~(1 << 5); //clear bit 5
	GPIOB->MODER &= ~(1 << 30); //clear bit 30
	GPIOB->MODER &= ~(1 << 31); //clear bit 31
//	GPIOC->PUPDR |= (1); //set bit 0
//	GPIOC->PUPDR &= ~(1 << 1); //clear bit 1
//	GPIOC->PUPDR |= (1 << 12); //set bit 12
//	GPIOC->PUPDR &= ~(1 << 13); //clear bit 13

	//Configure PA11 as push pull output
	//The buzzer is connected to that pin
	GPIOA->MODER |= (1 << 22); //set bit 22
	GPIOA->MODER &= ~(1 << 23); //clear bit 23
	GPIOA->OTYPER &= ~(1 << 11); //clear bit 11
	GPIOA->ODR &= ~(1 << 11); //clear bit 11

	//Enable and reset the CPU cycle counter
	//for evaluation of delay loop
//	DWT->CTRL |= 1; //enable counter
//	DWT->CYCCNT = 0; //reset counter to zero
//	SystemCoreClockUpdate(); //update the value of SystemCoreClock variable
	//only required if clock frequency has been changed
}

void UserLoop(void) //this gets called repeatedly
{
	static bool AlarmTripped = false;
	static bool BeepPause = true;
	if(((GPIOD->IDR & (1 << 2))) == false  && (AlarmTripped == false))
	{
		//Alarm has been tripped
		AlarmTripped = true;
	}
	if(((GPIOB->IDR & (1 << 15)) == false) && (AlarmTripped == true))
	{
		//Alarm should be reset
		AlarmTripped = false;
	}
	if(AlarmTripped == true) //alarm is active
	{
		if(BeepPause == false) //buzzer shall be sounded
			{
			Beep(100);
			BeepPause = true;
			}
		else //buzzer shall not be sounded (pause)
		{
			DelayMs(200);
			BeepPause = false;
		}
	}
	else //alarm is not active
	{
		//this is not really necessary
		//DelayMs(200);
	}
}

void DelayMs(uint32_t Milliseconds)
//Simple delay loop which takes clock frequency into consideration
{
	uint32_t LoopIndex;
	uint32_t Cycles = (Milliseconds * SystemCoreClock) / (13 * 1000);
//	DWT->CYCCNT = 0; //reset cycle counter
	for(LoopIndex=0; LoopIndex <= Cycles; LoopIndex++); //one empty loop cycle is approx. 13 cycles
//	temp = DWT->CYCCNT - 1; //How many cycles were taken by the for loop?
}

void Beep(uint32_t Periods)
//Beep for number Periods time. One beep is comprised of turning the buzzer on for 1ms and off for 1ms
{
	uint32_t LoopIndex;
	for(LoopIndex=0; LoopIndex <= Periods; LoopIndex++)
	{
		GPIOA->ODR |= (1 << 11);
		DelayMs(1);
		GPIOA->ODR &= ~(1 << 11);
		DelayMs(1);
	}
}
