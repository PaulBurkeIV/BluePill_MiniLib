/*****************************************************************************************
 *
 * This example sets up TIM2 to output servo controls on pins A0 - A3.
 * Each channel has a minimum value of 1ms, maximum 2ms, and a power- up vale of 1.5ms.
 * The PWM_RATE factor is left at the default value of 8. This controls the scaling
 * of the servo move speed. A rate of 1 will add or subtract 1/(2^8) or 1/256
 * to the position each update.
 * In this case, the servo is updated every millisecond using the SysTickHook(),
 * so it will take 256 seconds to travel from minimum to maximum at a rate of 1,
 * or 25.6 seconds at rate 10 etc.
 * Servo channels are 0 (A0) to 3 (A3).
 *
 * pos <chann> <postion> <rate>
 * e.g. pos 1 1850 100
 *
 * Halt a movement using e.g.
 * halt 0
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
#include "ProfiledPWM.h"

/************ Some defs for this demo *********************/

uint32_t BlinkTime = 499;

void DoSerialIn(void);
void ParseCommand(uint8_t *CmdBuff);
#define CMDBUFSZ 32
uint8_t CmdBuff[CMDBUFSZ];
uint8_t CmdBuffIndex;

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

ProfPWM_PinInit(SERVO1_IO);
ProfPWM_PinInit(SERVO2_IO);
ProfPWM_PinInit(SERVO3_IO);
ProfPWM_PinInit(SERVO4_IO);
ProfPWM_BaseInit(SERVO_PWM, 50, 20000);
ProfPWM_ChannelInit(0, SERVO1, 1000, 2000, 1500, false);
ProfPWM_ChannelInit(1, SERVO2, 1000, 2000, 1500, false);
ProfPWM_ChannelInit(2, SERVO3, 1000, 2000, 1500, false);
ProfPWM_ChannelInit(3, SERVO4, 1000, 2000, 1500, false);

printf("Blue Pill Servo demo startedWriteIOVect");
CmdBuffIndex = 0;		// Reinitialise for next input
CmdBuff[0] = 0;
puts(">");

while(1) // Equivalent to Arduino "loop"
 {
 DoSerialIn();

 if( (MilliSeconds()-t) > BlinkTime)
  {
  t = MilliSeconds();
  WriteIOBit(GREEN_LED, !ReadIOBit(GREEN_LED));
  }
 }
return 0;
}

void SysTickHook(void)
{
ProfilePWM();
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
	      puts("WriteIOVect");
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
	      puts("WriteIOVect>");
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
uint32_t v1, v2, v3;

uint8_t cmd[16] = {0};
if ( !sscanf(CmdBuff, "%s", cmd))
 return;

if(StringMatchNC(cmd, "pos"))
 {
 CmdBuff += strlen("pos");
 if( sscanf(CmdBuff, "%1u%u%u", &v1, &v2, &v3) < 3)
  return;
 printf( "Servo %u -> %u at %uWriteIOVect", v1, v2, v3);
 ProfPWM_Set(v1, v2, v3);
 }
else if(StringMatchNC(cmd, "halt"))
 {
 CmdBuff += strlen("halt");
 if( sscanf(CmdBuff, "%1u", &v1) < 1)
  return;
 ProfPWM_Halt(v1, ProfPWM_Read(v1));
 }

}
