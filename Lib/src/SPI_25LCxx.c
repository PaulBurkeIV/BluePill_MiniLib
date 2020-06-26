/*
 * SPI_25LCxx.c
 *
 *  Created on: 6 May 2020
 *      Author: User
 */
#ifdef USE_SPI25LCxx

#include <stdint.h>
#include <stdbool.h>
#include "Globals.h"
#include "Platform.h"
#include "SPI_25LCxx.h"

#define DESELECT true
#define SELECT false

#define READ 0x03 	//Read data from memory array beginning at selected address
#define WRITE 0x02	//Write data to memory array beginning at selected address
#define WRDI 0x04 	//Reset the write enable latch (disable write operations)
#define WREN 0x06	//Set the write enable latch (enable write operations)
#define RDSR 0x05	//Read STATUS register
#define WRSR 0x01	//Write STATUS register

#define WPEN Bit(7)
#define BP1 Bit(3)
#define BP0 Bit(2)
#define WEL Bit(1)
#define WIP Bit(0)

void SPI_25LCxx_Init (SPI_TypeDef *SPI, GPIO_TypeDef* CS_Port, uint16_t CS_Pin)
{
InitIOBit(CS_Port, CS_Pin, OUTPUT_PP);
SPI_Select_Address (CS_Port, CS_Pin, DESELECT);
}

uint8_t Read25LCxxStatus(SPI_TypeDef *SPI, GPIO_TypeDef* CS_Port, uint16_t CS_Pin)
{
uint8_t st;
SPI_Select_Address(CS_Port, CS_Pin, false);
SPI_Write(SPI, RDSR);
SPI_Read(SPI);
SPI_Write(SPI, 0);
st = SPI_Read(SPI);
SPI_Select_Address(CS_Port, CS_Pin, true);
return st;
}

void Write25LCxxEnable(SPI_TypeDef *SPI, GPIO_TypeDef* CS_Port, uint16_t CS_Pin)
{
while(Read25LCxxStatus(SPI,CS_Port, CS_Pin)& WIP)
 {}
SPI_Select_Address(CS_Port, CS_Pin, false);
SPI_Write(SPI, WREN);
SPI_Read(SPI);
SPI_Select_Address(CS_Port, CS_Pin, true);
SPI_Select_Address(CS_Port, CS_Pin, false);
while(!(Read25LCxxStatus(SPI,CS_Port, CS_Pin)& WEL))
 {}
SPI_Select_Address(CS_Port, CS_Pin, true);
}

void Write25LCxxProtect(SPI_TypeDef *SPI, GPIO_TypeDef* CS_Port, uint16_t CS_Pin, uint8_t Enables)
{
while(Read25LCxxStatus(SPI,CS_Port, CS_Pin)& WIP)
 {}
SPI_Select_Address(CS_Port, CS_Pin, false);
SPI_Write(SPI, WRSR);
SPI_Read(SPI);
SPI_Write(SPI, Enables);
SPI_Read(SPI);
SPI_Select_Address(CS_Port, CS_Pin, true);
}

uint16_t Read25LCxx(SPI_TypeDef *SPI, GPIO_TypeDef* CS_Port, uint16_t CS_Pin, uint8_t *dst, uint16_t StartAdr, uint16_t nbytes)
{
uint16_t i;
uint8_t ah = StartAdr>>8;
uint8_t al = StartAdr;

SPI_Select_Address(CS_Port, CS_Pin, false);
SPI_Write(SPI, READ);
SPI_Read(SPI);
SPI_Write(SPI, ah);
SPI_Read(SPI);
SPI_Write(SPI, al);
SPI_Read(SPI);
for( i=0; i<nbytes; i++)
 {
 SPI_Write(SPI, 0);
 *dst = SPI_Read(SPI);
 dst++;
 }
SPI_Select_Address(CS_Port, CS_Pin, true);
return i;
}

uint16_t Write25LCxxInPage(SPI_TypeDef *SPI, GPIO_TypeDef* CS_Port, uint16_t CS_Pin, uint8_t *src, uint16_t StartAdr, uint16_t nbytes)
{
uint16_t i;
uint8_t ah = StartAdr>>8;
uint8_t al = StartAdr;

while(Read25LCxxStatus(SPI,CS_Port, CS_Pin)& WIP)
 {}
Write25LCxxEnable(SPI,CS_Port, CS_Pin);
SPI_Select_Address(CS_Port, CS_Pin, false);
SPI_Write(SPI, WRITE);
SPI_Read(SPI);
SPI_Write(SPI, ah);
SPI_Read(SPI);
SPI_Write(SPI, al);
SPI_Read(SPI);
for( i=0; i<nbytes; i++)
 {
 SPI_Write(SPI, *src);
 SPI_Read(SPI);
 src++;
 }
SPI_Select_Address(CS_Port, CS_Pin, true);
return i;
}

uint16_t Write25LCxxByte(SPI_TypeDef *SPI, GPIO_TypeDef* CS_Port, uint16_t CS_Pin, uint8_t v, uint16_t Adr)
{
uint8_t ah = Adr >> 8;
uint8_t al = Adr;

while(Read25LCxxStatus(SPI,CS_Port, CS_Pin)& WIP)
 {}
Write25LCxxEnable(SPI,CS_Port, CS_Pin);
SPI_Select_Address(CS_Port, CS_Pin, false);
SPI_Write(SPI, WRITE);
SPI_Read(SPI);
SPI_Write(SPI, ah);
SPI_Read(SPI);
SPI_Write(SPI, al);
SPI_Read(SPI);
SPI_Write(SPI, v);
SPI_Read(SPI);
SPI_Select_Address(CS_Port, CS_Pin, true);
return Adr;
}

void Write25LCxx(SPI_TypeDef *SPI, GPIO_TypeDef* CS_Port, uint16_t CS_Pin, uint8_t *src, uint16_t StartAdr, uint16_t nbytes)
{
uint16_t EndAdr = StartAdr + nbytes;

while(1)
 {
 uint16_t bytestowrite;
 uint16_t LeftInPage = SPIMEM_PAGE_SIZE-(StartAdr % SPIMEM_PAGE_SIZE);

 if( nbytes < LeftInPage)
  {
  bytestowrite = nbytes;
  }
 else if( LeftInPage < SPIMEM_PAGE_SIZE)
  {
  bytestowrite =  LeftInPage;
  }
 else
  {
  bytestowrite = SPIMEM_PAGE_SIZE;
  }
 Write25LCxxInPage(SPI, CS_Port, CS_Pin, src, StartAdr, bytestowrite);
 StartAdr += bytestowrite;
 nbytes -= bytestowrite;
 if(!nbytes)
  break;

 }
}
#endif //USE_SPI25LCxx
