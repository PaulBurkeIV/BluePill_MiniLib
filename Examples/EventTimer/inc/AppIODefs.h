/*
 * AppIODefs.h
 *
 *  Created on: 30 Apr 2020
 *      Author: AmlodipineFiend
 */
/***************************************************************/
// How to use this header file:
// Write your redefinition for each pin with your chosen pin name
// Keep them in port, pin order
// That way you can readily see if you've used a pin twice!
/***************************************************************/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef APPINC_APPIODEFS_H_
#define APPINC_APPIODEFS_H_


/*************** Examples ***********************/

// *********** Port A ***********

#define 	EVENT_IN		PA8_IN_NP
#define		EVENT_TIM		TIM1
#define 	EVENT_CHANNEL		1

// *********** Port B ***********
#define 	SERIAL_TX_INIT		SERIAL3_PB10_TX
#define 	SERIAL_RX_INIT		SERIAL3_PB11_RX
#define		SERIAL			SERIAL3
// ******* UART direct or buffered *******

#define UART_BUFFERED
#undef COM3_BUFF_SIZE
#define COM3_BUFF_SIZE 1024

// *********** Port C ***********
#define 	GREEN_LED	PC13
#define 	GREEN_LED_INIT 	PC13_OUT_PP

// Half-bit time limits
/*
In a "1" bit, the first and last part of a bit shall have the same duration, and that duration shall nominally be
58 microseconds, giving the bit a total duration of 116 microseconds. Digital Command Station
components shall transmit "1" bits with the first and last parts each having a duration of between 55 and 61
microseconds. A Digital Decoder must accept bits whose first and last parts have a duration of between 52
and 64 microseconds, as a valid bit with the value of "1".

In a "0" bit, the duration of the first and last parts of each transition shall nominally be greater than or
equal to 100 microseconds. To keep the DC component of the total signal at zero as with the "1" bits, the 25
first and last part of the "0" bit are normally equal to one another. Digital Command Station components
shall transmit "0" bits with each part of the bit having a duration of between 95 and 9900 microseconds
with the total bit duration of the "0" bit not exceeding 12000 microseconds. A Digital Decoder must accept
bits whose first or last parts have a duration of between 90 and 10000 microseconds as a valid bit with the
value of "0".
*/

typedef enum { NHB, H0, H05, H1, BHB } t_BitState;

// It's microseconds * megaherz, so the exponents cancel

#define TB1_MIN 220	//55us * 4
#define TB1_MAX 244 	//61us * 4
#define TB0_MIN 380	//95us * 4
#define TB0_MAX 4800	//12000us * 4



#endif /* APPINC_APPIODEFS_H_ */
