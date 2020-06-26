#ifdef USE_TIMER
/*
 * PlatformTimer.c
 * * Setup timer to capture timing of ext input
 *
 *  Created on: 2 May 2020
 *      Author: AmlodipineFiend
 */

#include <stdint.h>
#include <stdbool.h>

#include "Platform.h"
#include "Platform_Timer.h"
#include "Globals.h"

#define TIM1CLK Bit(11)		//APB2
#define TIM2CLK Bit(0)		//APB1
#define TIM3CLK Bit(1)		//APB1
#define TIM4CLK Bit(2)		//APB1

#define ENABLE  Bit(0)
#define PRELOAD Bit(3)
#define Timer_MODE (Bit(5) | Bit(6))
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

void Timer_Start(TIM_TypeDef *TIM)
{
TIM->CR1 |= ENABLE;
TIM->EGR |=  UG;
}


//TIMCapture.TIM_Channel = TIM_Channel_1;
//TIMCapture.TIM_ICPolarity = TIM_ICPolarity_BothEdge;
//TIMCapture.TIM_ICSelection = TIM_ICSelection_DirectTI;
//TIMCapture.TIM_ICPrescaler = TIM_ICPSC_DIV1;
//TIMCapture.TIM_ICFilter = 15;
//TIM_ICInit(TIM, &TIMCapture);


void Timer_Stop(TIM_TypeDef *TIM)
{
TIM->CR1 &= !ENABLE;
}

void InitTimerTimebase(TIM_TypeDef *TIM, uint32_t Frequency, uint32_t MaxCount)
{
uint32_t ClockSpeed = APB2CLKSPEED;

switch ((uint32_t)TIM )
 {
 case (uint32_t)TIM1:
 			RCC->APB2RSTR |= TIM1CLK ; // Reset the peripheral
 			RCC->APB2RSTR &= ~TIM1CLK ;
 			RCC->APB2ENR |= TIM1CLK ; // Enable clock
 			ClockSpeed = APB2CLKSPEED;
 			if( RCC->CFGR & (1<<13))	// Weirdness in the design: the timer clock gets multiplied *2 sometimes!
  			ClockSpeed *= 2;
 			break;

  case (uint32_t)TIM2:
 			RCC->APB1RSTR |= TIM2CLK ; // Reset the peripheral
 			RCC->APB1RSTR &= ~TIM2CLK ;
 			RCC->APB1ENR |= TIM2CLK ; // Enable clock
 			ClockSpeed = APB1CLKSPEED;
 			if( RCC->CFGR & (1<<10))	// Weirdness in the design: the timer clock gets multiplied *2 sometimes!
  			ClockSpeed *= 2;
 			break;

  case (uint32_t)TIM3:
 			RCC->APB1RSTR |= TIM3CLK ; // Reset the peripheral
 			RCC->APB1RSTR &= ~TIM3CLK ;
 			RCC->APB1ENR |= TIM3CLK ; // Enable clock
 			ClockSpeed = APB1CLKSPEED;
 			if( RCC->CFGR & (1<<10))	// Weirdness in the design: the timer clock gets multiplied *2 sometimes!
  			ClockSpeed *= 2;
 			break;

  case (uint32_t)TIM4:
 			RCC->APB1RSTR |= TIM4CLK ; // Reset the peripheral
 			RCC->APB1RSTR &= ~TIM4CLK ;
 			RCC->APB1ENR |= TIM4CLK ; // Enable clock
 			ClockSpeed = APB1CLKSPEED;
 			if( RCC->CFGR & (1<<10))	// Weirdness in the design: the timer clock gets multiplied *2 sometimes!
  			ClockSpeed *= 2;
 			break;

 default:		return;

 }

TIM->CR1 = 2;			// Update only on capture
TIM->ARR = 0xffffffff;
TIM->PSC = (TIM_DIV-1);
TIM->RCR = 0;
TIM->EGR = 1;			// Generate an update event to reload the Prescaler and the Repetition counter values immediately


TIM->CR1 = 2;			// Update only on capture
TIM->PSC = ClockSpeed/(Frequency*MaxCount);
TIM->ARR =  0xffffffff;
TIM->RCR = 0;
TIM->BDTR |= Bit(15);		// Only needed for TIM1
}

void InitTimerChannel(TIM_TypeDef *TIM, uint8_t Channel, uint16_t InitValue, uint8_t Edges)
{
uint16_t *CCMR;
uint16_t CCMRVal = (15<<4) | 0x01; 	// Maximum filter, input T1
			// Both edges capture

//TIM_DMAConfig(TIM, TIM_DMABase_CCR1, TIM_DMABurstLength_1Transfer);
TIM->DCR = 0x0d; // Single transfers from CCR1
//TIM_SelectCCDMA(TIM, ENABLE);
TIM->CR2 |= (1<<3);
//TIM_DMACmd( TIM, TIM_DMA_CC1, ENABLE);
TIM->DIER |= (1<<9);
//TIM_Cmd(TIM, ENABLE);
TIM->CR1 |= 1;

TIM->CCMR1 = x
TIM->CCER = 0x0a;			// Both edges capture

//TIM_DMAConfig(TIM, TIM_DMABase_CCR1, TIM_DMABurstLength_1Transfer);
TIM->DCR = 0x0d; // Single transfers from CCR1
//TIM_SelectCCDMA(TIM, ENABLE);
TIM->CR2 |= (1<<3);
//TIM_DMACmd( TIM, TIM_DMA_CC1, ENABLE);
TIM->DIER |= (1<<9);
//TIM_Cmd(TIM, ENABLE);
TIM->CR1 |= 1;

switch (Channel)
 {
 case 1:
 			CCMR = &TIM->CCMR1;
 			*CCMR &= ~LO_8;		// Clear out any old configuration
 			*CCMR |= CCMRVal & LO_8;
 			TIM->CCR1 =  0;
 			TIM->CCER = Edges;
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

#endif //USE_TIMER
