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
#include "Servo.h"

/************ Some defs for this demo *********************/

uint32_t BlinkTime = 499;

void DoSerialIn(void);
void ParseCommand(uint8_t *CmdBuff);
#define CMDBUFSZ 32
uint8_t CmdBuff[CMDBUFSZ];
uint8_t CmdBuffIndex = 0;

#define MOTOR_MIN 0
#define MOTOR_MAX 1023

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

// Initialise servo PWM on TIM2
// 20ms repetition rate 1/20000 resolution
// Suitable for typical servos with 1.0ms - 2.0ms range
// when you get 1/1000 position resolution.
// I bet cheap servos can't achieve that!

ServoPinInit(SERVO1_IO);
ServoPinInit(SERVO2_IO);
ServoPinInit(SERVO3_IO);
ServoPinInit(SERVO4_IO);
ServoBaseInit(SERVO_PWM);
ServoStart(SERVO_PWM);
ServoChannelInit(0, SERVO1, 1000, 2000, 1500, false);
ServoChannelInit(1, SERVO2, 1000, 2000, 1500, false);
ServoChannelInit(2, SERVO3, 1000, 2000, 1500, false);
ServoChannelInit(3, SERVO4, 1000, 2000, 1500, false);

printf("Blue Pill Servo demo started\r\n");

while(1) // Equivalent to Arduino "loop"
 {
 DoSerialIn();

 if( (MilliSeconds()-t) > BlinkTime)
  {
  t = MilliSeconds();
//  WriteIOBit(GREEN_LED, !ReadIOBit(GREEN_LED));
  }
 }
return 0;
}

void SysTickHook(void)
{
WriteIOBit(GREEN_LED, !ReadIOBit(GREEN_LED));
PositionServo();
}


void DoSerialIn(void)
{
if(chrdy())		// Command input
 {
 uint8_t c = getchar();
 putchar(c);
 switch(c)
  {
  case '\r':
	      CmdBuff[CmdBuffIndex] = 0;	// Terminate the input
	      puts("\r\n");
	      ParseCommand(CmdBuff);			// Do command
	      CmdBuffIndex = 0;		// Reinitialise for next input
	      CmdBuff[0] = 0;
	      puts(">");
	      break;

  case 'a' ... 'z':
  case 'A' ... 'Z':
  case '0' ... '9':
  case ' ':
  case '.':
  case '-':
	      CmdBuff[CmdBuffIndex] = c;
	      CmdBuffIndex++;
	      if( CmdBuffIndex > (CMDBUFSZ-2)) // Make sure there's room for termination
	       {
	       CmdBuffIndex = 0;
	       }
	      break;
  default:
	      CmdBuff[CmdBuffIndex] = 0;	// Terminate the input
	      puts("\r\n>");
	      CmdBuffIndex = 0;		// Reinitialise for next input
	      CmdBuff[0] = 0;
	      puts(">");
	      break;


  }
 }
}

/************ Rational string compares *************************/
//

bool StringMatchNC( uint8_t *s1, uint8_t *s2)
{
while(*s1)
 {
 if( tolower(*s2) != tolower(*s1))
  return false;
 s1++;
 s2++;
 }
return true;
}

bool StringMatch( uint8_t *s1, uint8_t *s2)
{
while(*s1)
 {
 if( *s2 != *s1)
  return false;
 s1++;
 s2++;
 }
return true;
}


/************ Command parser ***********************************/
//
// Commands:
// Motor (Channel) (value)<return>
// e.g. M 1 200<return>
//	Sets motor PWM to value
//	Value > MOTOR_MAX set to MOTOR_MAX
//


void ParseCommand(uint8_t *CmdBuff)
{
uint32_t v1;
uint32_t v2;

uint8_t cmd[16] = {0};
if ( !sscanf(CmdBuff, "%s", cmd))
 return;

if(StringMatchNC(cmd, "servo"))
 {
 CmdBuff += strlen("servo");
 if( sscanf(CmdBuff, "%1u%u", &v1, &v2) < 2)
  return;
  printf( "Servo %u -> %u\r\n", v1, v2);
  switch(v1)
   {
   case 1:
	  ServoSet(1, v1, v2);
	  break;
   case 2:
	  ServoSet(2, v1, v2);
	  break;
   case 3:
	  ServoSet(3, v1, v2);
	  break;
   case 4:
	  ServoSet(4, v1, v2);
	  break;
   }
  }
}
