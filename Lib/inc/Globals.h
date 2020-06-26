/********************************************************************************
* $Author: AmlodipineFiend
*******************************************************************************/

#ifndef __GLOBALS_H
#define __GLOBALS_H

#define Bit(x) (1<<(x))
#define _WEAK __attribute__((weak))

uint32_t MilliSeconds(void);
void WaitTicks(uint32_t t);

bool chrdy(void);
uint16_t getchar(void);
void putchar(uint16_t c);
void puts(uint8_t * str);

#endif


