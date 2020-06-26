#ifdef USE_PWM
/*
 * PlatformPWM.c
 *
 *  Created on: 2 May 2020
 *      Author: AmlodipineFiend
 */

#include <stdint.h>
#include <stdbool.h>

#include "Platform.h"
#include "Platform_PWM.h"
#include "Globals.h"

#define ENABLE  Bit(0)
#define PRELOAD Bit(3)
#define PWM_MODE (Bit(5) | Bit(6))
#define INVERTED Bit(6)

#define LO_8 0x00ff
#define HI_8 0xff00

#define CCG1 Bit(1)
#define CCG2 Bit(2)
#define CCG3 Bit(3)
#define CCG4 Bit(4)

#define UG Bit(0)

#define CH1EN Bit(0)
#define CH2EN Bit(4)
#define CH3EN Bit(8)
#define CH4EN Bit(12)

#define CH1INV Bit(1)
#define CH2INV Bit(5)
#define CH3INV Bit(9)
#define CH4INV Bit(13)

void PWM_Start(TIM_TypeDef *TIM)
{
TIM->CR1 |= ENABLE;
TIM->EGR |=  UG;
}

void PWM_Stop(TIM_TypeDef *TIM)
{
TIM->CR1 &= !ENABLE;
}

void InitPWMTimebase(TIM_TypeDef *TIM, uint32_t Frequency, uint32_t Resolution)
{
uint32_t ClockSpeed;
if( !(ClockSpeed = InitTimerClock(TIM)))
 return;

TIM->PSC = ClockSpeed/(Frequency*Resolution)-1;
TIM->ARR = Resolution-1;
TIM->BDTR |= Bit(15);		// Only needed for TIM1
PWM_Start(TIM);
}

void InitPWMChannel(TIM_TypeDef *TIM, uint8_t Channel, uint16_t InitValue, bool Inverted)
{
uint16_t *CCMR;
uint16_t CCMRVal = PWM_MODE | PRELOAD;

switch (Channel)
 {
 case 1:
 			CCMR = &TIM->CCMR1;
 			*CCMR &= ~LO_8;		// Clear out any old configuration
 			*CCMR |= CCMRVal & LO_8;
 			TIM->CCR1 =  InitValue;
 			TIM->CCER |= CH1EN | (Inverted ? CH1INV : 0);
 			break;

 case 2:
 			CCMR = &TIM->CCMR1;
 			*CCMR &= ~HI_8;		// Clear out any old configuration
 			*CCMR |= (CCMRVal<<8) & HI_8;
 			TIM->CCR2 =  InitValue;
 			TIM->CCER |= CH2EN | (Inverted ? CH2INV : 0);
 			break;

 case 3:
 			CCMR = &TIM->CCMR2;
 			*CCMR &= ~LO_8;		// Clear out any old configuration
 			*CCMR |= CCMRVal & LO_8;
 			TIM->CCR3 =  InitValue;
 			TIM->CCER |= CH3EN | (Inverted ? CH3INV : 0);
			break;

 case 4:
 			CCMR = &TIM->CCMR2;
 			*CCMR &= ~HI_8;		// Clear out any old configuration
 			*CCMR |= (CCMRVal<<8) & HI_8;
 			TIM->CCR4 =  InitValue;
 			TIM->CCER |= CH4EN | (Inverted ? CH4INV : 0);
			break;

 }

}

void SetPWM(TIM_TypeDef *TIM, uint8_t Channel, uint16_t Value)
{
switch( Channel)
 {
 case 1:
 		TIM->CCR1 =  Value;
 		break;
 case 2:
 		TIM->CCR2 =  Value;
 		break;
 case 3:
 		TIM->CCR3 =  Value;
 		break;
 case 4:
 		TIM->CCR4 =  Value;
 		break;
 }

}
#endif //USE_PWM
