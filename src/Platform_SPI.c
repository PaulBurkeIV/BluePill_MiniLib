#ifdef USE_SPI
#include "Platform.h"
#include "Globals.h"

#define SPI_Mode_Master Bit(2)
#define SPI_CPHA_2Edge Bit(0)
#define SPI_NSS_Soft Bit(9)
#define SPI_BaudRatePrescaler_16 (3<<3)
#define SPI_CPOL_High  Bit(1)

#define SPI_TXE Bit(1)
#define SPI_RXNE Bit(0)
#define SPI_BSY Bit(7)


#define SPI1CLK Bit(12)		//APB2
#define SPI2CLK Bit(14)		//APB1
#define SPI3CLK Bit(15)		//APB1


//****************************************************************************
// MCU specific routines
//

void SPI_Init(SPI_TypeDef *SPI)
{
//MOSI & MISO on different pins as normal
//system clock 72MHz - div16=4.5MHz,
//clock is idle high
//with clock high, second edge is rising
//
//Normal to have SPI MSB transmitted first
switch ((uint32_t)SPI )
 {
 case (uint32_t)SPI1:
 			RCC->APB2RSTR |= SPI1CLK ; // Reset the peripheral
 			RCC->APB2RSTR &= ~SPI1CLK ;
 			RCC->APB2ENR |= SPI1CLK ; // Enable clock
 			break;

  case (uint32_t)SPI2:
 			RCC->APB1RSTR |= SPI2CLK ; // Reset the peripheral
 			RCC->APB1RSTR &= ~SPI2CLK ;
 			RCC->APB1ENR |= SPI2CLK ; // Enable clock
 			break;

//  case (uint32_t)SPI3:	// Not available on BluePill
// 			RCC->APB1RSTR |= SPI2CLK ; // Reset the peripheral
// 			RCC->APB1RSTR &= ~SPI2CLK ;
// 			RCC->APB1ENR |= SPI2CLK ; // Enable clock
// 			break;
  }

//  SPI->CR1 = SPI_Direction_2Lines_FullDuplex | (1<<2)  | SPI_CPOL_High |
//                  SPI_CPHA_2Edge | SPI_NSS_Hard |
//                  SPI_BaudRatePrescaler_32 | SPI_FirstBit_MSB;
  SPI->CR1 = (4<<3) | 7;
  SPI->CR2 = Bit(2);
  SPI->CR1 |= Bit(6);
}

bool SPI_Busy(SPI_TypeDef *SPI)
{
bool res = false;
if( SPI->SR & SPI_BSY)
 {
 res = true;
 }
return res;
}

uint8_t SPI_Read(SPI_TypeDef *SPI)
{
while(!(SPI->SR & SPI_RXNE))
 {}
return SPI->DR;
}

void SPI_Write(SPI_TypeDef *SPI, uint8_t v)
{
while(!(SPI->SR & SPI_TXE))
 {}
SPI->DR = v;
}


//****************************************************************************
//	Select SPI address (drive chip select)
//
//****************************************************************************


void SPI_Select_Address(GPIO_TypeDef* port, uint16_t pin, bool state)
{
WriteIOBit(port, pin, state);
}

#endif //USE_SPI
