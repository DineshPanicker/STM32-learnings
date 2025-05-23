#include <stdbool.h>
#include "stm32f4xx.h"
#include "bsp.h"
#include "stm32f4xx_hal_adc.h" //for ADC

//Hardware pin definitions, local to bsp.c
//buzzer
#define BUZZER_PIN GPIO_PIN_11
#define BUZZER_PORT GPIOA
//Pins and Ports for Keys
uint16_t KeyPins[]={GPIO_PIN_13, GPIO_PIN_2, GPIO_PIN_15, GPIO_PIN_14, GPIO_PIN_8};
GPIO_TypeDef *KeyPorts[]={GPIOC, GPIOD, GPIOB, GPIOB, GPIOA};
//Pins and Ports for LEDs
uint16_t LEDPins[]={GPIO_PIN_8, GPIO_PIN_7, GPIO_PIN_6, GPIO_PIN_3, GPIO_PIN_0, GPIO_PIN_5};
GPIO_TypeDef *LEDPorts[]={GPIOC, GPIOC, GPIOC, GPIOC, GPIOC, GPIOA};
//potentiometer
#define Potentiometer_Pin GPIO_PIN_4
#define Potentiometer_GPIO_Port GPIOA
//Pins and Ports for Joystick (x-axis, y-axis, button)
uint16_t JoystickPins[]={GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_8};
GPIO_TypeDef *JoystickPorts[]={GPIOA, GPIOA, GPIOA};
//DHT temperature and humidity sensor
#define DHT_Pin GPIO_PIN_12
#define DHT_GPIO_Port GPIOA
//infrared receiver
#define IRData_Pin GPIO_PIN_15
#define IRData_GPIO_Port GPIOA
//NTC temperature sensor
#define NTC_Pin GPIO_PIN_6
#define NTC_GPIO_Port GPIOA
//LDR resistor
#define LDR_Pin GPIO_PIN_7
#define LDR_GPIO_Port GPIOA
//Seven segment display
//Pins and Ports for Anodes
uint16_t AnodePins[]={GPIO_PIN_12, GPIO_PIN_11, GPIO_PIN_10, GPIO_PIN_9};
GPIO_TypeDef *AnodePorts[]={GPIOC, GPIOC, GPIOC, GPIOC};
//Pins and Ports for Cathodes
uint16_t CathodePins[]={GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_12, GPIO_PIN_13};
GPIO_TypeDef *CathodePorts[]={GPIOB, GPIOB, GPIOB, GPIOB, GPIOB, GPIOB, GPIOB, GPIOB, GPIOB};

//global variables
//lookup table for SSD
const uint8_t SegmentsLookupTable[] = {0b00111111, 0b00000110,
									0b01011011, 0b01001111,
									0b01100110, 0b01101101,
									0b01111101, 0b00000111,
									0b01111111, 0b01101111
									};
//Handle for Joystick ADC
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

//private functions, only called in this file
void JoystickADCInit(void);
void PotNTCLDRADCInit(void);
void TIM1_Init(void);
uint16_t IntegerToBCD(uint16_t IntegerValue);

//function definitions

void BoardInit(bool InitBuzzerPinDrive)
{
	//struct variable for configuring GPIO pins
	GPIO_InitTypeDef myGPIOInit;
	//Enable Clocks for required GPIO ports
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	//Configure Output pins
	//all outputs share the same following features:
	myGPIOInit.Mode = GPIO_MODE_OUTPUT_PP;
	myGPIOInit.Pull = GPIO_NOPULL;
	myGPIOInit.Speed = GPIO_SPEED_FREQ_LOW;
	//now configure all pins using these settings
	for(uint8_t Index=0; Index<sizeof(LEDPins)/sizeof(LEDPins[0]); Index++)
	{
		myGPIOInit.Pin = LEDPins[Index];
		HAL_GPIO_Init(LEDPorts[Index], &myGPIOInit);
	}
	//turn all LEDs off
	for(uint8_t Index=0; Index<sizeof(LEDPins)/sizeof(LEDPins[0]); Index++)
	{
		LEDTurnOff(Index);
	}
	if(InitBuzzerPinDrive == true)
	{
		myGPIOInit.Pin = BUZZER_PIN;
		HAL_GPIO_Init(BUZZER_PORT, &myGPIOInit);
	}
	else //timer 1 is used to directly drive the buzzer
	{
		TIM1_Init();
	}
	for(uint8_t Index=0; Index<sizeof(AnodePins)/sizeof(AnodePins[0]); Index++)
	{
		myGPIOInit.Pin = AnodePins[Index];
		HAL_GPIO_Init(AnodePorts[Index], &myGPIOInit);
	}
	SSDTurnOff();
	for(uint8_t Index=0; Index<sizeof(CathodePins)/sizeof(CathodePins[0]); Index++)
	{
		myGPIOInit.Pin = CathodePins[Index];
		HAL_GPIO_Init(CathodePorts[Index], &myGPIOInit);
	}
	//Configure Input Pins
	myGPIOInit.Mode = GPIO_MODE_INPUT;
	myGPIOInit.Pull = GPIO_NOPULL;
	for(uint8_t Index=0; Index<sizeof(KeyPins)/sizeof(KeyPins[0]); Index++)
	{
		myGPIOInit.Pin = KeyPins[Index];
		HAL_GPIO_Init(KeyPorts[Index], &myGPIOInit);
	}
	myGPIOInit.Pin = DHT_Pin;
	HAL_GPIO_Init(DHT_GPIO_Port, &myGPIOInit);
	myGPIOInit.Pin = IRData_Pin;
	HAL_GPIO_Init(IRData_GPIO_Port, &myGPIOInit);
	//Configure ADC requiring peripherals
	JoystickADCInit(); //Joystick
	PotNTCLDRADCInit(); //Potentiometer, NTC and LDR all use same ADC
}


