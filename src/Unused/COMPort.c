//#include "stm32f4xx_conf.h"
#include "platform.h"
#include "COMPort.h"



static uint16_t GetRxChrCount(t_COMPort COMPort);

static uint8_t  COM1RxBuff[COM1_BUFF_SIZE];
static uint8_t  COM1TxBuff[COM1_BUFF_SIZE];

static uint8_t  COM2RxBuff[COM2_BUFF_SIZE];
static uint8_t  COM2TxBuff[COM2_BUFF_SIZE];

static uint8_t  COM3RxBuff[COM3_BUFF_SIZE];
static uint8_t  COM3TxBuff[COM3_BUFF_SIZE];

typedef struct
{
	USART_TypeDef* 					UART;
	uint8_t 					IRQ;
	uint32_t 					DMAChannel;

} t_Device;

typedef struct
{
	t_Device*					Device;
	uint16_t					BuffSz;
	uint16_t					RxCntIn;
	uint16_t					RxCntOut;
	uint16_t					TxCntIn;
	uint16_t					TxCntOut;
	uint8_t*					BufferRx;
	uint8_t*					BufferTx;
	bool						Is9Bit;
} t_sCOMPort;

#define DMA_Channel(x) ((x)<<25)

t_Device sUART1 = {USART1, USART1_IRQn, DMA_Channel(5)};
t_Device sUART2 = {USART2, USART2_IRQn, DMA_Channel(6)};
t_Device sUART3 = {USART3, USART3_IRQn, DMA_Channel(3)};

t_sCOMPort sCOM1 = 	{
					&sUART1,
					COM1_BUFF_SIZE,
					0,0,
					0,0,
					COM1RxBuff, COM1TxBuff,
					false
					};

t_sCOMPort sCOM2 = 	{
					&sUART2,
					COM2_BUFF_SIZE,
					0,0,
					0,0,
					COM2RxBuff, COM2TxBuff,
					false
					};


t_sCOMPort sCOM3 = 	{
					&sUART3,
					COM3_BUFF_SIZE,
					0,0,
					0,0,
					COM3RxBuff, COM3TxBuff,
					false
					};

static const t_sCOMPort *COMPorts[] = {&sCOM1, &sCOM2, &sCOM3 }; //, &sCOM4, &sCOM5, &sCOM6};

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

bool chrdy_UART(USART_TypeDef* UART)
{
return (UART->SR & SR_RXNE) ? true : false;
}

uint16_t GetUARTData(USART_TypeDef* UART)
{
return UART->DR;
}

void PutUARTData(USART_TypeDef* UART, uint16_t c)
{
UART->DR = c;
}

static uint16_t GetRxChrCount(t_COMPort nCOMPort)
{
uint16_t nchrs = 0;
t_sCOMPort *COMPort = COMPorts[nCOMPort];
if( !COMPort->Device->IRQ)
 {
 if( chrdy_UART(COMPort->Device->UART))
  {
  nchrs = 1;
  }
 }
else if( !COMPort->Device->DMAStream)
 {
 nchrs = (COMPort->RxCntIn-COMPort->RxCntOut) & (COMPort->BuffSz-1);
 }
else
 {
 COMPort->RxCntIn = (COMPort->BuffSz - DMA_GetCurrDataCounter(COMPort->Device->DMAStream));
 nchrs =  (COMPort->RxCntIn - COMPort->RxCntOut) & (COMPort->BuffSz-1);
 }
return nchrs;
}

/***********************************************************/
/* Interrupt service routines for CM ports 				   */
/***********************************************************/

