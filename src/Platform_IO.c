/*
 * Platform_IO.c
 *
 */

#include "Platform.h"
#include "Globals.h"

// Note pullup/ down for inputs is selected in ODR reg: bit(x) 0 PD, 1 PU!!!!

#define CFGMODEBITS 	((uint32_t)15) 	// CNF xx, MODE xx
#define CFGIN_FLOAT 	((uint32_t)4) 	// CNF 01, MODE 00
#define CFGIN_PULL  	((uint32_t)8)	// CNF 10, MODE 00
#define CFGOUT_PP   	((uint32_t)3) 	// CNF 00, MODE 11 -  highest speed output
#define CFGOUT_OD   	((uint32_t)7) 	// CNF 01, MODE 11 -  highest speed output
#define CFGAF_PP    	((uint32_t)11)	// CNF 10, MODE 11 -  highest speed output
#define CFGAF_OD    	((uint32_t)15)	// CNF 11, MODE 11 -  highest speed output

void InitIOBit(GPIO_TypeDef* Port, uint8_t PinNo, uint8_t Mode, bool OType, uint8_t Pull)
{
__IO uint32_t *CR;
uint8_t Pin = PinNo;

if( Pin < 8)	// Point to the right configuration register
 {
 CR = &(Port->CRL);
 }
else
 {
 CR = &(Port->CRH);
 Pin -= 8;
 }

*CR &= ~(CFGMODEBITS << (Pin * 4)); // Clear the config and mode bits for this pin

switch(Mode)
 {
 case GPIO_Mode_IN:
		      switch( Pull)
		       {
		       case GPIO_PuPd_NP:
					      *CR |= CFGIN_FLOAT << (Pin * 4); // Set IO, no PUPD
					      break;
		       case GPIO_PuPd_PD:
					      *CR &= ~(CFGIN_PULL << (Pin * 4)); // Set IO, PU/PD
					      Port->ODR &= ~(1<<PinNo); // Clear PU (in ODR)
					      break;
		       case GPIO_PuPd_PU:
					      *CR |= CFGIN_PULL << (Pin * 4);
					      Port->ODR |= (1<<PinNo); // Set PU (in ODR!!)
					      break;
		       }
 			break;

 case GPIO_Mode_OUT:
		      if( OType == GPIO_OType_OD)
		       {
		       *CR |= CFGOUT_OD << (Pin *4);
		       }
		      else
		       {
		       *CR |= CFGOUT_PP << (Pin *4);
		       }
		      break;

 case GPIO_Mode_AF:
		      if( OType == GPIO_OType_OD)
		       {
		       *CR |= CFGAF_OD << (Pin *4);
		       }
		      else
		       {
		       *CR |= CFGAF_PP << (Pin *4);
		       }
		      break;

  }


}

void WriteIOBit(GPIO_TypeDef* Port, uint16_t Pin, bool bState)
{
if( bState)
 {
 Port->BSRR = (1<<Pin);
 }
else
 {
 Port->BSRR = (0x10000<<Pin);
 }
}

void ToggleIOBit(GPIO_TypeDef* Port, uint16_t Pin)
{
WriteIOBit(Port,Pin, !ReadIOBit(Port,Pin));
}

void WriteIOVect(GPIO_TypeDef* Port, uint16_t Vect, uint16_t Mask)
{
Port->BSRR = (Vect & Mask) | ((~Vect&Mask)<<16);
}

bool ReadIOBit(GPIO_TypeDef* Port, uint16_t Pin)
{
return  ((Port->IDR) & (1<<Pin)) ? true : false;
}


/**************** Pin interrupts ************************/

#define EXTI0 6
#define EXTI1 7
#define EXTI2 8
#define EXTI3 9
#define EXTI4 10
#define EXTI5_9 23
#define EXTI10_15 40

void IOPinIRQEnable(GPIO_TypeDef* Port, uint16_t Pin)
{
Port = Port;
EXTI->IMR |= Bit(Pin);
}

void IOPinIRQDisable(GPIO_TypeDef* Port, uint16_t Pin)
{
Port = Port;
EXTI->IMR &= ~Bit(Pin);
}

void IOPinIRQInit( GPIO_TypeDef* Port, uint16_t Pin, uint8_t edge)
{
uint32_t NVICReg = 0;
uint32_t NVICBit = 0;

switch(Pin)
 {
 case 0 ... 4:
 		NVICReg = 0;
 		NVICBit = Bit(Pin+6);
 		break;
 case 5 ... 9:
 		NVICReg = 0;
 		NVICBit = Bit(23);	// still in reg 0
 		break;
 case 10 ... 15:
 		NVICReg = 1;
 		NVICBit = Bit(40-32); // this one's in reg 1
 		break;
 }

switch( (uint32_t ) Port)
 {
 case (uint32_t)GPIOA:
		AFIO->EXTICR[Pin/4] &=  ~(0x00ff << (Pin*4));
		break;

 case (uint32_t)GPIOB:
		AFIO->EXTICR[Pin/4] &=  ~(0x00ff << (Pin*4));
		AFIO->EXTICR[Pin/4] |=  0x0001 << (Pin*4);
		break;

 case (uint32_t)GPIOC:
		AFIO->EXTICR[Pin/4] &=  ~(0x00ff << (Pin*4));
		AFIO->EXTICR[Pin/4] |=  0x0002 << (Pin*4);
		break;

 }

switch(edge)
 {
 case EXTIRQ_RISING:
 		EXTI->RTSR |= Bit(Pin);
 		break;
 case EXTIRQ_FALLING:
 		EXTI->FTSR |= Bit(Pin);
 		break;
 case EXTIRQ_BOTH:
 		EXTI->RTSR |= Bit(Pin);
 		EXTI->FTSR |= Bit(Pin);
 		break;

 }

IOPinIRQEnable(Port, Pin);

NVIC->ISER[NVICReg] |= NVICBit;
}

_WEAK void PinInterrupt(uint8_t Line)
{
Line = Line;
}

void EXTIRQ_Handler(uint8_t Line)
{
EXTI->PR |= Bit(Line); // Clear the interrupt
PinInterrupt(Line);
}

void EXTI0_IRQHandler(void)
{
EXTIRQ_Handler(0);
}

void EXTI1_IRQHandler(void)
{
EXTIRQ_Handler(1);
}

void EXTI2_IRQHandler(void)
{
EXTIRQ_Handler(2);
}

void EXTI3_IRQHandler(void)
{
EXTIRQ_Handler(3);
}

void EXTI4_IRQHandler(void)
{
EXTIRQ_Handler(4);
}

void EXTI5_IRQHandler(void)
{
EXTIRQ_Handler(5);
}

void EXTI9_5_IRQHandler(void)
{
uint8_t i;
for( i=6; i<10; i++)
 {
 if( EXTI->PR & Bit(i))
  {
  EXTIRQ_Handler(i);
  }
 }
}

void EXTI15_10_IRQHandler(void)
{
uint8_t i;
for( i=10; i<16; i++)
 {
 if( EXTI->PR & Bit(i))
  {
  EXTIRQ_Handler(i);
  }
 }
}
