#ifdef USE_UART
/*
 * Platform_UART.c
 *
 *  Created on: 1 May 2020
 *      Author: User
 */


#include <stdint.h>
#include <stdbool.h>

#include "Platform.h"
#include "Platform_UART.h"
#include "Globals.h"

#define UART1CLK Bit(14)
#define UART2CLK Bit(17)
#define UART3CLK Bit(18)

#define NINEBIT Bit(12)
#define PARITY_E (2<<9)
#define PARITY_O (3<<9)

#define TE Bit(3)
#define RE Bit(2)

#define ENABLE Bit(13)
#define TXRDY Bit(7)
#define RXRDY Bit(5)

#ifndef UART_BUFFERED
void InitUART(USART_TypeDef *UART, uint32_t BaudRate, uint16_t Format)
{
uint32_t ClockSpeed = APB2CLKSPEED;

switch ((uint32_t)UART )
 {
 case (uint32_t)USART1:
 			RCC->APB2RSTR |= UART1CLK ; // Reset the peripheral
 			RCC->APB2RSTR &= ~UART1CLK ;
 			RCC->APB2ENR |= UART1CLK ; // Enable clock
 			ClockSpeed = APB2CLKSPEED;
 			break;

 case (uint32_t)USART2:
 			RCC->APB1RSTR |= UART2CLK ; // Reset the peripheral
 			RCC->APB1RSTR &= ~ UART2CLK ;
 			RCC->APB1ENR |= UART2CLK ; // Enable clock
 			ClockSpeed = APB1CLKSPEED;
 			break;

 case (uint32_t)USART3:
 			RCC->APB1RSTR |= UART3CLK ; // Reset the peripheral
			RCC->APB1RSTR &= ~UART3CLK ;
  			RCC->APB1ENR |= UART3CLK ; // Enable clock
 			ClockSpeed = APB1CLKSPEED;
 			break;

 default:		return;


}

UART->CR1 = ENABLE;
UART->BRR = ClockSpeed/BaudRate;

switch( Format)
 {
 case DATA_8N1:
 		UART->CR1 |= TE | RE;
 		break;

 case DATA_9N1:
 		UART->CR1 |= TE | RE | NINEBIT;
 		break;
 case DATA_8E1:
 		UART->CR1 |= TE | RE | PARITY_E;
 		break;
 case DATA_8O1:
 		UART->CR1 |= TE | RE | PARITY_O;
 		break;
 }

}

// Check for UART transmit reg clear

bool UART_TxRdy( USART_TypeDef *UART)
{
return UART->SR & TXRDY;
}

// Check for character available in UART Rx register

uint16_t UART_RxRdy( USART_TypeDef *UART)
{
return (UART->SR & RXRDY) ? 1 : 0;
}

//Write a character directly to UART Txregister (blocking)

void WriteUART(USART_TypeDef *UART, uint16_t c)
{
while( !UART_TxRdy(UART))
 {}
UART->DR = c;
}

// Read a character from UART Rx reg - undefined if not UART_RxRdy()

uint16_t ReadUART(USART_TypeDef *UART)
{
while( !UART_RxRdy(UART))
 {}
return UART->DR;
}

#else
/*************************************************************************/
// e.g. COMPort_Init( &COM1, 115200, "8N1");
//

// STM32F4xx pecific hardware init

#define RCC_APB2Periph_USART1 Bit(14)
#define RCC_APB1Periph_USART2 Bit(17)
#define RCC_APB1Periph_USART3 Bit(18)
#define UART1IRQ Bit(37-32)	// Take off 32 for 2nd word of array
#define UART2IRQ Bit(38-32)
#define UART3IRQ Bit(39-32)

#define DMA_MemoryInc_Enable Bit(7)
#define DMA_Mode_Circular Bit(5)

uint16_t UART_RxRdy(USART_TypeDef *UART);

static uint8_t  COM1RxBuff[COM1_BUFF_SIZE];
static uint8_t  COM1TxBuff[COM1_BUFF_SIZE];

