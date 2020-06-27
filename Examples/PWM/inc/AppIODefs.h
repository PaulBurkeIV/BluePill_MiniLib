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
#define 	PWM_1_IO			PA8_TIM1CH1_O
#define 	PWM_2_IO			PA9_TIM1CH2_O
#define 	PWM_3_IO			PA10_TIM1CH3_O
#define 	PWM_4_IO			PA11_TIM1CH4_O
#define 	PWM_TIM			TIM1
#define 	PWM_1			PWM_TIM,1
#define 	PWM_2			PWM_TIM,2
#define 	PWM_3			PWM_TIM,3
#define 	PWM_4			PWM_TIM,4

#define 	N_PROFPWMS		4


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
