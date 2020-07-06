/**************************************************************************
 * This example uses an MCP23S17 to interface to a 4x3 keypad
 * using SPI2 and the device at address 0
 * The IO lines are all on GPIOA so there is a spare line
 * you could use for an extra key or an LED output or whatever
 **************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <ctype.h>

#include "Platform.h"
#include "Globals.h"
#include "sscanf.h"
#include "Print.h"
#include "SPI_23S17.h"

/************ Some defs for this demo *********************/

uint32_t BlinkTime = 499;

void InitKeyPad(void);
uint8_t DoKeyPad(void);

/************ Main ****************************************/
//
// The Raging main()

int main(void)
{
uint32_t t = 0;

SystemInit();		// This sets up the 72MHz clock using the 8MHz crystal
PlatformInit();		// Any special platform initialisation
SysTick_Config(SYSTEM_CLOCK/1000);	// This starts a 1ms timebase interrupt
InitGPIOClocks();	// This makes sure all the GPIO ports are clocked

//Initialise the port bits

InitIOBit(GREEN_LED_INIT);

// Initialise UART and its IO pins
// 115200 Baud, 8N1

InitIOBit(SERIAL_TX_INIT);	// Initialise the UART IO lines
InitIOBit(SERIAL_RX_INIT);
InitUART(SERIAL, 115200, DATA_8N1);	// Initialise the UART

// Initialise SPI2 on PB13/14/15 and the chip select on PC14

InitIOBit(KBD_SPI_CK_INIT);
InitIOBit(KBD_SPI_MISO_INIT);
InitIOBit(KBD_SPI_MOSI_INIT);
SPI_Init(KBD_SPI);
MCP23S17_Init (KBD_SPI, MCP23S17);

// Initialise the MCP23S17 port for the
InitKeyPad();

printf("Ext IO Keypad Demo started\r\n");

while(1) // Equivalent to Arduino "loop"
 {
 uint8_t kp;
 if( (kp = DoKeyPad()) != 0xff)
  {
  if( kp < 10)
   {
   printf( "Key %d\r\n", kp);
   }
  else
   {
   printf( "Key '%c'\r\n", kp);
   }
  }

 if( (MilliSeconds()-t) > BlinkTime)
  {
  t = MilliSeconds();
  WriteIOBit(GREEN_LED, !ReadIOBit(GREEN_LED));
  }
 }
return 0;
}

#define KBD_ADR 2

#define KPSCANTIME 10
#define KP_O1 Bit(1)
#define KP_O2 Bit(6)
#define KP_O3 Bit(5)
#define KP_O4 Bit(3)
#define KP_I1 Bit(2)
#define KP_I2 Bit(0)
#define KP_I3 Bit(4)

#define KP_IMASK (KP_I1 | KP_I2 | KP_I3)
uint8_t KPOpLine = KP_O1;
uint8_t KPScanTime;


//#define NOKEY 0
//#define KEY 1
//uint8_t State = NOKEY;

enum { NOKEY, KEY} State;

void InitKeyPad(void)
{
MCP23S17DIR_Write (KBD_SPI, MCP23S17, KBD_ADR, ~(KP_O1 | KP_O2 | KP_O3 | KP_O4)); // Set matrix drives as outputs
MCP23S17PU_Write (KBD_SPI, MCP23S17, KBD_ADR, KP_IMASK);	// Matrix inputs pulled up
MCP23S17POL_Write (KBD_SPI, MCP23S17, KBD_ADR, KP_IMASK);	// Matrix inputs inverted
KPScanTime = MilliSeconds();
MCP23S17_Write (KBD_SPI, MCP23S17, KBD_ADR, ~KPOpLine);
}

uint8_t GetKey(uint8_t Line)
{
uint8_t Val = 0xff;
uint8_t key = MCP23S17_Read(KBD_SPI, MCP23S17, KBD_ADR) & KP_IMASK;

 switch( Line )
  {
  case KP_O1:
 		if(  key == KP_I1)
  		 {
  		 Val = 1;
 		 }
 		else if( key == KP_I2)
  		 {
  		 Val = 2;
 		 }
 		else if( key == KP_I3)
  		 {
  		 Val = 3;
 		 }
 		break;
  case KP_O2:
 		if( key == KP_I1)
  		 {
  		 Val = 4;
 		 }
 		else if( key == KP_I2)
  		 {
  		 Val = 5;
 		 }
 		else if( key == KP_I3)
  		 {
  		 Val = 6;
 		 }
 		break;
  case KP_O3:
 		if( key == KP_I1)
  		 {
  		 Val = 7;
 		 }
 		else if( key == KP_I2)
  		 {
  		 Val = 8;
 		 }
 		else if( key == KP_I3)
  		 {
  		 Val = 9;
 		 }
 		break;

  case KP_O4:
 		if( key == KP_I1)
  		 {
  		 Val = '*';
 		 }
 		else if( key == KP_I2)
  		 {
  		 Val = 0;
 		 }
 		else if( key == KP_I3)
  		 {
  		 Val = '#';
 		 }
 		break;
  }

 return Val;
}

static uint8_t NextLine(uint8_t Line)
{
uint8_t val = KP_O1;
switch( Line)
	{
	case KP_O1:
		 val = KP_O2;
		 break;
	case KP_O2:
		 val = KP_O3;
		 break;
	case KP_O3:
		 val = KP_O4;
		 break;
	case KP_O4:
		 val = KP_O1;
		 break;
	}
return val;
}

uint8_t DoKeyPad(void)
{
static uint8_t  LastVal;
uint8_t val = 0xff;

if( (MilliSeconds()-KPScanTime) > KPSCANTIME)
 {
 KPScanTime = MilliSeconds();
 val = GetKey(KPOpLine);

 switch(State)
  {
  case NOKEY:
  		if(  val == 0xff)
  		 {
  		 KPOpLine = NextLine(KPOpLine);
  		 MCP23S17_Write (KBD_SPI, MCP23S17, KBD_ADR, ~KPOpLine);
  		 }
  		else
  		 {
  		 LastVal = val;
  		 State = KEY;
  		 }
  		break;
  case KEY:
  		if( val == 0xff)
  		 {
  		 KPOpLine = NextLine(KPOpLine);
 		 MCP23S17_Write (KBD_SPI, MCP23S17, KBD_ADR, ~KPOpLine);
 		 State = NOKEY;
  		 }
  		else if(val == LastVal)
  		 {
  		 val = 0xff;
  		 }
		break;
  }
 }
return val;
}