static uint8_t  COM2RxBuff[COM2_BUFF_SIZE];
static uint8_t  COM2TxBuff[COM2_BUFF_SIZE];

static uint8_t  COM3RxBuff[COM3_BUFF_SIZE];
static uint8_t  COM3TxBuff[COM3_BUFF_SIZE];

typedef struct
{
  __IO uint32_t CCR;
  __IO uint32_t CNDTR;
  __IO uint32_t CPAR;
  __IO uint32_t CMAR;
  __IO uint32_t Reserved;
} t_DMA1_Channel;

const t_DMA1_Channel *DMA1Channel =  (t_DMA1_Channel *)DMA1_Channel1;
#define RCC_AHBPeriph_DMA1 Bit(0)

#define CHAN(x) ((x)-1) // Because numbers start from 1, array from 0

#define UART1DMA CHAN(5)
#define UART2DMA CHAN(6)
#define UART3DMA CHAN(3)

#define DMA_16BIT (0x05<<8)

typedef struct
{
	uint16_t					BuffSz;
	uint16_t					RxCntIn;
	uint16_t					RxCntOut;
	uint16_t					TxCntIn;
	uint16_t					TxCntOut;
	uint8_t*					BufferRx;
	uint8_t*					BufferTx;
	uint16_t					flags;
} t_sCOMPort;

#define FLAG_9BIT Bit(0)

t_sCOMPort sCOM1 = 	{
					COM1_BUFF_SIZE,
					0,0,
					0,0,
					COM1RxBuff, COM1TxBuff,
					0
					};

t_sCOMPort sCOM2 = 	{
					COM2_BUFF_SIZE,
					0,0,
					0,0,
					COM2RxBuff, COM2TxBuff,
					0
					};


t_sCOMPort sCOM3 = 	{
					COM3_BUFF_SIZE,
					0,0,
					0,0,
					COM3RxBuff, COM3TxBuff,
					0
					};



/***********************************************************/
/* Device specific code				   */
/***********************************************************/
/***********************************************************/
/* Bit definitions				   */
/***********************************************************/
#define SR_TXE Bit(7)
#define SR_RXNE Bit(5)

#define CR1_UE Bit(13)
#define CR1_M Bit(12)
#define CR1_PARITY (Bit(10)|Bit(9))
#define CR1_PAR_E Bit(9)
#define CR1_PAR_O (Bit(10)|Bit(9))
#define CR1_TXEIE Bit(7)
#define CR1_RXNEIE Bit(5)
#define CR1_TE Bit(3)
#define CR1_RE Bit(2)

#define CR2_STOP2 (Bit(12) | Bit(13))

#define CR3_DMAR Bit(6)
#define USART_StopBits(x) (((x)-1)*2)

/***********************************************************/
/*Port routines				   */
/***********************************************************/

void EnableUartTx(USART_TypeDef* UART)
{
UART->CR1 |= CR1_TXEIE;
}

void DisableUartTx(USART_TypeDef* UART)
{
UART->CR1 &= ~CR1_TXEIE;
}

void EnableUartRx(USART_TypeDef* UART)
{
UART->CR1 |= CR1_RXNEIE; //Bit(5);
}

void DisableUartRx(USART_TypeDef* UART)
{
UART->CR1 &= ~CR1_RXNEIE;
}

void DisableUartTxIRQ(USART_TypeDef* UART)
{
UART->CR1 &= ~Bit(7);
}

void EnableUartTxIRQ(USART_TypeDef* UART)
{
UART->CR1 |= Bit(7);
}

// This initialises UARTs in 16x div mode, standard UART mode only