void TIM1_Init(void)
{
	TIM_HandleTypeDef htim1;
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};
	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};
	htim1.Instance = TIM1;
	htim1.Init.Prescaler = 0;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = 5000;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1.Init.RepetitionCounter = 0;
	htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	HAL_TIM_Base_Init(&htim1);
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig);
	HAL_TIM_OC_Init(&htim1);
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig);
	sConfigOC.OCMode = TIM_OCMODE_TOGGLE;
	sConfigOC.Pulse = 5000;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	HAL_TIM_OC_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_4);
	sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
	sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
	sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
	sBreakDeadTimeConfig.DeadTime = 0;
	sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
	sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
	sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
	HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig);
	//Configure Pin for timer
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOA_CLK_ENABLE();
	/**TIM1 GPIO Configuration
	PA11     ------> TIM1_CH4
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}




//Turns the respective LED on. You may either use the defined LED colors in bsp.h or
//numbers for the respective LEDs
void LEDTurnOn(uint8_t LED)
{
	if(LED >= 0 && LED < sizeof(LEDPins)/sizeof(LEDPins[0]))
	{
		HAL_GPIO_WritePin(LEDPorts[LED], LEDPins[LED], GPIO_PIN_SET);
	}
}


//Turns the respective LED on. You may either use the defined LED colors in bsp.h or
//numbers for the respective LEDs
void LEDTurnOff(uint8_t LED)
{
	if(LED >= 0 && LED < sizeof(LEDPins)/sizeof(LEDPins[0]))
	{
		HAL_GPIO_WritePin(LEDPorts[LED], LEDPins[LED], GPIO_PIN_RESET);
	}
}


//Turns the respective LED on. You may either use the defined LED colors in bsp.h or
//numbers for the respective LEDs
void LEDToggle(uint8_t LED)
{
	if(LED >= 0 && LED < sizeof(LEDPins)/sizeof(LEDPins[0]))
	{
		HAL_GPIO_TogglePin(LEDPorts[LED], LEDPins[LED]);
	}
}

void LEDTurnOnAll(void)
{
	for(uint8_t Index=0; Index<sizeof(LEDPins)/sizeof(LEDPins[0]); Index++)
	{
		HAL_GPIO_WritePin(LEDPorts[Index], LEDPins[Index], GPIO_PIN_SET);
	}
}

void LEDTurnOffAll(void)
{
	for(uint8_t Index=0; Index<sizeof(LEDPins)/sizeof(LEDPins[0]); Index++)
	{
		HAL_GPIO_WritePin(LEDPorts[Index], LEDPins[Index], GPIO_PIN_RESET);
	}
}


//turn the buzzer on
void BuzzerOn(void)
{
	HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_SET);
}

//turn the buzzer off
void BuzzerOff(void)
{
	HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_RESET);
}


//toggle the state of the buzzer
void BuzzerToggle(void)
{
	HAL_GPIO_TogglePin(BUZZER_PORT, BUZZER_PIN);
}


//Reads the state of the Rich shield keys K1 and K2, returns true if the respective button has been pressed
//The function detects negative edges on the respective pins. In order to be not affected by bouncing, you have
//to make sure that there is a pause (5ms or more) between calling the function repeatedly.
uint8_t KeyRead(uint8_t Key)
{
	uint8_t ReturnValue = false;
	static GPIO_PinState KeysOldState[]={GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET};
	GPIO_PinState KeyNewState;

	KeyNewState = HAL_GPIO_ReadPin(KeyPorts[Key], KeyPins[Key]);
	if((KeyNewState == GPIO_PIN_RESET) && (KeysOldState[Key] == GPIO_PIN_SET))
	{
		//negative edge has occured, key has been pressed
		ReturnValue = true;
	}
	//save key state for next function call
	KeysOldState[Key] = KeyNewState;
	return ReturnValue;
}


//turn off all seven segment display digits
void SSDTurnOff(void)
{
	for(uint8_t Index=0; Index<sizeof(AnodePins)/sizeof(AnodePins[0]); Index++)
	{
		HAL_GPIO_WritePin(AnodePorts[Index], AnodePins[Index], GPIO_PIN_SET);
	}
}


//turn off selected digit
void SSDTurnOffDigit(uint8_t Digit)
{
	HAL_GPIO_WritePin(AnodePorts[Digit], AnodePins[Digit], GPIO_PIN_SET);
}

//turn off selected digit
void SSDTurnOnDigit(uint8_t Digit)
{
	HAL_GPIO_WritePin(AnodePorts[Digit], AnodePins[Digit], GPIO_PIN_SET);
}


//Write a numeric value to a certain digit location on SSD
int SSDWrite(uint8_t Value, bool SetDP, uint8_t Digit)
{
	//Check value ranges
	uint8_t Segments;
	if((Value>9) || (Digit>3)) return -1;
	//get segment data
	Segments = SegmentsLookupTable[Value];
	if(SetDP) Segments |= 0x80;
	for(uint8_t Index=0; Index<8; Index++)
	{
		if((Segments>>Index) & 0x01)
		{
			HAL_GPIO_WritePin(CathodePorts[Index], CathodePins[Index], GPIO_PIN_RESET);
		}
		else
		{
			HAL_GPIO_WritePin(CathodePorts[Index], CathodePins[Index], GPIO_PIN_SET);
		}
	}
	//turn on digit
	HAL_GPIO_WritePin(AnodePorts[Digit], AnodePins[Digit], GPIO_PIN_RESET);
	return 0;
}


//Write a value to the display. This function needs to be called continously, at least every 10ms
void UpdateDisplay(uint16_t Value)
{
	static uint8_t CurrentDigit=0;
	SSDWrite((IntegerToBCD(Value) >> (CurrentDigit * 4) & 0x0f), false, CurrentDigit);
	//turn off previous digit
	if(CurrentDigit > 0)
	{
		SSDTurnOffDigit(CurrentDigit-1);
	}
	else
	{
		SSDTurnOffDigit(3);
	}
	CurrentDigit++;
	if(CurrentDigit > 3) CurrentDigit = 0;
}


void UpdateDisplayArbitrarySequence(uint16_t Value)
{
	uint8_t DigitOrder[] = {2, 0, 3, 1};
	static uint8_t CurrentDigitIndex=0;
	SSDWrite((IntegerToBCD(Value) >> (DigitOrder[CurrentDigitIndex] * 4) & 0x0f), false, DigitOrder[CurrentDigitIndex]);
	//turn off previous digit
	if(CurrentDigitIndex > 0)
	{
		SSDTurnOffDigit(DigitOrder[CurrentDigitIndex-1]);
	}
	else
	{
		SSDTurnOffDigit(DigitOrder[3]);
	}
	CurrentDigitIndex++;
	if(CurrentDigitIndex > 3) CurrentDigitIndex = 0;
}



//Convert an integer number to binary representation
uint16_t IntegerToBCD(uint16_t IntegerValue)
{
	uint16_t BCDValue=0;
	uint16_t Shift = 0;
	while (IntegerValue > 0)
	{
		BCDValue |= (IntegerValue % 10) << (Shift++ << 2);
		IntegerValue /= 10;
   }
	return BCDValue;
}

//Initialize ADC1 for joystick usage. Channel0 and Channel1 are used by the two axis
void JoystickADCInit(void)
{
	GPIO_InitTypeDef myGPIOInit;
	__ADC1_CLK_ENABLE();
	__GPIOA_CLK_ENABLE();

	myGPIOInit.Pin = GPIO_PIN_0;
	myGPIOInit.Mode = GPIO_MODE_ANALOG;
	myGPIOInit.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &myGPIOInit);
	myGPIOInit.Pin = GPIO_PIN_1;
	HAL_GPIO_Init(GPIOA, &myGPIOInit);

	ADC_ChannelConfTypeDef sConfig = {0};
	/** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
	 */
	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
	hadc1.Init.Resolution = ADC_RESOLUTION_12B;
	hadc1.Init.ScanConvMode = ENABLE;
	hadc1.Init.ContinuousConvMode = DISABLE;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.NbrOfConversion = 2;
	hadc1.Init.DMAContinuousRequests = DISABLE;
	hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	if (HAL_ADC_Init(&hadc1) != HAL_OK)
	{
	}
	/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	 */
	sConfig.Channel = ADC_CHANNEL_0;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
	}
	/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	 */
	sConfig.Channel = ADC_CHANNEL_1;
	sConfig.Rank = 2;
	sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
	}
}

