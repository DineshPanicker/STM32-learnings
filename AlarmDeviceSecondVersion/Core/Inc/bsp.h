#include <stdbool.h>

//bsp.h headerfile for the board support package. All functions accessing devices from the Rich Shield go here
#ifndef __BSP_H //prevent recursive inclusion
#define __BSP_H

//Device designators

#define LED_GREEN_LEFT 0
#define LED_RED_LEFT 1
#define LED_GREEN_RIGHT 2
#define LED_YELLOW_RIGHT 3
#define LED_RED_RIGHT 4
#define LED_BLUE_RIGHT 5

#define KEY_BLUE 0
#define KEY_RED 1
#define KEY_GREEN 2
#define KEY_WHITE 3
#define KEY_JOYSTICK 4


//function declarations
//use false, if the buzzer is to be driven by a timer output e.g.
void BoardInit(bool InitBuzzerPin);
void LEDTurnOn(uint8_t);
void LEDTurnOff(uint8_t);
void LEDTurnOnAll();
void LEDTurnOffAll();
void LEDToggle(uint8_t);
void BuzzerOn(void);
void BuzzerOff(void);
void BuzzerToggle(void);
uint8_t KeyRead(uint8_t);

void SSDTurnOff(void);
void SSDTurnOffDigit(uint8_t Digit);
void SSDTurnOnDigit(uint8_t Digit);
int SSDWrite(uint8_t Value, bool SetDP, uint8_t Digit);
uint16_t IntegerToBCD(uint16_t IntegerValue);

uint16_t* JoystickReadAxis();

uint16_t* PotNTCLDRADCRead(); //return pointer to all three values
uint16_t PotentiometerGetValue();
uint16_t NTCGetValue();
uint16_t LDRGetValue();


#endif /* __BSP_H */