void UART_IRQHandler(t_COMPort nCOMPort)
{
t_sCOMPort *COMPort = COMPorts[nCOMPort];
USART_TypeDef* 	UART = COMPort->Device->UART;

// Check for transmit interrupt
if(UART->SR & SR_TXE) //USART_GetITStatus(UART, USART_IT_TXE) != RESET)
 {
 if(COMPort->Is9Bit)
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

// Receive interrupt used if IRQ enabled and no DMA
if( UART->SR & SR_RXNE ) //USART_GetITStatus(UART, USART_IT_RXNE) != RESET)
 {
 if(COMPort->Is9Bit)
  {
  if( (COMPort->Device->DMAStream == 0) && (COMPort->Device->IRQ != 0)) // UART doesn't use DMA and is interrupt driven
   {
   uint16_t c = GetUARTData(UART); //USART_ReceiveData(UART);
   COMPort->BufferRx[COMPort->RxCntIn] = c;
   COMPort->BufferRx[COMPort->RxCntIn+1] = c>>8;
   COMPort->RxCntIn = (COMPort->RxCntIn+2) & (COMPort->BuffSz-1);
   }
  }
 else
  {
  COMPort->BufferRx[COMPort->RxCntIn] = GetUARTData(UART);  //USART_ReceiveData(UART);
  COMPort->RxCntIn = (COMPort->RxCntIn+1) & (COMPort->BuffSz-1);
  }
 UART->SR &= ~SR_RXNE; //Clear interrupt
 }
}

void USART1_IRQHandler(void)
{
UART_IRQHandler(COM1);
}

void USART2_IRQHandler(void)
{
UART_IRQHandler(COM2);
}

void USART3_IRQHandler(void)
{
UART_IRQHandler(COM3);
}

void UART4_IRQHandler(void)
{
UART_IRQHandler(COM4);
}

void UART5_IRQHandler(void)
{
UART_IRQHandler(COM5);
}

void UART6_IRQHandler(void)
{
UART_IRQHandler(COM6);
}

/*************************************************************************/
// e.g. COMPort_Init( &COM1, 115200, "8N1");
//

// STM32F4xx pecific hardware init

#define SYSCLK 168000000
#define UART2345CLK (SYSCLK/4)
#define UART16CLK (SYSCLK/2)

#define RCC_APB2Periph_USART1 Bit(14)
//#define RCC_APB2Periph_USART6 Bit(5)
#define RCC_APB1Periph_USART2 Bit(17)
#define RCC_APB1Periph_USART3 Bit(18)
//#define RCC_APB1Periph_UART4 Bit(19)
//#define RCC_APB1Periph_UART5 Bit(20)
//#define RCC_APB1Periph_UART7 Bit(30)
//#define RCC_APB1Periph_UART8 Bit(31)

#define DMA_MemoryInc_Enable Bit(10)
#define DMA_Mode_Circular Bit(8)
#define USART_StopBits_1 0
#define PAR_NO 0
#define PAR_E 1
#define PAR_O 2

// This initialises UARTs in 16x div mode, standard UART mode only

void UARTInit(t_sCOMPort *COMPort, uint32_t BaudRate, uint16_t USART_Parity, uint16_t StopBits)
{
t_Device *Device = COMPort->Device;
USART_TypeDef* UART = Device->UART;
uint32_t clk;
uint32_t WordSize = 1;
uint32_t IRQNo;
DMA_Channel_TypeDef DMA_Channel;


switch( (uint32_t)UART)
 {
 case (uint32_t)USART1:
 		RCC->APB2ENR |= RCC_APB2Periph_USART1;
		clk = UART16CLK;
		DMA_Channel = DMA1_Channel5;
		break;

 case (uint32_t)USART2:
 		RCC->APB1ENR |= RCC_APB1Periph_USART2;
		clk = UART2345CLK;
		DMA_Channel = DMA1_Channel6;
		break;

 case (uint32_t)USART3:
 		RCC->APB1ENR |= RCC_APB1Periph_USART3;
		clk = UART2345CLK;
		DMA_Channel = DMA1_Channel3;
		break;

 default:	// Nothing to do if no UART!
  		return;
 }

UART->BRR = clk/BaudRate;
if( COMPort->Is9Bit )
 {
 UART->CR1 |= CR1_M;
 WordSize = 2;
 }
else
 {
 UART->CR1 &= ~CR1_M;
 }

switch( USART_Parity)
 {
 default:
 case PAR_NO:
 		UART->CR1 &= ~CR1_PARITY;
 		break;
 case PAR_E:
  		UART->CR1 &= ~CR1_PARITY;
		UART->CR1 |= CR1_PAR_E;
 		break;
 case PAR_O:
		UART->CR1 |= CR1_PAR_O;
 		break;
 }

if( StopBits == 2)
 {
 UART->CR2 |= CR2_STOP2;
 }
else
 {
 UART->CR2 &= ~CR2_STOP2;
 }
UART->CR1 |= CR1_TXEIE | CR1_TE | CR1_RE;

// 9 bit word can't be DMA'd until we change DMA setup
if( COMPort->Is9Bit && (USART_Parity != PAR_NO))
 {
 Device->DMAStream = 0;
 Device->DMAChannel = 0;
 }


NVIC_InitTypeDef NVIC_InitStructure = {0};

//DMA_InitTypeDef DMA_InitStructure = {0};

if( Device->IRQ) // UART uses IRQ
 {
 NVIC->ISER[0] |= COMPort->Device.IRQ;
 }

if( Device->DMAChannel) // UART uses DMA
 {
Device->DMAStream->CR = 0;
Device->DMAStream->FCR = 0;
//Device->DMAStream->CR = DMA_MemoryInc_Enable | DMA_Mode_Circular | ((WordSize-1)<<11) | ((WordSize-1)<<13) |  Device->DMAChannel;
Device->DMAStream->CR = DMA_MemoryInc_Enable | DMA_Mode_Circular |  Device->DMAChannel;
Device->DMAStream->PAR = (uint32_t)&COMPort->Device->UART->DR;
Device->DMAStream->M0AR = (uint32_t)COMPort->BufferRx;
Device->DMAStream->NDTR = COMPort->BuffSz;
Device->DMAStream->FCR =
UART->CR3 |= CR3_DMAR;
Device->DMAStream->CR |= 1;
 // Enable USART_Rx DMA Receive request
 UART->CR3 |= CR3_DMAR;

 }

else if( Device->IRQ)
 {
 EnableUartRx(UART);
 }
if( Device->IRQ)
 {
 EnableUartTx(UART);
 }
// UART device enable
UART->CR1 |= CR1_UE;

}

/***********************************************************/
/* Portable code				   */
/***********************************************************/
// Portable init

void COMPort_Init( t_COMPort nCOMPort, uint32_t BaudRate, char *setup)
{
char *ps;
uint16_t USART_Parity = PAR_NO;
uint16_t StopBits = 1;

t_sCOMPort *COMPort = COMPorts[nCOMPort];

// Get word length, parity, stop bits
ps = setup;
while( *ps)
 {
 switch(*ps)
  {
  case '8':		ps++;
  			break;
  case '9':		COMPort->Is9Bit = true;
  			ps++;
  			break;
  case 'n':
  case 'N':
  			USART_Parity = PAR_NO;
  			ps++;
  			break;
  case 'e':
  case 'E':
  			USART_Parity = PAR_E;
  			ps++;
  			break;
  case 'o':
  case 'O':
  			USART_Parity =  PAR_O;
  			ps++;
  			break;
  case '1':
			StopBits = 1;
			ps++;
			break;
  case '2':
			StopBits = 2;
			ps++;
			break;
  default:
  			ps++;
  			break;
  }
 }

// Initialise the buffer pointers

COMPort->RxCntIn = 0;
COMPort->RxCntOut = 0;
COMPort->TxCntIn = 0;
COMPort->TxCntOut = 0;

UARTInit(COMPort, BaudRate, USART_Parity, StopBits);
}


int getc(t_COMPort nCOMPort)
{
uint16_t c;
t_sCOMPort *COMPort = COMPorts[nCOMPort];
if( !COMPort->Device->IRQ)
 {
 c = GetUARTData(COMPort->Device->UART);
 }
else
 {
 c = COMPort->BufferRx[COMPort->RxCntOut];
 COMPort->RxCntOut = (COMPort->RxCntOut+1)&(COMPort->BuffSz-1);
 }
if(COMPort->Is9Bit)
 {
 c |= COMPort->BufferRx[COMPort->RxCntOut] << 8;
 COMPort->RxCntOut = (COMPort->RxCntOut+1)&(COMPort->BuffSz-1);
 }
return c;
}

uint16_t chrdy(t_COMPort nCOMPort)
{
return GetRxChrCount(nCOMPort);
}

void flush(t_COMPort nCOMPort)
{
t_sCOMPort *COMPort = COMPorts[nCOMPort];
COMPort->RxCntOut = COMPort->RxCntIn;
}

int putc(t_COMPort nCOMPort, uint16_t c)
{
t_sCOMPort *COMPort = COMPorts[nCOMPort];
if( !COMPort->Device->IRQ ) // Not interrupt driven Tx
 {
 uint32_t t = MilliSeconds;
 while(!chrdy_UART(COMPort->Device->UART))
  {
  if( (MilliSeconds-t) > 5)
   break;
  }
 PutUARTData(COMPort->Device->UART, c);
 }
else
 {
 NVIC_DisableIRQ( COMPort->Device->IRQ);
 COMPort->BufferTx[COMPort->TxCntIn] = c;
 COMPort->TxCntIn = (COMPort->TxCntIn+1)&(COMPort->BuffSz-1);
 if(COMPort->Is9Bit)
  {
  COMPort->BufferTx[COMPort->TxCntIn] = c >> 8;
  COMPort->TxCntIn = (COMPort->TxCntIn+1)&(COMPort->BuffSz-1);
  }
 EnableUartTx(COMPort->Device->UART);

 NVIC_EnableIRQ( COMPort->Device->IRQ);
 }
return c;
}

void puts(t_COMPort nCOMPort, uint8_t *pData)
{
while(*pData)
 {
 putc(nCOMPort, *pData);
 pData++;
 }
}

void putns(t_COMPort nCOMPort, uint8_t *pData, uint32_t Len)
{
while(*pData && Len)
 {
 putc(nCOMPort, *pData);
 pData++;
 Len--;
 }
}


