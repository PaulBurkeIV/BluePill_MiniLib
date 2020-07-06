/*****************************************************************************************
 *
 * This example sets up a 16 bit high speed (MHz) quadrature encoder input on pins A8 and A9.
 * The encoder reading is sent to serial on UART3.
 * The green LED (Pin C13) is on if the last count direction was down.
 *
 *
 *****************************************************************************************/

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

uint32_t BlinkTime = 499;

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

InitIOBit(SERIAL_TX_INIT);
InitIOBit(SERIAL_RX_INIT);
InitUART(SERIAL, 115200, DATA_8N1);

// Initialise motor PWM_ on TIM1
// 20kHz 1/1000 resolution

InitIOBit(ENCA_IO);
InitIOBit(ENCB_IO);
QuadCounterInit( ENC_TIM, 0, ENC_EDGE_BOTH, 15, false);

printf("Blue Pill motor Quadrature Encoder demo started\r\n");

while(1) // Equivalent to Arduino "loop"
 {
 WriteIOBit(GREEN_LED, !(ENC_TIM->CR1 & Bit(4)));

 if( (MilliSeconds()-t) > BlinkTime)
  {
  t = MilliSeconds();
  printf("%d\r\n", QuadEncoderRead(ENC_TIM));
  }
 }
return 0;
}
