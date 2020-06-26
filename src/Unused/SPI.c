
/*
 * SPI_25LCxx.c
 *
 *  Created on: 6 May 2020
 *      Author: User
 */
#include <stdint.h>
#include <stdbool.h>
#include "Globals.h"
#include "Platform.h"
#include "SPI_25LCxx.h"

#define SPI_IORD 0x41
#define SPI_IOWR 0x40

#define SPI_OFS_DIR  0	// IODIR – I/O DIRECTION REGISTER
#define SPI_OFS_POL  1	// IPOL – INPUT POLARITY PORT REGISTER
#define SPI_OFS_IE   2	// GPINTEN – INTERRUPT-ON-CHANGE PINS
#define SPI_OFS_DEF  3	// DEFVAL – DEFAULT VALUE REGISTER
#define SPI_OFS_ICR  4	// INTCON – INTERRUPT-ON-CHANGE CONTROL REGISTER
#define SPI_OFS_CON  5	// IOCON – I/O EXPANDER CONFIGURATION REGISTER
#define SPI_OFS_PU   6	// GPPU – GPIO PULL-UP RESISTOR REGISTER
#define SPI_OFS_IF   7	// INTF – INTERRUPT FLAG REGISTER
#define SPI_OFS_ICAP 8	// INTCAP – INTERRUPT CAPTURED VALUE FOR PORT REGISTER
#define SPI_OFS_IO   9	// GPIO – GENERAL PURPOSE I/O PORT REGISTER
#define SPI_OFS_LAT  10 // OUTPUT LATCH REGISTER


// Configuration register bits

#define SPI_CONF_INTPOL 2
#define SPI_CONF_ODR	4
#define SPI_CONF_HAEN 	8
#define SPI_CONF_DISSLW 0x10
#define SPI_CONF_SEQUOP 0x20
#define SPI_CONF_MIRROR 0x40
#define SPI_CONF_BANK 	0x80

static void SPI_IO_Write_Reg(uint16_t reg, uint16_t chan, uint16_t val);
static uint16_t SPI_IO_Read_Reg(uint16_t reg, uint16_t chan);
static void SPI_Select_Address( uint8_t addr);
static void SPI_Mem_Enable(bool enable);

//****************************************************************************
// MCU specific routines
//

void SPI_Master_Init(void)
{
//MOSI & MISO on different pins as normal
//system clock 168MHz - div32=5.25MHz, MCP23S17 10MHz max
//NSS output from SPI (PA4) not used
//clock is idle high
//with clock high, second edge is rising
//
//Normal to have SPI MSB transmitted first


  SPI1->CR1 = SPI_Direction_2Lines_FullDuplex | SPI_Mode_Master |
                  SPI_DataSize_8b | SPI_CPOL_High |
                  SPI_CPHA_2Edge | SPI_NSS_Soft |
                  SPI_BaudRatePrescaler_32 | SPI_FirstBit_MSB;
  SPI1->I2SCFGR &= ~SPI_I2SCFGR_I2SMOD;
  SPI1->CR1 |= SPI_CR1_SPE;
}

bool SPI_Tx_Busy(void)
{
bool res = false;
if( !(SPI1->SR & SPI_I2S_FLAG_TXE))
 {
 res = true;
 }
return res;
}

bool SPI_Rx_Rdy(void)
{
bool res = true;
if( !(SPI1->SR & SPI_I2S_FLAG_RXNE))
 {
 res = false;
 }
return res;
}

uint16_t SPI_Read(void)
{
return SPI1->DR;
}

void SPI_Write(uint16_t v)
{
SPI1->DR = v;
}

//****************************************************************************
//	Select SPI address (drive chip select)
//
//	Receives: uint8_t - Device to select
//
//	Returns: nothing
//
//****************************************************************************


#define CS_OFF true
#define CS_ON false

static void SPI_Select_Address(SPI_SELS addr)
{
SetOutputState(SMEMCS, CS_OFF);
SetOutputState(CSIO, CS_OFF);
SetOutputState(CS_MCU2, CS_OFF);

switch( addr)
 {
 case SPI_SEL_IO:	  SetOutputState(CSIO, CS_ON);
 					  break;
 case SPI_SEL_MEM:	  SetOutputState(SMEMCS, CS_ON);
 					  break;
 case SPI_SEL_MCU2:	  SetOutputState(CS_MCU2, CS_ON);
 					  break;
 default:
					  break;

 }
}

//****************************************************************************
//	Read single SPI IO register
//
//	Receives: uint16_t - register
//			  uint16_t - channel to read (0-9) for register
//
//	Returns: uint16_t - register value
//
//****************************************************************************

