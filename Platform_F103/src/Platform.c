/****************************************************/
//
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

#include "Platform.h"
#include "stm32f10x.h"
#include "Globals.h"

#define AF_CLK 		Bit(0)
#define GPIOA_CLK 	Bit(2)
#define GPIOB_CLK 	Bit(3)
#define GPIOC_CLK 	Bit(4)
#define GPIOD_CLK 	Bit(4)
#define GPIOE_CLK 	Bit(5)
#define GPIOF_CLK 	Bit(6)
#define GPIOG_CLK 	Bit(7)

static volatile uint32_t MilliSecCnt = 0;


void InitGPIOClocks(void)
{
RCC->APB2ENR |= AF_CLK | GPIOA_CLK | GPIOB_CLK | GPIOC_CLK | GPIOD_CLK | GPIOE_CLK | GPIOF_CLK | GPIOG_CLK ; // Enable AF and PA-PC clocks
}

void InitClockOutput(uint32_t Output)
{
RCC->CFGR &= ~MCO_BITS;
RCC->CFGR |= Output & (Output);
}

void PlatformInit(void)
{
SysTick_Config(72000);	// This starts a 1ms timebase interrupt
InitGPIOClocks();	// This makes sure all the GPIO ports are clocked
MilliSecCnt = 0;
}

bool chrdy(void)
{
return UART_RxRdy(SERIAL);
}

uint16_t getchar(void)
{
return ReadUART(SERIAL);
}

void putchar(uint16_t c)
{
WriteUART(SERIAL,c);
}

void puts(uint8_t * str)
{
while( *str )
 {
 putchar(*str);
 str++;
 }
}

void HardFault_Handler(void)
{
while(1)
 {}
}


_WEAK void SysTickHook(void)
{}

void SysTick_Handler(void)
{
MilliSecCnt++;
SysTickHook();
}

uint32_t MilliSeconds(void)
{
return MilliSecCnt;
}

void WaitTicks(uint32_t t)
{
uint32_t tstart = MilliSeconds();
while((MilliSeconds()-tstart) < t)
 {}
}

// This is needed by all the different timer functions
// (uS, counter, PWM, etc) so it goes here

uint32_t InitTimerClock(TIM_TypeDef *TIM)
{
uint32_t ClockSpeed = 0;
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

 }
return ClockSpeed;
}


void InituSCounter(void)
{
uint32_t ClockSpeed = InitTimerClock(TIM_uS);

TIM_uS->ARR = 0xffff;
TIM_uS->PSC = (ClockSpeed % 1000000)-1; // Gives 1MHz counter
TIM_uS->CCMR1 = 0x01;	//input capture
TIM_uS->SR &= ~0x22; // Clear status bits
TIM_uS->EGR |= 2;		//Enable CCR1 capture event
TIM_uS->CCER |= 1;	//Enable CCR1
TIM_uS->CR1 |= TIM_CR1_CEN;
}

uint16_t ReaduSCounter(void)
{
return TIM_uS->CNT;
}
