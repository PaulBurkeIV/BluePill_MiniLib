/*
 * SPI_23S17.c
 *
 *  Created on: 6 May 2020
 *      Author: User
 */
#ifdef USE_SPI23S17
#include <stdint.h>
#include <stdbool.h>
#include "Globals.h"
#include "Platform.h"
#include "SPI_23S17.h"

#define DESELECT true
#define SELECT false

#define MCP23S17RD 0x41
#define MCP23S17WR 0x40

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

//****************************************************************************
//	Read single SPI IO register
//
//	Receives: uint16_t - register
//			  uint16_t - channel to read (0-15) for register
//
//	Returns: uint16_t - register value
//
//****************************************************************************

uint16_t MCP23S17_Read_Reg (SPI_TypeDef *SPI, GPIO_TypeDef* CS_Port, uint16_t CS_Pin, uint16_t reg, uint16_t chan)
{
  uint16_t temp = (chan & 0x0e) + MCP23S17RD;

  //select the IO devices

  SPI_Select_Address (CS_Port, CS_Pin, SELECT);
  SPI_Write (SPI,  temp);
  SPI_Read (SPI);
  temp = (reg << 1) + (chan & 1);
  SPI_Write (SPI,  temp);
  SPI_Read (SPI);
  temp = 0;
  SPI_Write (SPI,  temp);
  temp = SPI_Read (SPI);
  SPI_Select_Address (CS_Port, CS_Pin, DESELECT);
  return temp;
}

//****************************************************************************
//	Write single SPI IO register
//
//	Receives: uint16_t - register
//			  uint16_t - channel to read (0-15) for register
//			  uint16_t - Value to write
//
//	Returns:  Nothing
//
//****************************************************************************

static void MCP23S17_Write_Reg (SPI_TypeDef *SPI, GPIO_TypeDef* CS_Port, uint16_t CS_Pin, uint16_t reg, uint16_t chan, uint16_t val)
{
  uint16_t temp = (chan & 0x0e) + MCP23S17WR;

  //select the IO devices

  SPI_Select_Address (CS_Port, CS_Pin, SELECT);
  SPI_Write (SPI,temp);
  SPI_Read (SPI);
  temp = (reg << 1) + (chan & 1);
  SPI_Write (SPI, temp);
  SPI_Read (SPI);
  temp = val;
  SPI_Write (SPI, temp);
  SPI_Read (SPI);
  SPI_Select_Address (CS_Port, CS_Pin, DESELECT);
}

//****************************************************************************
//	Initialise IO SPI
//
//	Receives: Nothing
//
//	Returns:  Nothing
//
//****************************************************************************

void MCP23S17_Init (SPI_TypeDef *SPI, GPIO_TypeDef* CS_Port, uint16_t CS_Pin)
{
InitIOBit(CS_Port, CS_Pin, OUTPUT_PP);
SPI_Select_Address (CS_Port, CS_Pin, DESELECT);
// Up till this all devices share address 0
MCP23S17_Write_Reg ( SPI, CS_Port, CS_Pin, SPI_OFS_CON, 0, SPI_CONF_HAEN);
}

//****************************************************************************
//	Read SPI IO direction register
//
//	Receives: uint16_t - register
//			  uint16_t - channel to read (0-15) for register
//
//	Returns: uint16_t - register value
//
//****************************************************************************

uint16_t MCP23S17_Read (SPI_TypeDef *SPI, GPIO_TypeDef* CS_Port, uint16_t CS_Pin, uint16_t chan)
{
  uint16_t v;
  v = MCP23S17_Read_Reg (SPI, CS_Port, CS_Pin, SPI_OFS_IO, chan);
  return v;
}

//****************************************************************************
//	Write SPI output register
//
//	Receives: uint16_t - channel to read (0-15) for register
//			  uint16_t - Value to write
//
//	Returns:  Nothing
//
//****************************************************************************

void MCP23S17_Write (SPI_TypeDef *SPI, GPIO_TypeDef* CS_Port, uint16_t CS_Pin, uint16_t chan, uint16_t val)
{
  MCP23S17_Write_Reg (SPI, CS_Port, CS_Pin, SPI_OFS_IO, chan, val);
}

//****************************************************************************
//	Write SPI direction register
//
//	Receives: uint16_t - channel to write (0-15) for register
//			  uint16_t - mask (0 for output, 1 for input) to write
//
//	Returns:  Nothing
//
//****************************************************************************

void MCP23S17DIR_Write (SPI_TypeDef *SPI, GPIO_TypeDef* CS_Port, uint16_t CS_Pin, uint16_t chan, uint16_t mask)
{
  MCP23S17_Write_Reg (SPI, CS_Port, CS_Pin, SPI_OFS_DIR, chan, mask);
}

//****************************************************************************
//	Read SPI direction register
//
//	Receives: uint16_t - channel to read (0-15) for register
//
//	Returns: uint16_t - register value
//
//****************************************************************************

uint16_t MCP23S17DIR_Read (SPI_TypeDef *SPI, GPIO_TypeDef* CS_Port, uint16_t CS_Pin, uint16_t chan)
{
  return MCP23S17_Read_Reg (SPI, CS_Port, CS_Pin, SPI_OFS_DIR, chan);
}

//****************************************************************************
//	Write SPI polarity register
//
//	Receives: uint16_t - channel to write (0-15) for register
//			  uint16_t - mask (0 for output, 1 for input) to write
//
//	Returns:  Nothing
//
//****************************************************************************

void MCP23S17POL_Write (SPI_TypeDef *SPI, GPIO_TypeDef* CS_Port, uint16_t CS_Pin, uint16_t chan, uint16_t mask)
{
  MCP23S17_Write_Reg (SPI, CS_Port, CS_Pin, SPI_OFS_POL, chan, mask);
}

//****************************************************************************
//	Read SPI polarity register
//
//	Receives: uint16_t - channel to read (0-15) for register
//
//	Returns: uint16_t - register value
//
//****************************************************************************

uint16_t MCP23S17POL_Read (SPI_TypeDef *SPI, GPIO_TypeDef* CS_Port, uint16_t CS_Pin, uint16_t chan)
{
  return MCP23S17_Read_Reg (SPI, CS_Port, CS_Pin, SPI_OFS_POL, chan);
}

//****************************************************************************
//	Write SPI pullup register
//
//	Receives: uint16_t - channel to write (0-15) for register
//			  uint16_t - mask (0 for output, 1 for input) to write
//
//	Returns:  Nothing
//
//****************************************************************************

void MCP23S17PU_Write (SPI_TypeDef *SPI, GPIO_TypeDef* CS_Port, uint16_t CS_Pin, uint16_t chan, uint16_t mask)
{
  MCP23S17_Write_Reg (SPI, CS_Port, CS_Pin, SPI_OFS_PU, chan, mask);
}

//****************************************************************************
//	Read SPI pullup register
//
//	Receives: uint16_t - channel to read (0-15) for register
//
//	Returns: uint16_t - register value
//
//****************************************************************************

uint16_t MCP23S17PU_Read (SPI_TypeDef *SPI, GPIO_TypeDef* CS_Port, uint16_t CS_Pin, uint16_t chan)
{
  return MCP23S17_Read_Reg (SPI, CS_Port, CS_Pin, SPI_OFS_PU, chan);
}
#endif //USESPI_23S17
