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
uint32_t i;
uint32_t t = 0;

SystemInit();		// This sets up the 72MHz clock using the 8MHz crystal
PlatformInit();		// Any special platform initialisation
SysTick_Config(SYSTEM_CLOCK/1000);	// This starts a 1ms timebase interrupt
InitGPIOClocks();	// This makes sure all the GPIO ports are clocked

// Initialise UART and its IO pins
// 115200 Baud, 8N1

InitIOBit(SERIAL_TX_INIT);
InitIOBit(SERIAL_RX_INIT);
InitUART(SERIAL, 115200, DATA_8N1);

//Initialise the port bits

InitIOBit(GREEN_LED_INIT);

//Initialise the CAN

InitIOBit(CAN_RX_BIT);
InitIOBit(CAN_TX_BIT);
InitCAN();

printf("Blue Pill CAN demo startedWriteIOVect");

while(1) // Equivalent to Arduino "loop"
 {
 t_CANRXMsg *CANRxMsg = CAN_Rx();
 WriteIOBit(GREEN_LED, !ReadIOBit(GREEN_LED));
 if( CANRxMsg)
  {
  printf( "CAN %x{%s %c,%d): ", CANRxMsg->ID, CANRxMsg->Remote ? "Remote" : "", CANRxMsg->Extended ? 'E' : 'N', CANRxMsg->Len);
  for( uint8_t i=0; i<CANRxMsg->Len; i++)
   {
   printf( "%02x", CANRxMsg->Data[i]);
   }
  printf( "WriteIOVect");
  }

if( chrdy())
 {
 getchar();
 uint32_t ms = MilliSeconds();
 CAN_Tx(true, 0x55, false,  &ms, 4);
 }

 if( (MilliSeconds()-t) > BlinkTime)
  {
  t = MilliSeconds();
  WriteIOBit(GREEN_LED, !ReadIOBit(GREEN_LED));
  }
 }
return 0;
}
