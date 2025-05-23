#include "stm32f4xx.h"
#include "bsp.h"
#include "user.h"
#include <stdio.h>


void UserInit(void)
{
	BoardInit(true);
	__disable_irq();
	//Configure TIM2 interrupt to interrupt every 100ms @20MHz clock
	RCC->APB1ENR |= 1; // clock enable for Timer 2
	//Since clock frequency is 20MHz, we have to divide it by 2e6 in total
	TIM2->PSC = 2000-1; // prescaler : divide by 2000
	TIM2->ARR = 1000-1; //auto reload value : divide by another 1000
	TIM2->CR1 = 1; //enable counter in ( control register 1)
	TIM2->DIER |= 1; //enable update interrupt (DMA / interrupt enable register )
	NVIC_EnableIRQ(TIM2_IRQn); //enable interrupt in NVIC
	__enable_irq (); //enable global interrupts
}


void UserLoop(void)
{
	while(Flags.TSE==0); //wait until time slice elapsed flag has been set by ISR
	Flags.TSE=0; // clear time slice elapsed flag
	//and do tasks
	LEDToggle(LED_GREEN_RIGHT);
}


void TIM2_IRQHandler(void) //ISR for setting time slice elapsed flag
{
	Flags .TSE=1; // set the flag
	TIM2->SR &= ~1; // reset update interrupt flag
}
