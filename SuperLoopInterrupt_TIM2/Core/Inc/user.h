/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USER_H
#define __USER_H

void UserInit(void);
void UserLoop(void);
void TIM2_IRQHandler(void);


struct //Control flags for program execution
{
uint8_t TSE:1; //time slice elapsed
uint8_t SEE:1; //second elapsed
uint8_t MEE:1; //minute elapsed
//add more flags as needed . . .
}
Flags ;



#endif /* __USER_H */
