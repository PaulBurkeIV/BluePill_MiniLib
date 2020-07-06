/*
 * Platform_EVTIM.c
 *
 */
#ifdef USE_EVTIM

#include <stdint.h>
#include <stdbool.h>

#include "Platform.h"
#include "Globals.h"
#include "print.h"
#include "Platform_EVTIM.h"


#define RCC_AHBPeriph_DMA1 Bit(0)

#define CHAN(x) ((x)-1) // Because numbers start from 1, array from 0

#define TIM1_CH1_DMA CHAN(2)
#define TIM1_CH3_DMA CHAN(6)
#define TIM1_CH4_DMA CHAN(4)

#define TIM2_CH1_DMA CHAN(5)
#define TIM2_CH2_DMA CHAN(7)
#define TIM2_CH3_DMA CHAN(1)
#define TIM2_CH4_DMA CHAN(7)

#define TIM3_CH1_DMA CHAN(6)
#define TIM3_CH3_DMA CHAN(2)
#define TIM3_CH4_DMA CHAN(3)

#define TIM4_CH1_DMA CHAN(1)
#define TIM4_CH2_DMA CHAN(4)
#define TIM4_CH3_DMA CHAN(5)

#define TIM1CH1 ((uint32_t)TIM1 + 1)
#define TIM1CH3 ((uint32_t)TIM1 + 3)
#define TIM1CH4 ((uint32_t)TIM1 + 4)

#define TIM2CH1 ((uint32_t)TIM2 + 1)
#define TIM2CH2 ((uint32_t)TIM2 + 2)
#define TIM2CH3 ((uint32_t)TIM2 + 3)
#define TIM2CH4 ((uint32_t)TIM2 + 4)

#define TIM3CH1 ((uint32_t)TIM3 + 1)
#define TIM3CH3 ((uint32_t)TIM3 + 3)
#define TIM3CH4 ((uint32_t)TIM3 + 4)

#define TIM4CH1 ((uint32_t)TIM4 + 1)
#define TIM4CH2 ((uint32_t)TIM4 + 2)
#define TIM4CH3 ((uint32_t)TIM4 + 3)

#define DMA_16BIT (0x05<<8)
#define DMA_MemoryInc_Enable Bit(7)
#define DMA_Mode_Circular Bit(5)
#define DMA_Priority_Medium Bit(12)

void SetupEventTimer(TIM_TypeDef *TIM, uint16_t Chann, uint16_t Div)
{
uint32_t ClockSpeed;
if( !(ClockSpeed = InitTimerClock(TIM)))
 return;

RCC->AHBENR |= RCC_AHBPeriph_DMA1;
TIM->CR1 &= ~ENABLE;

switch(Chann)
 {
 case 1:
 	TIM->CCMR1 = (3<<12) | 1;
 	TIM->DIER = Bit(9);
	TIM->CCER = Bit(0);
 	break;

 case 2:
 	TIM->CCMR1 = (3<<12) | (1<<8);
 	TIM->DIER = Bit(10);
	TIM->CCER = Bit(4);
 	break;
 case 3:
 	TIM->CCMR2 = (3<<4) | 1;
 	TIM->DIER = Bit(11);
 	TIM->CCER = Bit(8);
	break;
 case 4:
 	TIM->CCMR2 = (3<<12) | (1<<8);
 	TIM->DIER = Bit(12);
 	TIM->CCER = Bit(12);
	break;
 }

TIM->CR1 = 0;
TIM->ARR = 0xffff;
TIM->PSC = (Div-1);
TIM->RCR = 0;
TIM->EGR = Bit(1);
TIM->CR1 |= 1;
}


void InitEventTimer(t_EventTimer *EventTimer )
{
t_DMA_Channel *DMA1Channel =  (t_DMA_Channel *)DMA1_Channel1;
t_DMA_Channel *DMA_Channel = 0;

uint16_t Chann = 0;

uint32_t TimChann = (uint32_t)EventTimer->TIM + EventTimer->Channel;

switch( TimChann)
 {
 case TIM1CH1:
 		DMA_Channel = &DMA1Channel[TIM1_CH1_DMA];
 		Chann = 1;
 		break;

 case TIM1CH3:
 		DMA_Channel = &DMA1Channel[TIM1_CH3_DMA];
 		Chann = 3;
 		break;

 case TIM1CH4:
 		DMA_Channel = &DMA1Channel[TIM1_CH4_DMA];
 		Chann = 4;
 		break;

 case TIM2CH1:
 		DMA_Channel = &DMA1Channel[TIM2_CH1_DMA];
 		Chann = 1;
 		break;

 case TIM2CH2:
 		DMA_Channel = &DMA1Channel[TIM2_CH2_DMA];
 		Chann = 2;
 		break;

 case TIM2CH3:
 		DMA_Channel = &DMA1Channel[TIM2_CH3_DMA];
 		Chann = 3;
 		break;

 case TIM2CH4:
 		DMA_Channel = &DMA1Channel[TIM2_CH4_DMA];
 		Chann = 4;
 		break;

 case TIM3CH1:
 		DMA_Channel = &DMA1Channel[TIM3_CH1_DMA];
 		Chann = 1;
 		break;

 case TIM3CH3:
 		DMA_Channel = &DMA1Channel[TIM3_CH3_DMA];
 		Chann = 3;
 		break;

 case TIM3CH4:
 		DMA_Channel = &DMA1Channel[TIM3_CH4_DMA];
 		Chann = 4;
 		break;

 case TIM4CH1:
 		DMA_Channel = &DMA1Channel[TIM4_CH1_DMA];
 		Chann = 1;
 		break;

 case TIM4CH2:
 		DMA_Channel = &DMA1Channel[TIM4_CH2_DMA];
 		Chann = 2;
 		break;

 case TIM4CH3:
 		DMA_Channel = &DMA1Channel[TIM4_CH3_DMA];
 		Chann = 3;
 		break;

 default:
 		return;
 }

EventTimer->Channel = Chann;
EventTimer->DMA = DMA_Channel;
SetupEventTimer( EventTimer->TIM, Chann, EventTimer->Div);

EventTimer->OutCnt = 0;

DMA_Channel->CCR =  DMA_MemoryInc_Enable | DMA_16BIT | DMA_Mode_Circular | DMA_Priority_Medium;
DMA_Channel->CNDTR = EventTimer->BufSz;
DMA_Channel->CPAR = (uint32_t)&EventTimer->TIM->CCR1;
DMA_Channel->CMAR = (uint32_t)EventTimer->Buff;
DMA_Channel->CCR |= 1; // Enable the DMA
}

void InvertEventTimer( t_EventTimer *EventTimer )
{
TIM_TypeDef *TIM = EventTimer->TIM;
uint8_t Chann = EventTimer->Channel;

switch(Chann)
 {
 case 1:
	TIM->CCER ^= Bit(1);
 	break;
 case 2:
	TIM->CCER ^= Bit(5);
 	break;
 case 3:
	TIM->CCER ^= Bit(9);
	break;
 case 4:
	TIM->CCER ^= Bit(12);
	break;
 }
}

uint16_t EventTimerDataRdy(t_EventTimer *EventTimer)
{
return ((EventTimer->BufSz - EventTimer->DMA->CNDTR) - EventTimer->OutCnt ) & ((EventTimer->BufSz)-1);
}

uint16_t ReadEventTimer(t_EventTimer *EventTimer)
{
uint16_t t = EventTimer->Buff[EventTimer->OutCnt];
EventTimer->OutCnt = (EventTimer->OutCnt+1) & ((EventTimer->BufSz)-1);
return t;
}

#endif //USE_EVTIM
