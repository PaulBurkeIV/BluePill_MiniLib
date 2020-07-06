/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Pin interrupt Example
 *
 * This demo sets up PA8 to raise an interrupt on falling edge.
 * The value of the millisecond timer is displayed via UART3.
 *  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
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
InitIOBit(INTERRUPT_PIN_INIT);
IOPinIRQInit(INTERRUPT_PIN, EXTIRQ_FALLING);

// Initialise UART and its IO pins
// 115200 Baud, 8N1

InitIOBit(SERIAL_TX_INIT);
InitIOBit(SERIAL_RX_INIT);
InitUART(SERIAL, 115200, DATA_8N1);


printf("Blue Pill Pin Interrupt demo started\r\n");

while(1) // Equivalent to Arduino "loop"
 {
 if( (MilliSeconds()-t) > BlinkTime)
  {
  t = MilliSeconds();
  WriteIOBit(GREEN_LED, !ReadIOBit(GREEN_LED));
  IOPinIRQEnable(INTERRUPT_PIN);
  }
 }
return 0;
}

void PinInterrupt(uint8_t Line)
{
uint32_t ms = MilliSeconds();
IOPinIRQDisable(INTERRUPT_PIN);
printf( "Pin Interrupt at %dms\r\n", ms);
}

