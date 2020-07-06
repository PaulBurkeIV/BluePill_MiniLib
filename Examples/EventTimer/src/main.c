/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Event Timer Example
 *
 * This demo sets up Timer TIM1 to decode DCC input (at 3.3V level) on pin A8.
 * A ring buffer is filled with bit time values by DMA. F103 limitations mean
 * that capture is between two rising or two falling edges, i.e. NOT the usual
 * half- bit sequence. To compensate for inverting DCC connections, the edge used
 * is inverted if the number of half- and- half bits > 100.
 * You can manually switch the polarity by pressing any key.
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

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

uint32_t BlinkTime = 999;

void DoSerialIn(void);
void ParseCommand(uint8_t *CmdBuff);
t_BitState NextBit(uint16_t BitTime);

#define EVENTBUFSZ 128
uint16_t EventBuff[EVENTBUFSZ];
uint16_t EventBuffOutInx;

t_EventTimer Events;
uint16_t ncnts;

uint16_t LastTime = 0;
uint32_t BitTimes[5];
uint32_t BitCnt[5];

uint8_t Chk;
#define PKTBUFSIZ 16
uint8_t PacketData[PKTBUFSIZ+1];

uint16_t ReadPacket(t_BitState Bit);

#define PKT_CHKERR 0xff
#define PKT_BITERR 0xfe

uint8_t Packet_BitCnt;
uint8_t DataCnt;
uint8_t PacketCount;
uint16_t Addr;

uint32_t nchkerrs;
uint32_t NPackets;

//uint32_t TestArray32[] = {0,1,2,3,4,5,6,7,8,9,10,-11,-12,13,14,15,16,17,18,19,20};
//uint16_t TestArray16[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
//uint8_t TestArray8[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
//
/************ Main ****************************************/
//
// The Raging main()


int main(void)
{
uint32_t BlipTime = 0;
uint32_t InvTim = 0;
bool silent = false;

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

Events.TIM = EVENT_TIM;
Events.Channel = EVENT_CHANNEL;
Events.Div = 36;		// Gives 0.5us resolution @ 72MHz
Events.DMA = 0;	// Filled in by init
Events.Buff = EventBuff;
Events.BufSz = EVENTBUFSZ;
Events.OutCnt = 0;
InitIOBit(EVENT_IN);
InitEventTimer( &Events);
InitIOBit(PA9, OUTPUT_PP);

printf("Blue Pill Event Timer example started\n");

while(1) // Equivalent to Arduino "loop"
 {
 ncnts = EventTimerDataRdy(&Events);
 if( ncnts)
  {
  uint16_t pktsz;
  t_BitState bit;
  uint16_t v;
  WriteIOBit(GREEN_LED, true);

  v = ReadEventTimer(&Events);	//Get the next DCC bit time
  bit = NextBit(v);		//Is time since last byte DCC 1, DCC 0, or error?
  BitCnt[bit]++;		// Count the bits of each type for monitoring
  if((bit == H0) || (bit == H1)) // It's a real DCC bit
   {
   pktsz = ReadPacket(bit);	// So turn it into a command packet
   if( pktsz > PKTBUFSIZ)	// error condition
    {
    printf( "Packet error %02x\n", pktsz);
    }
   else if( pktsz && !silent) // OK - print resuts unless silent
    {
    uint16_t addr = PacketData[1];	// Short address or MS of long address
    if( addr != 0xff)			// ignore idle packets
     {
     if( !( addr & 0xc0))		// Short address?
      {
      printf( "B%d: %[c,02x\n", addr, 2, PacketData[0]-1, PacketData);
      }
     else	// It's long
      {
      addr &= ~0xc0;
      addr = (addr << 8) + PacketData[2];
      printf( "E%d: %[c,02x\n", addr, 3, PacketData[0]-2, PacketData);
      }
     }
    }
   WriteIOBit(GREEN_LED, false);
   }
  }

 if( chrdy())
  {
  switch( getchar() )
   {
   case 's':
  	silent = !silent;
  	printf( "%s\n", silent ? "Silent" : "Verbose");
  	break;
   case 'f':
  	InvertEventTimer(&Events);
  	InvTim = MilliSeconds();
  	break;
   case 'd':
   	printf( "Run time (ms): %d Packet count: %d Checksum errors: %d\n", MilliSeconds(), NPackets, nchkerrs);
  	printf( "NHB %d H0 %d H05 %d H1 %d BHB %d\n", BitCnt[NHB], BitCnt[H0], BitCnt[H05], BitCnt[H1], BitCnt[BHB]);
  	printf( "TNHB %d TH0 %d TH05 %d TH1 %d TBHB %d\n",
	   BitTimes[NHB]/BitCnt[NHB], BitTimes[H0]/BitCnt[H0], BitTimes[H05]/BitCnt[H05], BitTimes[H1]/BitCnt[H1], BitTimes[BHB]/BitCnt[BHB]);
	   BitCnt[H05] = 0;
  	break;

   }
  }

if( BitCnt[H05] > 50)
 {
 uint8_t i;
 InvertEventTimer(&Events);
 printf( "Polarity switched in %dms\n", MilliSeconds() - InvTim);
 for(i=0; i<5; i++)
  {
  BitCnt[i] = 0;
  BitTimes[i] = 0;
  }
  }

 if( (MilliSeconds()-BlipTime) > BlinkTime)
  {
  BlipTime = MilliSeconds();
  //WriteIOBit(GREEN_LED, !ReadIOBit(GREEN_LED));
  }
 }
return 0;
}


