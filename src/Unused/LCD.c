/* LCD display driver functions - 4 bit mode */

#include <stdint.h>
#include <stdbool.h>
#include "main.h"
#include "AppIODefs.h"
#include "lcd.h"


/* 16x2 standard LCD display */

// define E, RW and RS and 4 bit data Port */
#define LCDWAIT 2

#define LCDBITE LCD_E
#define LCDBITRW LCD_RW
#define LCDBITRS LCD_RS

#define LCDSETWR() LCDPortOut(true); WaitMicroSec(LCDWAIT)
#define LCDSETRD() LCDPortOut(false); WaitMicroSec(LCDWAIT)
#define LCDDRD() ReadLCDData()
#define LCDDWR(x) WriteLCDData(x); WaitMicroSec(LCDWAIT)
#define LCDE(x) ((x) ? WriteIOBit(LCD_E,true) : WriteIOBit(LCD_E,false)); WaitMicroSec(LCDWAIT)
#define LCDRW(x) ((x) ? WriteIOBit(LCD_RW,true) : WriteIOBit(LCD_RW,false)); WaitMicroSec(LCDWAIT)
#define LCDRS(x) ((x) ? WriteIOBit(LCD_RS,true) : WriteIOBit(LCD_RS,false)); WaitMicroSec(LCDWAIT)

#define LCDBusyFlag BitMask(7)        /* DB7 is busy flag */

#define LCDLine1 0              /* address of 1st character */
#define LCDLine2 0x40

/* Control codes */

#define LCDClrDis 1
#define LCDHome 2

#define LCDModeSet 4
#define LCDDec 0       /* Subcodes to LCDModeSet */
#define LCDInc 2
#define LCDShift 1        /* shift entire display on entry */

#define LCDonoff 8      /* LCD on off functions */
#define LCDDispOn 4
#define LCDDispOff 0
#define LCDCrsOn 2
#define LCDCrsOff 0
#define LCDCrsBlink 1


#define LCDFuncSet 0x20
#define LCD4bit 0
#define LCD8bit 0x10
#define LCD1line 0
#define LCD2line 8
#define LCD5x7 0
#define LCD5x10 4

#define LCDSetDDRAM 0x80

int LCDBusy(void);
void LCDWrIns( uint8_t ins);

void LCDWait(void)
{
volatile uint32_t t;
for( t=5000; t; t--)
 ;
}

/* Set LCD 4 bit*/

void LCDSet4Bit(void)
{
while( LCDBusy())
 {
 }
LCDSETWR();
LCDRS(0);
LCDRW(0);
LCDDWR(2);
LCDE(1);
LCDE(0);
LCDSETRD();
}

/* Write LCD instruction */
 
void LCDWrIns( uint8_t ins)
{
while( LCDBusy())
 {
 }
LCDSETWR();
LCDRS(0);
LCDRW(0);
LCDDWR(ins>>4);
LCDE(1);
LCDE(0);
LCDDWR(ins);
LCDE(1);
LCDE(0);
LCDSETRD();
}
/* write data as 2x4bit */

int LCDputch(char c)
{
while( LCDBusy())
 {
 }
LCDSETWR();
LCDRS(1);
LCDRW(0);
LCDDWR(c>>4);
LCDE(1);
LCDE(0);
LCDDWR(c);
LCDE(1);
LCDE(0);
LCDSETRD();
return c;
}

void LCDSetCrsr(uint8_t n)
{
LCDWrIns( LCDSetDDRAM + n);
}

void LCDSetLine( int line)
{
if( !line)
 LCDSetCrsr( LCDLine1);
else
 LCDSetCrsr( LCDLine2);
}


int LCDReadStatus(void)
{
unsigned char st;
LCDPortOut(false);
LCDSETRD();
LCDRW(1);
LCDRS(0);
LCDE(1);
st = LCDDRD();
LCDE(0);
LCDE(1);
st = (st<<4) | LCDDRD();
LCDE(0);
return st;
}

int LCDBusy(void)
{
bool st;
LCDRW(1);
LCDRS(0);
LCDPortOut(false);
LCDE(1);
st = GetBitState(LCD3);
LCDE(0);
LCDE(1);
LCDE(0);
return st;
}

/* Output a string */

void LCDputs( char *str)
{
while( *str)
 LCDputch(*(str++));
}

void LCDClear(void)
{
LCDWrIns( LCDClrDis);
LCDSetLine(0);
}

/* Initialise LCD to 4 bit */

void LCDInit(void)
{
uint32_t timer;
for( timer = MilliSeconds(); (MilliSeconds()-timer) < 100;)
 {
 }
LCDSet4Bit();
for( timer = MilliSeconds(); (MilliSeconds()-timer) < 10;)
 {
 }
LCDWrIns( LCDFuncSet + LCD4bit + LCD2line + 0x04);
for( timer = MilliSeconds(); (MilliSeconds()-timer) < 10;)
 {
 }
LCDWrIns( LCDonoff + LCDDispOn );
for( timer = MilliSeconds(); (MilliSeconds()-timer) < 10;)
 {
 }
LCDClear();
for( timer = MilliSeconds(); (MilliSeconds()-timer) < 10;)
 {
 }
LCDWrIns( LCDModeSet + LCDInc );
}

/**********************************************/

/* Print input char in hex */

void LCDputx( unsigned int c)
{
unsigned int x;
x = (c & 0xf0) >> 8;
if( x > 9)
 LCDputch( x-10+'A');
else
 LCDputch( x+'0');
x = c & 0x0f;
if( x > 9)
 LCDputch( x-10+'A');
else
 LCDputch( x+'0');
}

void LCDGotoXY( uint8_t pos, uint8_t Line)
{
uint8_t crsr = 0;
if( pos > 15)
 {
 pos = 15;
 }
crsr = pos;
if( Line > 0)
 {
 crsr |= LCDLine2;
 }
LCDSetCrsr(crsr);
}

void LCDClrLine(uint8_t line)
{
uint8_t i;
LCDGotoXY(0,line);
for(i=0; i<16; i++)
 {
 LCDputch(' ');
 }
LCDGotoXY(0,line);
}