//Read both axis of the joystick, return pointer on result array
uint16_t* JoystickReadAxis(void)
{
	static uint16_t Axis[2]; //contains x- and y-axis respectively
	//Start ADC conversion cycle to obtain readings from controller
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
	Axis[1] = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
	Axis[0] = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);
	//Compensate for wrong direction of both joystick potentiometers
	Axis[0] = 4095 - Axis[0];
	Axis[1] = 4095 - Axis[1];
	return Axis; //return pointer
}


//Initialize ADC2 for usage with Potentiometer, NTC and LDR
void PotNTCLDRADCInit(void)
{
	GPIO_InitTypeDef myGPIOInit;
	__ADC2_CLK_ENABLE();
	__GPIOA_CLK_ENABLE();

	myGPIOInit.Pin = GPIO_PIN_4;
	myGPIOInit.Mode = GPIO_MODE_ANALOG;
	myGPIOInit.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &myGPIOInit);
	myGPIOInit.Pin = GPIO_PIN_6;
	HAL_GPIO_Init(GPIOA, &myGPIOInit);
	myGPIOInit.Pin = GPIO_PIN_7;
	HAL_GPIO_Init(GPIOA, &myGPIOInit);

	ADC_ChannelConfTypeDef sConfig = {0};
	/** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
	 */
	hadc2.Instance = ADC2;
	hadc2.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
	hadc2.Init.Resolution = ADC_RESOLUTION_12B;
	hadc2.Init.ScanConvMode = ENABLE;
	hadc2.Init.ContinuousConvMode = DISABLE;
	hadc2.Init.DiscontinuousConvMode = DISABLE;
	hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc2.Init.NbrOfConversion = 3;
	hadc2.Init.DMAContinuousRequests = DISABLE;
	hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	if (HAL_ADC_Init(&hadc2) != HAL_OK)
	{
	}
	/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	 */
	sConfig.Channel = ADC_CHANNEL_4;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_144CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
	{
	}
	/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	 */
	sConfig.Channel = ADC_CHANNEL_6;
	sConfig.Rank = 2;
	sConfig.SamplingTime = ADC_SAMPLETIME_144CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
	{
	}
	/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	 */
	sConfig.Channel = ADC_CHANNEL_7;
	sConfig.Rank = 3;
	sConfig.SamplingTime = ADC_SAMPLETIME_144CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
	{
	}
}


//Convert and read analog values for potentiometer, NTC and LDR. Return pointer to result array
uint16_t* PotNTCLDRADCRead(void)
{
	static uint16_t Results[3]; //contains contains digital values for pot, NTC and LDR respectively
	//Start ADC conversion cycle to obtain readings from controller
	HAL_ADC_Start(&hadc2);
	HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY);
	Results[0] = HAL_ADC_GetValue(&hadc2);
	HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY);
	Results[1] = HAL_ADC_GetValue(&hadc2);
	HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY);
	Results[2] = HAL_ADC_GetValue(&hadc2);
	HAL_ADC_Stop(&hadc2);
	return Results; //return pointer
}

//Get a new value for the potentiometer only
uint16_t PotentiometerGetValue(void)
{
	uint16_t *Result;
	Result = PotNTCLDRADCRead();
	return *Result;
}

//Get a new value for the NTC only
uint16_t NTCGetValue(void)
{
	uint16_t *Result;
	Result = PotNTCLDRADCRead();
	return *(Result + 2);
}

//Get a new value for the LDR only
uint16_t LDRGetValue(void)
{
	uint16_t *Result;
	Result = PotNTCLDRADCRead();
	return *(Result + 4);
}
