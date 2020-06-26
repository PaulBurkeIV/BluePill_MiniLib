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
#define 	SERVO1_IO		PA0_TIM2CH1_O
#define 	SERVO2_IO		PA1_TIM2CH2_O
#define 	SERVO3_IO		PA2_TIM2CH3_O
#define 	SERVO4_IO		PA3_TIM2CH4_O
#define 	SERVO_PWM		TIM2
#define 	SERVO1			SERVO_PWM,1
#define 	SERVO2			SERVO_PWM,2
#define 	SERVO3			SERVO_PWM,3
#define 	SERVO4			SERVO_PWM,4

#define 	NSERVOS			4


// *********** Port B ***********
#define 	SERIAL_TX_INIT		SERIAL3_PB10_TX
#define 	SERIAL_RX_INIT		SERIAL3_PB11_RX
#define		SERIAL			SERIAL3
// ******* UART direct or buffered *******

#define UART_BUFFERED

// *********** Port C ***********
#define 	GREEN_LED	PC13
#define 	GREEN_LED_INIT 	PC13_OUT_PP

#endif /* APPINC_APPIODEFS_H_ */
