#include "Globals.h"

#ifndef __PLATFORM_I2C_H
#define __PLATFORM_I2C_H

bool I2CWrite(uint8_t addr, uint8_t* data,  uint16_t nbytes);
uint8_t I2C_ReadByte(uint8_t addr);
bool I2C_Read(uint8_t addr, uint8_t* data, uint16_t nbytes);
uint8_t *I2CReadOffenders(void);

#endif
