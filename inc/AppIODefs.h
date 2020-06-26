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

#ifndef APPINC_APPIODEFS_H_
#define APPINC_APPIODEFS_H_


/*************** Examples ***********************/

// *********** Port A ***********

// *********** Port B ***********
#define 	SERIAL_TX_INIT		SERIAL3_PB10_TX
#define 	SERIAL_RX_INIT		SERIAL3_PB11_RX
#define		SERIAL			SERIAL3

#define		MEM_CS_INIT		PB12_SPI2NSS
#define		MEM_CK_INIT		PB13_SPI2CK
#define		MEM_MISO_INIT		PB14_SPI2MISO
#define		MEM_MOSI_INIT		PB15_SPI2MOSI
#define 	MEM_SPI			SPI2

#define		MEM_CS			PB12

// ******* UART direct or buffered *******

#define UART_BUFFERED

// *********** Port C ***********
#define 	GREEN_LED	PC13
#define 	GREEN_LED_INIT 	PC13_OUT_PP

#endif /* APPINC_APPIODEFS_H_ */