uint16_t SPI_IO_Read_Reg(uint16_t reg, uint16_t chan)
{
	uint16_t temp = (chan & 0x0e) + SPI_IORD;

	//select the IO devices

	SPI_Select_Address(SPI_SEL_IO);

	while(SPI_Tx_Busy())
	 {}
	SPI_Write(temp);
	while (!SPI_Rx_Rdy())
     	 {}
    	SPI_Read();
	temp = (reg<<1)+(chan&1);
	while(SPI_Tx_Busy())
	 {}
	SPI_Write(temp);
	while (!SPI_Rx_Rdy())
     	 {}
	SPI_Read();
	temp = 0;
	while(SPI_Tx_Busy())
	 {}
	SPI_Write(temp);
	while (!SPI_Rx_Rdy())
     	 {}
	temp = SPI_Read();
	SPI_Select_Address(SPI_SEL_OFF);
	return temp;
}

//****************************************************************************
//	Write single SPI IO register
//
//	Receives: uint16_t - register
//			  uint16_t - channel to read (0-9) for register
//			  uint16_t - Value to write
//
//	Returns:  Nothing
//
//****************************************************************************

static void SPI_IO_Write_Reg(uint16_t reg, uint16_t chan, uint16_t val)
{
	uint16_t temp = (chan & 0x0e) + SPI_IOWR;

	//select the IO devices

	SPI_Select_Address(SPI_SEL_IO);

	while(SPI_Tx_Busy())
	 {}
	SPI_Write(temp);
	while (!SPI_Rx_Rdy())
	 {}
	SPI_Read();
	temp = (reg<<1)+(chan&1);
	while(SPI_Tx_Busy())
	 {}
	SPI_Write(temp);
	while (!SPI_Rx_Rdy())
	 {}
	SPI_Read();
	temp = val;
	while(SPI_Tx_Busy())
	 {}
	SPI_Write(temp);
	while (!SPI_Rx_Rdy())
	 {}
	SPI_Read();
	SPI_Select_Address(SPI_SEL_OFF);
}

//****************************************************************************
//	Initialise IO SPI
//
//	Receives: Nothing
//
//	Returns:  Nothing
//
//****************************************************************************


void SPI_IO_Init(void)
{
SPI_Select_Address( SPI_SEL_OFF);

// Up till this all devices share address 0
SPI_IO_Write_Reg( SPI_OFS_CON, 0, SPI_CONF_HAEN);
}

//****************************************************************************
//	Read SPI IO direction register
//
//	Receives: uint16_t - register
//			  uint16_t - channel to read (0-9) for register
//
//	Returns: uint16_t - register value
//
//****************************************************************************

uint16_t SPI_IO_Read(uint16_t chan)
{
uint16_t v;
v = SPI_IO_Read_Reg(SPI_OFS_IO, chan);
return v;
}

//****************************************************************************
//	Write SPI output register
//
//	Receives: uint16_t - channel to read (0-9) for register
//			  uint16_t - Value to write
//
//	Returns:  Nothing
//
//****************************************************************************

void SPI_IO_Write( uint16_t chan, uint16_t val)
{
SPI_IO_Write_Reg(SPI_OFS_IO, chan,  val);
}

//****************************************************************************
//	Write SPI direction register
//
//	Receives: uint16_t - channel to write (0-9) for register
//			  uint16_t - mask (0 for output, 1 for input) to write
//
//	Returns:  Nothing
//
//****************************************************************************

void SPI_IODIR_Write( uint16_t chan, uint16_t mask)
{
SPI_IO_Write_Reg(SPI_OFS_DIR, chan,  mask);
}

//****************************************************************************
//	Read SPI direction register
//
//	Receives: uint16_t - channel to read (0-9) for register
//
//	Returns: uint16_t - register value
//
//****************************************************************************

uint16_t SPI_IODIR_Read(uint16_t chan)
{
return SPI_IO_Read_Reg(SPI_OFS_DIR, chan);
}

//****************************************************************************
//	SPI memory
//****************************************************************************

#define SPI_MEM_SZ 65536


#define MEM_EN 6
#define MEM_STS 5
#define MEM_DIS 4
#define MEM_WR 2
#define MEM_RD 3

//****************************************************************************
//	Write SPI memory data bytes
//
//	Receives: uint16_t address - start address to write at
//			  uint8_t *data - points to data to write
//			  uint16_t nbytes - number of bytes to write
//
//	Returns:  Nothing
//
//****************************************************************************

static void SPI_Mem_Write(uint8_t *data, uint32_t nbytes)
{
uint32_t i;

for(i=0; i<nbytes; i++)
 {
 while(SPI_Tx_Busy())
  {}
 SPI_Write(*data);
 while (!SPI_Rx_Rdy())
  {}
 SPI_Read();
 data++;
 }
}