void InitUART(USART_TypeDef *UART, uint32_t BaudRate, uint16_t Format)
{
uint32_t IRQBit;
t_DMA1_Channel *DMA_Channel = 0;
uint32_t ClockSpeed = APB2CLKSPEED;
t_sCOMPort *COMPort;

RCC->AHBENR |= RCC_AHBPeriph_DMA1;

switch( (uint32_t)UART)
 {
 case (uint32_t)USART1:
 		RCC->APB2RSTR |= RCC_APB2Periph_USART1;
 		RCC->APB2RSTR &= ~RCC_APB2Periph_USART1;
 		RCC->APB2ENR |= RCC_APB2Periph_USART1;
 		COMPort = &sCOM1;
 		COMPort->BuffSz = COM1_BUFF_SIZE;
		COMPort->RxCntIn = 0;
		COMPort->RxCntOut = 0;
		COMPort->TxCntIn = 0;
		COMPort->TxCntOut = 0;
		ClockSpeed = APB2CLKSPEED;
	 	DMA_Channel = &DMA1Channel[UART1DMA];
		IRQBit = UART1IRQ;
		break;

 case (uint32_t)USART2:
  		RCC->APB1RSTR |= RCC_APB1Periph_USART2;
 		RCC->APB1RSTR &= ~RCC_APB1Periph_USART2;
		RCC->APB1ENR |= RCC_APB1Periph_USART2;
		COMPort = &sCOM2;
 		COMPort->BuffSz = COM2_BUFF_SIZE;
		COMPort->RxCntIn = 0;
		COMPort->RxCntOut = 0;
		COMPort->TxCntIn = 0;
		COMPort->TxCntOut = 0;
		ClockSpeed = APB1CLKSPEED;
		DMA_Channel = &DMA1Channel[UART2DMA];
		IRQBit = UART2IRQ;
		break;

 case (uint32_t)USART3:
 		RCC->APB1RSTR |= RCC_APB1Periph_USART3;
 		RCC->APB1RSTR &= ~RCC_APB1Periph_USART3;
 		RCC->APB1ENR |= RCC_APB1Periph_USART3;
		COMPort = &sCOM3;
 		COMPort->BuffSz = COM3_BUFF_SIZE;
		COMPort->RxCntIn = 0;
		COMPort->RxCntOut = 0;
		COMPort->TxCntIn = 0;
		COMPort->TxCntOut = 0;
		ClockSpeed = APB1CLKSPEED;
		DMA_Channel = &DMA1Channel[UART3DMA];
		IRQBit = UART3IRQ;
		break;

 default:	// Nothing to do if no UART!
  		return;
 }

COMPort->flags = 0;

UART->BRR = ClockSpeed/BaudRate;

switch( Format)
 {
 case DATA_8N1:
 		UART->CR1 |= TE | RE;
 		break;

 case DATA_9N1:
 		UART->CR1 |= TE | RE | NINEBIT;
 		COMPort->flags |= FLAG_9BIT;
 		break;
 case DATA_8E1:
 		UART->CR1 |= TE | RE | PARITY_E;
 		break;
 case DATA_8O1:
 		UART->CR1 |= TE | RE | PARITY_O;
 		break;
 }

UART->CR1 |= CR1_TXEIE | CR1_TE | CR1_RE;

DMA_Channel->CCR = DMA_MemoryInc_Enable | DMA_Mode_Circular;
if( COMPort->flags & FLAG_9BIT)
 {
 DMA_Channel->CCR |= DMA_16BIT;
 }
 // Enable USART_Rx DMA Receive request
UART->CR3 |= CR3_DMAR;

DMA_Channel->CPAR = (uint32_t)&UART->DR;
DMA_Channel->CMAR = (uint32_t)COMPort->BufferRx;
DMA_Channel->CNDTR = COMPort->BuffSz;
DMA_Channel->CCR |= 1;

EnableUartTx(UART);
NVIC->ISER[1] |= IRQBit;
// UART device enable
UART->CR1 |= CR1_UE;
}

static t_sCOMPort *GetCOMStruct(USART_TypeDef* UART)
{
t_sCOMPort * COMPort = 0;

switch( (uint32_t)UART)
 {
 case (uint32_t)USART1:
			COMPort = &sCOM1;
			break;

 case (uint32_t)USART2:
			COMPort = &sCOM2;
			break;

 case (uint32_t)USART3:
			COMPort = &sCOM3;
			break;

 }
return COMPort;
}

