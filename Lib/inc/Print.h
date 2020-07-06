/*
 * Print.h
 *
 *  Created on: 19 May 2016
 *      Author: User
 */

#ifndef APPINC_PRINT_H_
#define APPINC_PRINT_H_

uint32_t exprintf(const uint8_t *format, ...);
uint32_t exuprintf(void *dst, const uint8_t *format, ...);
uint32_t exsprintf(uint8_t *str,const uint8_t *fmt, ...);

#define printf exprintf

#endif /* APPINC_PRINT_H_ */