static void SPI_Mem_Read(uint8_t *data, uint32_t nbytes)
{
uint32_t i;

for(i=0; i<nbytes; i++)
 {
 while(SPI_Tx_Busy())
  {}
 SPI_Write(0);
 while (!SPI_Rx_Rdy())
  {}
 *data = SPI_Read();
 data++;
 }
}

static void SPI_Mem_Enable(bool enable)
{
uint8_t En = enable ? MEM_EN : MEM_DIS;
SPI_Select_Address(SPI_SEL_MEM);
SPI_Mem_Write(&En, 1);
SPI_Select_Address(SPI_SEL_OFF);
}

static bool SPI_Mem_Rdy(void)
{
uint8_t cmd = MEM_STS;
uint8_t sts;
SPI_Select_Address(SPI_SEL_MEM);
SPI_Mem_Write(&cmd, 1);
SPI_Mem_Read(&sts, 1);
SPI_Select_Address(SPI_SEL_OFF);
if( sts & 1)
 return false;
else
 return true;
}


// Write bytes within a page
// End address must be on same page as start address

static void SPI_Mem_Write_Data_Inpage( uint32_t addr, uint8_t *data, uint32_t nbytes)
{
uint8_t temp[3];
uint32_t end = addr+nbytes-1;
uint32_t blockstart = addr % 128;
temp[0] = MEM_WR;
temp[1] = addr>>8;
temp[2] = addr;

if( nbytes > 128)
 nbytes = 128;
if((blockstart + nbytes) > 128)
 nbytes = 128-blockstart;
// Wait for ready
while(!SPI_Mem_Rdy())
 ;
// Send WR command and address

SPI_Mem_Enable(true);
SPI_Select_Address(SPI_SEL_MEM);
SPI_Mem_Write(temp, 3);
SPI_Mem_Write(data, nbytes);
SPI_Select_Address(SPI_SEL_OFF);
SPI_Mem_Enable(false);

// Send data

SPI_Mem_Enable(true);
SPI_Select_Address(SPI_SEL_MEM);
SPI_Mem_Write(temp, 3);
SPI_Mem_Write(data, nbytes);
SPI_Select_Address(SPI_SEL_OFF);
SPI_Mem_Enable(false);
}

//**************************************************************************
//	Public SPI memory read and write functions
//
//	Receives:	uint16_t start address
//				uint8_t* data to read write
// 				uint16_t nbytes to read/write
//
//	Returns:	Nothing
//
//****************************************************************************
// Public write function

void SPI_Mem_Write_Data( uint32_t addr, uint8_t *data, uint32_t nbytes)
{
uint32_t end = addr+nbytes-1;
uint32_t blockstart;
uint32_t npageb;

if( (end + addr) > SPI_MEM_SZ-1 ) // Don't allow wrap
 end = SPI_MEM_SZ-1;
while(nbytes)
 {
 blockstart = addr & 0x007f;
 npageb = 128-blockstart;
 if( nbytes > npageb)
  nbytes -= npageb;
 else
  {
  npageb = nbytes;
  nbytes = 0;
  }
 SPI_Mem_Write_Data_Inpage( addr, data, npageb);
 data += npageb;
 addr += npageb;
 }
}

// Public read function

void SPI_Mem_Read_Data( uint32_t addr, uint8_t *data, uint32_t nbytes)
{
uint8_t temp[3];

temp[0] = MEM_RD;
temp[1] = addr>>8;
temp[2] = addr;

while(!SPI_Mem_Rdy())
 ;

SPI_Select_Address(SPI_SEL_MEM);

// Send RD command and address
SPI_Mem_Write(temp, 3);

// Read back data
SPI_Mem_Read(data, nbytes);
SPI_Select_Address(SPI_SEL_OFF);
}



//**********************************************************************//
// Write a block of data from txdata to 2nd processor
// Write data received to rxdata and return number of bytes processed

uint32_t SPI_MCU2_ReadWrite(uint8_t *txdata, uint8_t *rxdata, uint32_t nbytes)
{
uint32_t i;
uint16_t rx,tx;
uint32_t txcnt = nbytes;
SPI_Select_Address(SPI_SEL_MCU2);
while( GetInputState(HS_MCU2) == true)
 {}

while(txcnt)
 {
 SoftDelay(30);
 tx = *txdata;
 while(SPI_Tx_Busy())
  {}
 SPI_Write(tx);
 while( !SPI_Rx_Rdy())
  {}
 rx = SPI_Read();
 txcnt--;
 if( !txcnt)
  {
  SPI_Select_Address(SPI_SEL_OFF);
  }
 *rxdata = rx;
 txdata++;
 rxdata++;
 }
//while(((SPI_Tx_Busy()) && (SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE) != RESET)) ) // || (!SPI_Rx_Rdy()))
//  {}
SPI_Select_Address(SPI_SEL_OFF);
while( !GetInputState(HS_MCU2))
 {}
}
