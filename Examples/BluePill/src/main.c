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
#include "SPI_25LCxx.h"
#include "SPI_23S17.h"
#include "ProfiledPWM.h"

/************ Some defs for this demo *********************/
//
#define SERVO_MIN 1000
#define SERVO_MAX 2000
#define MOTOR_MIN 0
#define MOTOR_MAX 1023

uint32_t BlinkTime = 499;

const uint8_t MemClr[1024] = {0};
const uint8_t MemTstMsg[] = "O for a muse of fire, that would ascend the brightest heaven of invention! A kingdom for a stage, princes to act etc etc\r\n";
uint8_t spibuff[128];

#define CMDBUFSZ 32
uint8_t CmdBuff[CMDBUFSZ];
uint8_t CmdBuffIndex = 0;
bool ShowMsgs = true;

void DoSerialIn(void);
void ParseCommand(uint8_t *CmdBuff);

#define MCP23S17 SPI2, PC14

void InitKeyPad(void);
uint8_t DoKeyPad(void);

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
//InituSCounter();	// Start a 1us timer for fine timing up to 65ms

//Initialise the port bits

InitIOBit(SWITCH_INIT);
InitIOBit(GREEN_LED_INIT);
InitIOBit(PA8_IN_PU);
IOPinIRQInit(PA8, EXTIRQ_FALLING);

// Initialise UART and its IO pins
// 115200 Baud, 8N1

InitIOBit(SERIAL_TX_INIT);
InitIOBit(SERIAL_RX_INIT);
InitUART(SERIAL, 115200, DATA_8N1);

// Initialise 8MHz out
//InitIOBit(CLOCK_OUT);
//void InitClockOutput(MCO_SYS);



//
InitIOBit(POT_IO);
InitIOBit(PB12_SPI2NSS);
ADCInit(POT_ADC);
StartADC(POT);

InitIOBit(PB12_SPI2NSS);
InitIOBit(PB13_SPI2CK);
InitIOBit(PB14_SPI2MISO);
InitIOBit(PB15_SPI2MOSI);
SPI_Init(SPI2);
MCP23S17_Init (MCP23S17);

// Initialise PWM on TIM2
// 20ms repetition rate 1/20000 resolution
// Suitable for typical servos with 1.0ms - 2.0ms range
// when you get 1/1000 position resolution.
// I bet cheap servos can't achieve that!

//InitIOBit(SERVO1_IO);
//InitIOBit(SERVO2_IO);
//InitIOBit(SERVO3_IO);
//InitIOBit(SERVO4_IO);
//InitPWMTimebase(SERVO_PWM, 50, 20000);
////InitPWMChannel(SERVO1, 1000, false);
////InitPWMChannel(SERVO2, 1500, false);
//InitPWMChannel(SERVO3, 1500, false);
////InitPWMChannel(SERVO4, 10000, false);
//PWM_Start(SERVO_PWM);


InitIOBit(PA11_CANRX_INIT);
InitIOBit(PA12_CANTX_INIT);
InitCAN();

printf("Blue Pill started\r\n");

while(1) // Equivalent to Arduino "loop"
 {
 t_CANRXMsg *CANRxMsg = CAN_Rx();
 if( CANRxMsg && ShowMsgs)
  {
  printf( "CAN %x{%c,%d): ", CANRxMsg->ID, CANRxMsg->Extended ? 'E' : 'N', CANRxMsg->Len);
  for( uint8_t i=0; i<CANRxMsg->Len; i++)
   {
   printf( "%02x", CANRxMsg->Data[i]);
   }
  puts( "\r\n");
  }

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
WriteIOBit(GREEN_LED, !ReadIOBit(GREEN_LED));
}

