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

#define 	POT_IO			PA0_ADC
#define		POT_ADC			ADC1
#define		POT			ADC1_CH7

#define 	CLOCK_OUT		PA8_MCO
#define 	MOTOR1_IO		PA8_TIM1CH1_O
#define 	MOTOR2_IO		PA9_TIM1CH2_O
#define 	MOTOR3_IO		PA10_TIM1CH3_O
//#define 	MOTOR4_IO		PA11_TIM1CH4_O
#define 	MOTOR_PWM		TIM1
#define 	MOTOR1			MOTOR_PWM,1
#define 	MOTOR2			MOTOR_PWM,2
#define 	MOTOR3			MOTOR_PWM,3
//#define 	MOTOR4			MOTOR_PWM,4


// *********** Port B ***********
#define 	SERIAL_TX_INIT		SERIAL3_PB10_TX
#define 	SERIAL_RX_INIT		SERIAL3_PB11_RX
#define		SERIAL			SERIAL3
// ******* UART direct or buffered *******

#define UART_BUFFERED

// *********** Port C ***********
#define 	GREEN_LED	PC13
#define 	GREEN_LED_INIT 	PC13_OUT_PP
#define 	SWITCH 		PC15
#define 	SWITCH_INIT	PC15_IN_PU

// *********** Clock OP ***********

#define CLK_8M_OUT 		MCO_HSE

//#define COM3_BUFF_SIZE 1024

// *********** Servos ***********

#define 	NSERVOS		4

#endif /* APPINC_APPIODEFS_H_ */
