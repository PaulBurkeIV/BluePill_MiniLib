/*
 * Print.h
 *
 *  Created on: 19 May 2016
 *      Author: User
 */

#ifndef APPINC_PRINT_H_
#define APPINC_PRINT_H_

int mprintf(const char *format, ...);
int uprintf(void *dst, const char *format, ...);
int sprintf(char *str,const char *fmt, ...);

#define printf mprintf

#endif /* APPINC_PRINT_H_ */