void DoSerialIn(void)
{
if(chrdy())		// Command input
 {
 uint8_t c = getchar();
 putchar(c);
 ShowMsgs = false;
 switch(c)
  {
  case '\r':
	      CmdBuff[CmdBuffIndex] = 0;	// Terminate the input
	      puts("\r\n");
	      ParseCommand(CmdBuff);			// Do command
	      CmdBuffIndex = 0;		// Reinitialise for next input
	      CmdBuff[0] = 0;
	      puts(">");
	      ShowMsgs = true;
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
	      ShowMsgs = true;
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
//
// Servo (Channel) (value)<return>
// e.g. S 2 600<return>
//	Sets servo channel to value taking minimum as 0 and maximum as 1000
// 	Values > 1 treated as maximum
//
// Motor (Channel) (value)<return>
// e.g. M 1 200<return>
//	Sets motor PWM to value
//	Value > MOTOR_MAX set to MOTOR_MAX
//
// LED (blinktime)
// e.g L 333
//	Sets LED on/off time to blinktime in milliseconds
// You can set anything here, up to whatever 32 bit holds
// The longest blink rate is abot a cycle every 15 weeks
// Maybe lockdown will be over in a few blinks!
//


void ParseCommand(uint8_t *CmdBuff)
{
uint32_t v1;
uint32_t v2;

uint8_t cmd[16] = {0};
if ( !sscanf(CmdBuff, "%s", cmd))
 return;

if( StringMatchNC(cmd, "time"))
 {
 printf( "Time = %d\r\n", MilliSeconds());
 }
if( StringMatchNC(cmd, "led"))
 {
 CmdBuff += strlen("LED");
 if( !sscanf(CmdBuff, "%u", &v1))
  return;
 BlinkTime = v1;
 printf( "Blink = %u\r\n", BlinkTime);
 }
else if(StringMatchNC(cmd, "Motor"))
 {
 CmdBuff += strlen("motor");
 if( sscanf(CmdBuff, "%1u%u", &v1, &v2) < 2)
  return;
  if( v2 > MOTOR_MAX)
   {
   v2 = MOTOR_MAX;
   }
  printf( "Motor %u -> %u\r\n", v1, v2);
  switch(v1)
   {
   case 1:
	  SetPWM(MOTOR1, v2);
	  break;
   case 2:
	  SetPWM(MOTOR2, v2);
	  break;
   case 3:
	  SetPWM(MOTOR3, v2);
	  break;
//   case 4:
//	  SetPWM(MOTOR4, v2);
//	  break;
   }
 }
else if(StringMatchNC(cmd, "lcxx"))
 {
 uint32_t madr=0, nbytes=0;
 uint8_t sub[32];
 CmdBuff += strlen("LCxx");
 if( sscanf(CmdBuff, "%s%x%x", sub, &madr, &nbytes) < 3)
  return;
 if( !strcmp(sub, "dump"))
  {
  for(uint16_t i=madr; i<(madr+nbytes); i+=16)
   {
   printf( "%04x: ", i);
    Read25LCxx(SPI2,PB12_SPI2NSS_PIN, spibuff, i, 16);
    for(uint16_t j=0; j<16; j++)
     {
     printf( "%02x%s", spibuff[j], (j%16 == 15) ? "\r\n" : " ");
     }
    printf( "\r\n");
    WaitTicks(6);
    if(chrdy())
     {
     if( getchar() == 0x03)
      break;
     }
   }
  printf( "\r\n");
  }
 else if(StringMatchNC(sub, "clr"))
  {
  for(uint16_t i=madr; i<(madr+nbytes); i++)
   {
   Write25LCxxByte(SPI2,PB12_SPI2NSS_PIN, 0, i);
   }
  }
 else if( !strcmp(sub, "fill"))
  {
  for(uint16_t i=madr; i<(madr+nbytes); i++)
   {
   Write25LCxxByte(SPI2,PB12_SPI2NSS_PIN, 0xff, i);
   }
  }
 else if( StringMatchNC(sub, "write"))
  {
  Write25LCxx(SPI2,PB12_SPI2NSS_PIN, MemTstMsg, madr, strlen(MemTstMsg));
  }
 }
}

void PinInterrupt(uint8_t Line)
{
union {uint32_t i; uint8_t b[4];} ms;
ms.i = MilliSeconds();
//WriteIOBit(GREEN_LED, !ReadIOBit(GREEN_LED));
CAN_Tx(true, 0x55, false, ms.b, 4);
}


#define KPSCANTIME 10
#define KP_O1 Bit(1)
#define KP_O2 Bit(6)
#define KP_O3 Bit(5)
#define KP_O4 Bit(3)
#define KP_I1 Bit(2)
#define KP_I2 Bit(0)
#define KP_I3 Bit(4)

#define KP_IMASK (KP_I1 | KP_I2 | KP_I3)
uint8_t KPOpLine;
uint32_t KPScanTime;

void InitKeyPad(void)
{
MCP23S17DIR_Write (MCP23S17, 0, ~(KP_O1 | KP_O2 | KP_O3 | KP_O4)); // Set matrix drives as outputs
MCP23S17PU_Write (MCP23S17, 0, KP_IMASK);	// Matrix inputs pulled up
MCP23S17POL_Write (MCP23S17, 0, KP_IMASK);	// Matrix inputs inverted
KPOpLine = KP_O1;
KPScanTime = MilliSeconds();
MCP23S17_Write (MCP23S17, 0, ~KPOpLine);
}

uint8_t GetKey(uint8_t Line)
{
uint8_t Val = 0xff;
uint8_t key = MCP23S17_Read(MCP23S17, 0) & KP_IMASK;

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

enum KeyState {NOKEY, KEY};

uint8_t DoKeyPad(void)
{
static uint8_t State = NOKEY;
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
  		 MCP23S17_Write (MCP23S17, 0, ~KPOpLine);
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
 		 MCP23S17_Write (MCP23S17, 0, ~KPOpLine);
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

