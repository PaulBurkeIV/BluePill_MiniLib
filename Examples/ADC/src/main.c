/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "Platform.h"
#include "AppIODefs.h"
#include "Globals.h"
#include "sscanf.h"
#include "Print.h"

/************ Some defs for this demo *********************/
//

uint32_t BlinkTime = 499;

void DoSerialIn(void);
void ParseCommand(uint8_t *CmdBuff);

/************ Main ****************************************/
//
// The Raging main()


int main(void)
{
uint32_t i;
uint32_t t = 0;

SystemInit();		// This sets up the 72MHz clock using the 8MHz crystal
PlatformInit();		// Any special platform initialisation
SysTick_Config(SYSTEM_CLOCK/1000);	// This starts a 1ms timebase interrupt
InitGPIOClocks();	// This makes sure all the GPIO ports are clocked

//Initialise the port bits

InitIOBit(GREEN_LED_INIT);

// Initialise UART and its IO pins
// 115200 Baud, 8N1

InitIOBit(SERIAL_TX_INIT);
InitIOBit(SERIAL_RX_INIT);
InitUART(SERIAL, 115200, DATA_8N1);

InitIOBit(POT_IO);
ADCInit(POT_ADC);
StartADC(POT);


printf("Blue Pill ADC example started\r\n");

while(1) // Equivalent to Arduino "loop"
 {
 if( (MilliSeconds()-t) > BlinkTime)
  {
  t = MilliSeconds();
  WriteIOBit(GREEN_LED, !ReadIOBit(GREEN_LED));
  if(ADCRdy(POT_ADC))
   {
   uint16_t v = ReadADC(POT_ADC);
   printf( "ADC %d\r\n", v);
   StartADC(POT);
   }
  }
 }
return 0;
}

