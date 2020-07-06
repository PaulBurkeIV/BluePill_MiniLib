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

/************ Some defs for this demo *********************/
//
uint32_t BlinkTime = 499;

const uint8_t MemClr[1024] = {0};
const uint8_t MemTstMsg[] = "O for a muse of fire, that would ascend the brightest heaven of invention! A kingdom for a stage, princes to act etc etc\r\n";
uint8_t spibuff[128];

#define CMDBUFSZ 32
uint8_t CmdBuff[CMDBUFSZ];
uint8_t CmdBuffIndex = 0;

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

InitIOBit(GREEN_LED_INIT);

// Initialise UART and its IO pins
// 115200 Baud, 8N1

InitIOBit(SERIAL_TX_INIT);
InitIOBit(SERIAL_RX_INIT);
InitUART(SERIAL, 115200, DATA_8N1);

InitIOBit(MEM_CS_INIT);
InitIOBit(MEM_CK_INIT);
InitIOBit(MEM_MISO_INIT);
InitIOBit(MEM_MOSI_INIT);
SPI_Init(MEM_SPI);

printf("Blue Pill SPI Memory Demo started\r\n");

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
//


void ParseCommand(uint8_t *CmdBuff)
{
uint8_t cmd[16] = {0};
if ( !sscanf(CmdBuff, "%s", cmd))
 return;

if(StringMatchNC(cmd, "mem"))
 {
 uint32_t V1=0, V2=0;
 uint8_t sub[32];
 CmdBuff += strlen("mem");
 if( sscanf(CmdBuff, "%s%x%x", sub, &V1, &V2) < 3)
  return;
 if( !strcmp(sub, "dump"))
  {
  for(uint16_t i=V1; i<(V1+V2); i+=16)
   {
   printf( "%04x: ", i);
   Read25LCxx(MEM_SPI, MEM_CS, spibuff, i, 16);
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
  for(uint16_t i=V1; i<(V1+V2); i++)
   {
   Write25LCxxByte(MEM_SPI, MEM_CS, 0, i);
   }
  }
 else if( !strcmp(sub, "fill"))
  {
  for(uint16_t i=V1; i<(V1+V2); i++)
   {
   Write25LCxxByte(MEM_SPI, MEM_CS, 0xff, i);
   }
  }
 else if( StringMatchNC(sub, "write"))
  {
  printf( "Writing %08x to %04x\r\n", V2, V1);
  Write25LCxx(MEM_SPI, MEM_CS, &V2, V1, 4);
  }
 }
}
