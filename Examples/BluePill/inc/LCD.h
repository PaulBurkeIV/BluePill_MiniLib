// Functions for LCD access
#ifndef _LCD_H_
#define _LCD_H_

void LCDInit(void);
void LCDputs( char *str);
void LCDSetCrsr(uint8_t n);
void LCDSetLine( int line);
int LCDputch( char c );
void LCDClear(void);
void LCDputx( unsigned int c);
void LCDPuts( char *str);
void LCDGotoXY( uint8_t pos, uint8_t Line);
void LCDClrLine(uint8_t line);

// Target port routines

void LCDPortOut(bool dir);
char ReadLCDData(void);
void WriteLCDData(char c);

#define LCDprintf(format,args...) uprintf(LCDputch, format, args)

#endif // _LCD_H_


