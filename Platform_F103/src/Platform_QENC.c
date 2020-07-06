/*
 * Platform_QENC.c
 *
 */
#ifdef USE_QENC
#include "Platform.h"
#include "Globals.h"

// Quadrature encoder interface setup
// Pass 0 for maximal 16 count length, otherwise desired counts
// filter 0 (none) to 15 (max)


void QuadCounterInit(   TIM_TypeDef *TIMx, uint32_t MaxCount, uint8_t Edges, uint8_t Filter, bool Polarity)
{
InitTimerClock(TIMx);

TIMx->SMCR = Edges & 3;
TIMx->SMCR |= (Filter & 0x0f) << 8;
TIMx->ARR = MaxCount-1;
TIMx->CCMR1 = (Polarity ? 2 : 1 )| (1<<8); // Only need to invrt 1 pin to reverse direction
TIMx->CCER = 2 | (2<<4); // enable inputs
TIMx->CR1 = 1; // Enable counter
}

int16_t QuadEncoderRead(TIM_TypeDef *TIMx)
{
return TIMx->CNT;
}

#endif
