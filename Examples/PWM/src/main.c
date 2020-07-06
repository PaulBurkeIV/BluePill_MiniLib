/*****************************************************************************************
 *
 * This example sets up TIM2 to output 20kHz motor controls on pins A8 - A11.
 * Each channel has a minimum value of 0 (0%), maximum 1000 (100%), and a power- up vale of 0.
 * The PWM_RATE factor is left at the default value of 8. This controls the scaling
 * of the servo move speed. A rate of 1 will add or subtract 1/(2^8) or 1/256
 * to the position each update.
 * In this case, the servo is updated every millisecond using the SysTickHook(),
 * so it will take 256 seconds to acceleratel from minimum to maximum at a rate of 1,
 * or 25.6 seconds at rate 10 etc.
 * Servo channels are 0 (A8) to 3 (A11).
 *
 * pos <chann> <postion> <rate>
 * e.g. motor 0
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

// Initialise motor PWM_ on TIM1
// 20kHz 1/1000 resolution

ProfPWM_PinInit(PWM_1_IO);
ProfPWM_PinInit(PWM_2_IO);
ProfPWM_PinInit(PWM_3_IO);
ProfPWM_PinInit(PWM_4_IO);
ProfPWM_BaseInit(PWM_TIM, 20000, 1000);
ProfPWM_ChannelInit(0, PWM_TIM, 1, 0, 1000, 0, false);
ProfPWM_ChannelInit(1, PWM_TIM, 2, 0, 1000, 0, false);
ProfPWM_ChannelInit(2, PWM_TIM, 3, 0, 1000, 0, false);
ProfPWM_ChannelInit(3, PWM_TIM, 4, 0, 1000, 0, false);

printf("Blue Pill motor PWM_ demo started\r\n");
CmdBuffIndex = 0;
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
//	Sets motor PWM_ to value
//	Value > MOTOR_MAX set to MOTOR_MAX
//


void ParseCommand(uint8_t *CmdBuff)
{
uint32_t v1, v2, v3;

uint8_t cmd[16] = {0};
if ( !sscanf(CmdBuff, "%s", cmd))
 return;

if(StringMatchNC(cmd, "pwm"))
 {
 CmdBuff += strlen("pwm");
 if( sscanf(CmdBuff, "%1u%u%u", &v1, &v2, &v3) < 3)
  return;
 printf( "pwm %u -> %u at %u\r\n", v1, v2, v3);
 ProfPWM_Set(v1, v2, v3);
 }
else if(StringMatchNC(cmd, "halt"))
 {
 CmdBuff += strlen("halt");
 if( sscanf(CmdBuff, "%1u", &v1) < 1)
  return;
 ProfPWM_Halt(v1,0);
 }

}