volatile uint16_t hbt0;
volatile uint16_t hbt1;
volatile uint16_t hbtb;



t_BitState NextBit(uint16_t Time)
{
uint16_t BitTime = (Time-LastTime);
LastTime = Time;
t_BitState tbit = NHB;

 if( (BitTime >= TB1_MIN) && (BitTime <= TB1_MAX))
  {
  tbit = H1;
  }
 else if( (BitTime > TB1_MAX) && (BitTime < TB0_MIN))
  {
  tbit = H05;
  }
 else if( (BitTime >= TB0_MIN) && (BitTime <= TB0_MAX))
  {
  tbit = H0;
  }
 else
  {
  tbit = BHB;
  }
BitTimes[tbit] += BitTime;
return tbit;
}

enum {PRE, DATA, W_NEXT, PKT_DONE};

uint16_t PacketState;

volatile uint32_t NResynchs;

uint16_t ReadPacket(t_BitState Bit)
{
PacketCount = 0;

switch( PacketState)
  {
  case PRE:
	   switch(Bit)
	    {
	    case H0:
	      	if( Packet_BitCnt > 9) // Got 10+ 1 bits followed by 0: valid preamble
	      	 {
	      	 DataCnt = 1;
	      	 Chk = 0;
	       	 Packet_BitCnt = 0;
	         PacketState = DATA;
	         }
	        else	// Less than 10 1's before 0: not a preamble
	         {
	         Packet_BitCnt = 0;
	         }
	        break;

	    case H1:
		Packet_BitCnt++;
		break;

	    default:
	    	break;
	    }

	   break;

  case DATA:
	  if( (Bit == H0) || (Bit == H1))
	   {
	   Packet_BitCnt++;
	   PacketData[DataCnt] = (PacketData[DataCnt] << 1) | ((Bit == H1) ? 1 : 0); // Shift new bit in
	   if( Packet_BitCnt > 7) // Finished a byte?
	    {
	    Chk ^= PacketData[DataCnt];
	    Packet_BitCnt = 0;
	    PacketState = W_NEXT;
	    }
	   }
	  else
	   {
	   PacketCount = PKT_BITERR;
	   }
	  break;

  case W_NEXT:
	  if( Bit == H0)  // 0 means more data bytes available
	   {
	   Packet_BitCnt = 0;
	   DataCnt++;
	   if( DataCnt > PKTBUFSIZ)	// Don't let the buffer overflow
	    {
	    PacketState = PRE;
	    }
	   else
	    {
	    PacketState = DATA;
	    }
       	   }
	else if( Bit == H1)	// 1 is start of next preamble
	 {
	 Packet_BitCnt = 1;
	 if(Chk )	// A  ^  A = 0 so this should be the checksum
	    {
	    NPackets++;
	    PacketCount = PKT_CHKERR;
	    nchkerrs++;
	    PacketState = PRE;
	    }
	   else	// A complete checksummed package is available
	    {
	    NPackets++;
	    PacketState = PKT_DONE;
	    }
	  }
        break;

  case PKT_DONE:
      	PacketData[0] = DataCnt;
      	PacketCount = DataCnt;
      	PacketState = PRE;
	break;

  default:
  	PacketState = PRE;
  	break;

 }
return PacketCount;
}

