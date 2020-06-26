#ifdef USE_ADC
/*
 * Platform_ADC.c
 *
 *  Created on: 5 May 2020
 *      Author: AmlodipineFiend
 */

#include "Platform.h"
#include "Globals.h"


#define ADC1CLK Bit(9)		//APB2
#define ADC2CLK Bit(10)		//APB2
#define ADC3CLK Bit(15)		//APB2

#define SINGLE Bit(13)
#define DISCONTINUOUS Bit(11)
#define STARTCONV Bit(22)
#define RSTCAL Bit(3)
#define CALIBRATE Bit(2)
#define ADCON Bit(0)
#define START_CONV Bit(0)
#define EOC Bit(1)

void ADCInit( ADC_TypeDef *ADC)
{
switch( (uint32_t)ADC)
 {
 case (uint32_t)ADC1:
 			RCC->APB2RSTR |= ADC1CLK ; // Reset the peripheral
 			RCC->APB2RSTR &= ~ADC1CLK ;
 			RCC->APB2ENR |= ADC1CLK ; // Enable clock
 			break;

 case (uint32_t)ADC2:
 			RCC->APB2RSTR |= ADC2CLK ; // Reset the peripheral
 			RCC->APB2RSTR &= ~ADC2CLK ;
 			RCC->APB2ENR |= ADC2CLK ; // Enable clock
 			break;

// Not available on BluePill
// case (uint32_t)ADC3:
// 			RCC->APB2RSTR |= ADC3CLK ; // Reset the peripheral
// 			RCC->APB2RSTR &= ~ADC3CLK ;
// 			RCC->APB2ENR |= ADC3CLK ; // Enable clock
// 			break;
 }

ADC->CR2 = ADCON;

// do a calibration sequence
ADC->CR2 |= RSTCAL;
while( ADC->CR2 & RSTCAL)
 {}
ADC->CR2 |= CALIBRATE;
while( ADC->CR2 & CALIBRATE)
 {}

ADC->CR1 = SINGLE | DISCONTINUOUS;
}

void StartADC( ADC_TypeDef *ADC, uint8_t channel)
{
ADC->SQR3 = channel;
ADC->CR2 |= START_CONV;
}

bool ADCRdy(ADC_TypeDef *ADC)
{
return ADC->SR & EOC;
}

uint16_t ReadADC(ADC_TypeDef *ADC)
{
return ADC->DR;
}
#endif //USE_ADC