uint16_t UART_RxRdy(USART_TypeDef *UART)
{
t_sCOMPort * COMPort = GetCOMStruct(UART);
t_DMA1_Channel *DMA_Channel = 0;

switch( (uint32_t)UART)
 {
 case (uint32_t)USART1:
			COMPort = &sCOM1;
			DMA_Channel = &DMA1Channel[UART1DMA];
			break;

 case (uint32_t)USART2:
			COMPort = &sCOM2;
			DMA_Channel = &DMA1Channel[UART2DMA];
			break;

 case (uint32_t)USART3:
			COMPort = &sCOM3;
			DMA_Channel = &DMA1Channel[UART3DMA];
			break;

 }
uint16_t nchrs = 0;

COMPort->RxCntIn = (COMPort->BuffSz - DMA_Channel->CNDTR);
nchrs =  (COMPort->RxCntIn - COMPort->RxCntOut) & (COMPort->BuffSz-1);

return nchrs;
}

/***********************************************************/
/* Interrupt service routines 				   */
/***********************************************************/

void UART_IRQHandler(USART_TypeDef *UART)
{
t_sCOMPort *COMPort = GetCOMStruct(UART);

// Check for transmit interrupt
if(UART->SR & SR_TXE) //USART_GetITStatus(UART, USART_IT_TXE) != RESET)
 {
 if((COMPort->flags & UART_FORMATBITS) == DATA_9N1)
  {
  if(((COMPort->TxCntOut)-(COMPort->TxCntIn)) & (COMPort->BuffSz-1))
   {
   uint16_t c = COMPort->BufferTx[COMPort->TxCntOut] + (COMPort->BufferTx[COMPort->TxCntOut+1] <<8);
   UART->DR = c; //USART_SendData(UART, c);
   COMPort->TxCntOut = (COMPort->TxCntOut+2) & (COMPort->BuffSz-1);
   }
  else
   {
   DisableUartTx(UART);
   }
  }
 else
  {
  if(((COMPort->TxCntOut)-(COMPort->TxCntIn)) & (COMPort->BuffSz-1))
   {
   UART->DR = COMPort->BufferTx[COMPort->TxCntOut]; //USART_SendData(UART, COMPort->BufferTx[COMPort->TxCntOut]);
   COMPort->TxCntOut = (COMPort->TxCntOut+1) & (COMPort->BuffSz-1);
   }
  else
   {
   DisableUartTx(UART);
   }
  UART->SR &= ~SR_TXE; // Clear interrupt
  }
 }
}

void USART1_IRQHandler(void)
{
UART_IRQHandler(USART1);
}

void USART2_IRQHandler(void)
{
UART_IRQHandler(USART2);
}

void USART3_IRQHandler(void)
{
UART_IRQHandler(USART3);
}

uint16_t ReadUART(USART_TypeDef *UART)
{
t_sCOMPort *COMPort = GetCOMStruct(UART);
uint16_t c;

c = COMPort->BufferRx[COMPort->RxCntOut];
COMPort->RxCntOut = (COMPort->RxCntOut+1)&(COMPort->BuffSz-1);

if( COMPort->flags & FLAG_9BIT)
 {
 c |= COMPort->BufferRx[COMPort->RxCntOut] << 8;
 COMPort->RxCntOut = (COMPort->RxCntOut+1)&(COMPort->BuffSz-1);
 }
return c;
}

void WriteUART(USART_TypeDef *UART, uint16_t c)
{
t_sCOMPort *COMPort = GetCOMStruct(UART);

DisableUartTxIRQ(UART); // make sure pointers aren't changed by Tx interrupt
COMPort->BufferTx[COMPort->TxCntIn] = c;
COMPort->TxCntIn = (COMPort->TxCntIn+1)&(COMPort->BuffSz-1);
if((COMPort->flags & UART_FORMATBITS) == DATA_9N1)
 {
 COMPort->BufferTx[COMPort->TxCntIn] = c >> 8;
 COMPort->TxCntIn = (COMPort->TxCntIn+1)&(COMPort->BuffSz-1);
 }
EnableUartTxIRQ(UART);
}

#endif //UART_BUFFERED
#endif //USE_UART
