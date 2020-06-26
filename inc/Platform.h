/****************************************
* Description   :
* Blue Pill STM32F103C8 definitions
******************************************/

#ifndef __IO_H
#define __IO_H
#include <stdint.h>
#include <stdbool.h>

#include "stm32f10x.h"
#include "Platform_UART.h"
#include "Platform_PWM.h"
#include "Platform_ADC.h"
#include "Platform_SPI.h"
#include "Platform_CAN.h"

#define _WEAK __attribute__((weak))

#define SYSTEM_CLOCK SystemFrequency_SysClk
#define APB2CLKSPEED SystemFrequency_APB2Clk
#define APB1CLKSPEED SystemFrequency_APB1Clk

extern uint32_t minram;
#define RAM_START (uint32_t)(&minram)
extern uint32_t maxram;
#define RAM_END (uint32_t)(&maxram)

/************* Platform IO defs **********/

#define EXTIRQ_NONE	0
#define EXTIRQ_RISING	1
#define EXTIRQ_FALLING	2
#define EXTIRQ_BOTH	3

void PinInterrupt(uint8_t InterruptPin);	// Whatever you want the interrupt to do....
void IOPinIRQInit( GPIO_TypeDef* Port, uint16_t Pin, uint8_t edge);	// Also enables the interrupt
void IOPinIRQEnable(GPIO_TypeDef* Port, uint16_t Pin);
void IOPinIRQDisable(GPIO_TypeDef* Port, uint16_t Pin);

enum {GPIO_Mode_IN, GPIO_Mode_OUT,  GPIO_Mode_AF, GPIO_Mode_AF_REMAP, GPIO_Mode_AN };
enum {GPIO_PuPd_NP, GPIO_PuPd_PU, GPIO_PuPd_PD};
enum {GPIO_Init_LOW, GPIO_Init_HI, GPIO_Init_NONE};

#define GPIO_OType_PP false
#define GPIO_OType_OD true

#define INPUT_FLOATING 		GPIO_Mode_IN,GPIO_OType_PP,GPIO_PuPd_NP
#define INPUT_PU			GPIO_Mode_IN,GPIO_OType_PP,GPIO_PuPd_PU
#define INPUT_PD			GPIO_Mode_IN,GPIO_OType_PP,GPIO_PuPd_PD
#define INPUT_ANALOG 			GPIO_Mode_AN,GPIO_OType_PP,GPIO_PuPd_NP
#define OUTPUT_PP 		GPIO_Mode_OUT,GPIO_OType_PP,GPIO_PuPd_NP
#define OUTPUT_OD 		GPIO_Mode_OUT,GPIO_OType_PP,GPIO_PuPd_OD
#define GPIO_Mode_AF_IN_NP 		GPIO_Mode_IN,GPIO_OType_PP,GPIO_PuPd_NP
#define OUTPUT_AFPP 		GPIO_Mode_AF,GPIO_OType_PP,GPIO_PuPd_NP
#define GPIO_Mode_AF_OD_NP 		GPIO_Mode_AF,GPIO_OType_OD,GPIO_PuPd_NP
#define GPIO_Mode_AF_OD_PU 		GPIO_Mode_AF,GPIO_OType_OD,GPIO_PuPd_PU
#define GPIO_Mode_AF_OD_NP 		GPIO_Mode_AF,GPIO_OType_OD,GPIO_PuPd_NP

#define INIT_0 0
#define INIT_1 1
#define INIT_X 2

// I/O Port/ pin definitions
#define 	PA0 	GPIOA, 0 		// 	GPIO pin A0
#define 	PA1 	GPIOA, 1 		// 	GPIO pin A1
#define 	PA2 	GPIOA, 2 		// 	GPIO pin A2
#define 	PA3 	GPIOA, 3 		// 	GPIO pin A3
#define 	PA4 	GPIOA, 4 		// 	GPIO pin A4
#define 	PA5 	GPIOA, 5 		// 	GPIO pin A5
#define 	PA6 	GPIOA, 6 		// 	GPIO pin A6
#define 	PA7 	GPIOA, 7 		// 	GPIO pin A7
#define 	PA8 	GPIOA, 8 		// 	GPIO pin A7
#define 	PA9 	GPIOA, 9 		// 	GPIO pin A8
#define 	PA10 	GPIOA, 10 		// 	GPIO pin A10
#define 	PA11 	GPIOA, 11 		// 	GPIO pin A11
#define 	PA12 	GPIOA, 12 		// 	GPIO pin A12
#define 	PA15 	GPIOA, 15 		// 	GPIO pin A15

#define 	PB0 	GPIOB, 0 		// 	GPIO pin B0
#define 	PB1 	GPIOB, 1 		// 	GPIO pin B1
#define 	PB3 	GPIOB, 3 		// 	GPIO pin B3
#define 	PB4 	GPIOB, 4 		// 	GPIO pin B4
#define 	PB5 	GPIOB, 5 		// 	GPIO pin B5
#define 	PB6 	GPIOB, 6 		// 	GPIO pin B6
#define 	PB7 	GPIOB, 7 		// 	GPIO pin B7
#define 	PB8 	GPIOB, 8 		// 	GPIO pin B7
#define 	PB9 	GPIOB, 9 		// 	GPIO pin B8
#define 	PB10 	GPIOB, 10 		// 	GPIO pin B10
#define 	PB11 	GPIOB, 11 		// 	GPIO pin B11
#define 	PB12 	GPIOB, 12 		// 	GPIO pin B12
#define 	PB13 	GPIOB, 13 		// 	GPIO pin B13
#define 	PB14 	GPIOB, 14 		// 	GPIO pin B14
#define 	PB15 	GPIOB, 15 		// 	GPIO pin B15

#define 	PC13 	GPIOC, 13 		// 	GPIO pin C13
#define 	PC14 	GPIOC, 14 		// 	GPIO pin C14
#define 	PC15 	GPIOC, 15 		// 	GPIO pin C15


// Remap bits

#define CAN_REMAP	(0x10<<13)
#define TIM3_REMAP	(0x10<<10)
#define TIM2_REMAP	(0x11<<8)
#define TIM1_REMAP	(0x01<<6)
#define USART1_REMAP	(1<<2)
#define I2C1_REMAP	(1<<1)
#define SPI1_REMAP	1

// IO setup definitions

#define PINDEF(port, pin)		GPIO_TypeDef* port, uint16_t pin

//*****************************
//	Port A
//*****************************

/********************************/
/*	Plain inputs		*/
/********************************/

// No PUPD

#define PA0_IN_NP	PA0, INPUT_FLOATING
#define PA1_IN_NP	PA1, INPUT_FLOATING
#define PA2_IN_NP	PA2, INPUT_FLOATING
#define PA3_IN_NP	PA3, INPUT_FLOATING
#define PA4_IN_NP	PA4, INPUT_FLOATING
#define PA5_IN_NP	PA5, INPUT_FLOATING
#define PA6_IN_NP	PA6, INPUT_FLOATING
#define PA7_IN_NP	PA7, INPUT_FLOATING
#define PA8_IN_NP	PA8, INPUT_FLOATING
#define PA9_IN_NP	PA9, INPUT_FLOATING
#define PA10_IN_NP	PA10, INPUT_FLOATING
#define PA11_IN_NP	PA11, INPUT_FLOATING
#define PA12_IN_NP	PA12, INPUT_FLOATING
//#define PA13_IN_NP	PA13, INPUT_FLOATING
//#define PA14_IN_NP	PA14, INPUT_FLOATING
#define PA15_IN_NP	PA15, INPUT_FLOATING

// Pulled up

#define PA0_IN_PU	PA0, INPUT_PU
#define PA1_IN_PU	PA1, INPUT_PU
#define PA2_IN_PU	PA2, INPUT_PU
#define PA3_IN_PU	PA3, INPUT_PU
#define PA4_IN_PU	PA4, INPUT_PU
#define PA5_IN_PU	PA5, INPUT_PU
#define PA6_IN_PU	PA6, INPUT_PU
#define PA7_IN_PU	PA7, INPUT_PU
#define PA8_IN_PU	PA8, INPUT_PU
#define PA9_IN_PU	PA9, INPUT_PU
#define PA10_IN_PU	PA10, INPUT_PU
#define PA11_IN_PU	PA11, INPUT_PU
#define PA12_IN_PU	PA12, INPUT_PU
//#define PA13_IN_PU	PA13, INPUT_PU
//#define PA14_IN_PU	PA14, INPUT_PU
#define PA15_IN_PU	PA15, INPUT_PU

// Pulled down

#define PA0_IN_PD	PA0, INPUT_PD
#define PA1_IN_PD	PA1, INPUT_PD
#define PA2_IN_PD	PA2, INPUT_PD
#define PA3_IN_PD	PA3, INPUT_PD
#define PA4_IN_PD	PA4, INPUT_PD
#define PA5_IN_PD	PA5, INPUT_PD
#define PA6_IN_PD	PA6, INPUT_PD
#define PA7_IN_PD	PA7, INPUT_PD
#define PA8_IN_PD	PA8, INPUT_PD
#define PA9_IN_PD	PA9, INPUT_PD
#define PA10_IN_PD	PA10, INPUT_PD
#define PA11_IN_PD	PA11, INPUT_PD
#define PA12_IN_PD	PA12, INPUT_PD
//#define PA13_IN_PD	PA13, INPUT_PD
//#define PA14_IN_PD	PA14, INPUT_PD
#define PA15_IN_PD	PA15, INPUT_PD


/********************************/
/*	Plain outputs		*/
/********************************/

// Push pull

#define PA0_OUT_PP	PA0, OUTPUT_PP
#define PA1_OUT_PP	PA1, OUTPUT_PP
#define PA2_OUT_PP	PA2, OUTPUT_PP
#define PA3_OUT_PP	PA3, OUTPUT_PP
#define PA4_OUT_PP	PA4, OUTPUT_PP
#define PA5_OUT_PP	PA5, OUTPUT_PP
#define PA6_OUT_PP	PA6, OUTPUT_PP
#define PA7_OUT_PP	PA7, OUTPUT_PP
#define PA8_OUT_PP	PA8, OUTPUT_PP
#define PA9_OUT_PP	PA9, OUTPUT_PP
#define PA10_OUT_PP	PA10, OUTPUT_PP
#define PA11_OUT_PP	PA11, OUTPUT_PP
#define PA12_OUT_PP	PA12, OUTPUT_PP
#define PA13_OUT_PP	PA13, OUTPUT_PP
#define PA14_OUT_PP	PA14, OUTPUT_PP
#define PA15_OUT_PP	PA15, OUTPUT_PP

// Open drain

#define PA0_OUT_OD	PA0, OUTPUT_OD
#define PA1_OUT_OD	PA1, OUTPUT_OD
#define PA2_OUT_OD	PA2, OUTPUT_OD
#define PA3_OUT_OD	PA3, OUTPUT_OD
#define PA4_OUT_OD	PA4, OUTPUT_OD
#define PA5_OUT_OD	PA5, OUTPUT_OD
#define PA6_OUT_OD	PA6, OUTPUT_OD
#define PA7_OUT_OD	PA7, OUTPUT_OD
#define PA8_OUT_OD	PA8, OUTPUT_OD
#define PA9_OUT_OD	PA9, OUTPUT_OD
#define PA10_OUT_OD	PA10, OUTPUT_OD
#define PA11_OUT_OD	PA11, OUTPUT_OD
#define PA12_OUT_OD	PA12, OUTPUT_OD
//#define PA13_OUT_OD	PA13, OUTPUT_OD
//#define PA14_OUT_OD	PA14, OUTPUT_OD
#define PA15_OUT_OD	PA15, OUTPUT_OD

/********************************/
/*	Analog inputs		*/
/********************************/

#define PA0_IN_A		PA0, INPUT_ANALOG
#define PA1_IN_A		PA1, INPUT_ANALOG
#define PA2_IN_A		PA2, INPUT_ANALOG
#define PA3_IN_A		PA3, INPUT_ANALOG
#define PA4_IN_A		PA4, INPUT_ANALOG
#define PA5_IN_A		PA5, INPUT_ANALOG
#define PA6_IN_A		PA6, INPUT_ANALOG
#define PA7_IN_A		PA7, INPUT_ANALOG

//*****************************
//	Port B
//*****************************
/********************************/
/*	Plain inputs		*/
/********************************/

// No PUPD

#define PB0_IN_PD	PB0, INPUT_PD
#define PB1_IN_PD	PB1, INPUT_PD
//#define PB2_IN_PD	PB2, INPUT_PD
#define PB3_IN_PD	PB3, INPUT_PD
#define PB4_IN_PD	PB4, INPUT_PD
#define PB5_IN_PD	PB5, INPUT_PD
#define PB6_IN_PD	PB6, INPUT_PD
#define PB7_IN_PD	PB7, INPUT_PD
#define PB8_IN_PD	PB8, INPUT_PD
#define PB9_IN_PD	PB9, INPUT_PD
#define PB10_IN_PD	PB10, INPUT_PD
#define PB11_IN_PD	PB11, INPUT_PD
#define PB12_IN_PD	PB12, INPUT_PD
#define PB13_IN_PD	PB13, INPUT_PD
#define PB14_IN_PD	PB14, INPUT_PD
#define PB15_IN_PD	PB15, INPUT_PD

// Pulled up

#define PB0_IN_NP	PB0, INPUT_FLOATING
#define PB1_IN_NP	PB1, INPUT_FLOATING
//#define PB2_IN_NP	PB2, INPUT_FLOATING
#define PB3_IN_NP	PB3, INPUT_FLOATING
#define PB4_IN_NP	PB4, INPUT_FLOATING
#define PB5_IN_NP	PB5, INPUT_FLOATING
#define PB6_IN_NP	PB6, INPUT_FLOATING
#define PB7_IN_NP	PB7, INPUT_FLOATING
#define PB8_IN_NP	PB8, INPUT_FLOATING
#define PB9_IN_NP	PB9, INPUT_FLOATING
#define PB10_IN_NP	PB10, INPUT_FLOATING
#define PB11_IN_NP	PB11, INPUT_FLOATING
#define PB12_IN_NP	PB12, INPUT_FLOATING
#define PB13_IN_NP	PB13, INPUT_FLOATING
#define PB14_IN_NP	PB14, INPUT_FLOATING
#define PB15_IN_NP	PB15, INPUT_FLOATING

// Pulled down

#define PB0_IN_PU	PB0, INPUT_PU
#define PB1_IN_PU	PB1, INPUT_PU
//#define PB2_IN_PU	PB2, INPUT_PU
#define PB3_IN_PU	PB3, INPUT_PU
#define PB4_IN_PU	PB4, INPUT_PU
#define PB5_IN_PU	PB5, INPUT_PU
#define PB6_IN_PU	PB6, INPUT_PU
#define PB7_IN_PU	PB7, INPUT_PU
#define PB8_IN_PU	PB8, INPUT_PU
#define PB9_IN_PU	PB9, INPUT_PU
#define PB10_IN_PU	PB10, INPUT_PU
#define PB11_IN_PU	PB11, INPUT_PU
#define PB12_IN_PU	PB12, INPUT_PU
#define PB13_IN_PU	PB13, INPUT_PU
#define PB14_IN_PU	PB14, INPUT_PU
#define PB15_IN_PU	PB15, INPUT_PU

#define PB0_IN_PD	PB0, INPUT_PD
#define PB1_IN_PD	PB1, INPUT_PD
//#define PB2_IN_PD	PB2, INPUT_PD
#define PB3_IN_PD	PB3, INPUT_PD
#define PB4_IN_PD	PB4, INPUT_PD
#define PB5_IN_PD	PB5, INPUT_PD
#define PB6_IN_PD	PB6, INPUT_PD
#define PB7_IN_PD	PB7, INPUT_PD
#define PB8_IN_PD	PB8, INPUT_PD
#define PB9_IN_PD	PB9, INPUT_PD
#define PB10_IN_PD	PB10, INPUT_PD
#define PB11_IN_PD	PB11, INPUT_PD
#define PB12_IN_PD	PB12, INPUT_PD
#define PB13_IN_PD	PB13, INPUT_PD
#define PB14_IN_PD	PB14, INPUT_PD
#define PB15_IN_PD	PB15, INPUT_PD

/********************************/
/*	Plain outputs		*/
/********************************/

// Push pull

#define PB0_OUT_PP	PB0, OUTPUT_PP
#define PB1_OUT_PP	PB1, OUTPUT_PP
//#define PB2_OUT_PP	PB2, OUTPUT_PP
#define PB3_OUT_PP	PB3, OUTPUT_PP
#define PB4_OUT_PP	PB4, OUTPUT_PP
#define PB5_OUT_PP	PB5, OUTPUT_PP
#define PB6_OUT_PP	PB6, OUTPUT_PP
#define PB7_OUT_PP	PB7, OUTPUT_PP
#define PB8_OUT_PP	PB8, OUTPUT_PP
#define PB9_OUT_PP	PB9, OUTPUT_PP
#define PB10_OUT_PP	PB10, OUTPUT_PP
#define PB11_OUT_PP	PB11, OUTPUT_PP
#define PB12_OUT_PP	PB12, OUTPUT_PP
#define PB13_OUT_PP	PB13, OUTPUT_PP
#define PB14_OUT_PP	PB14, OUTPUT_PP
#define PB15_OUT_PP	PB15, OUTPUT_PP

// Open drain

#define PB0_OUT_OD	PB0, OUTPUT_OD
#define PB1_OUT_OD	PB1, OUTPUT_OD
//#define PB2_OUT_OD	PB2, OUTPUT_OD
#define PB3_OUT_OD	PB3, OUTPUT_OD
#define PB4_OUT_OD	PB4, OUTPUT_OD
#define PB5_OUT_OD	PB5, OUTPUT_OD
#define PB6_OUT_OD	PB6, OUTPUT_OD
#define PB7_OUT_OD	PB7, OUTPUT_OD
#define PB8_OUT_OD	PB8, OUTPUT_OD
#define PB9_OUT_OD	PB9, OUTPUT_OD
#define PB10_OUT_OD	PB10, OUTPUT_OD
#define PB11_OUT_OD	PB11, OUTPUT_OD
#define PB12_OUT_OD	PB12, OUTPUT_OD
#define PB13_OUT_OD	PB13, OUTPUT_OD
#define PB14_OUT_OD	PB14, OUTPUT_OD
#define PB15_OUT_OD	PB15, OUTPUT_OD

/********************************/
/*	Analog inputs		*/
/********************************/

#define PB0_IN_A		PB0, INPUT_ANALOG

//*****************************
//	Port C
//*****************************
/********************************/
/*	Plain inputs		*/
/********************************/

// No PUPD

#define PC13_IN_NP	PC13, INPUT_FLOATING
#define PC14_IN_NP	PC14, INPUT_FLOATING
#define PC15_IN_NP	PC15, INPUT_FLOATING

// Pulled up

#define PC13_IN_PU	PC13, INPUT_PU
#define PC14_IN_PU	PC14, INPUT_PU
#define PC15_IN_PU	PC15, INPUT_PU

// Pulled down

#define PC13_IN_PD	PC13, INPUT_PD
#define PC14_IN_PD	PC14, INPUT_PD
#define PC15_IN_PD	PC15, INPUT_PD

/********************************/
/*	Plain outputs		*/
/********************************/

// Push pull

#define PC13_OUT_PP	PC13, OUTPUT_PP
#define PC14_OUT_PP	PC14, OUTPUT_PP
#define PC15_OUT_PP	PC15, OUTPUT_PP

// Open drain

#define PC13_OUT_OD	PC13, OUTPUT_OD
#define PC14_OUT_OD	PC14, OUTPUT_OD
#define PC15_OUT_OD	PC15, OUTPUT_OD

//*****************************
//	Peripherals
//*****************************

#define PA11_CANRX_INIT		PA11, INPUT_FLOATING
#define PA12_CANTX_INIT		PA12, OUTPUT_AFPP

#define PA9_UART1TX		PA9, OUTPUT_AFPP
#define PA10_UART1RX		PA10, INPUT_FLOATING

#define PA2_UART2TX		PA2, OUTPUT_AFPP
#define PA3_UART2RX		PA3, INPUT_FLOATING

#define PB10_UART3TX		PB10, OUTPUT_AFPP
#define PB11_UART3RX		PB11, INPUT_FLOATING

#define PA8_TIM1CH1_O		PA8, OUTPUT_AFPP
#define PA9_TIM1CH2_O		PA9, OUTPUT_AFPP
#define PA10_TIM1CH3_O		PA10, OUTPUT_AFPP
#define PA11_TIM1CH4_O		PA11, OUTPUT_AFPP

#define PA0_TIM2CH1_O		PA0, OUTPUT_AFPP
#define PA1_TIM2CH2_O		PA1, OUTPUT_AFPP
#define PA2_TIM2CH3_O		PA2, OUTPUT_AFPP
#define PA3_TIM2CH4_O		PA3, OUTPUT_AFPP

#define PA0_TIM2CH1_I		PA0, GPIO_Mode_AF_NP
#define PA1_TIM2CH2_I		PA1, GPIO_Mode_AF_NP
#define PA2_TIM2CH3_I		PA2, GPIO_Mode_AF_NP
#define PA3_TIM2CH4_I		PA3, GPIO_Mode_AF_NP

#define PA6_TIM3CH1_O		PA6, OUTPUT_AFPP
#define PA7_TIM3CH2_O		PA7, OUTPUT_AFPP
#define PB0_TIM3CH3_O		PB0, OUTPUT_AFPP
#define PB1_TIM3CH4_O		PB1, OUTPUT_AFPP

#define PA6_TIM3CH1_I		PA6, GPIO_Mode_AF_NP
#define PA7_TIM3CH2_I		PA7, GPIO_Mode_AF_NP
#define PB0_TIM3CH3_I		PB0, GPIO_Mode_AF_NP
#define PB1_TIM3CH4_I		PB1, GPIO_Mode_AF_NP

#define PB6_TIM4CH1_O		PB6, OUTPUT_AFPP
#define PB9_TIM4CH2_O		PB7, OUTPUT_AFPP
#define PB8_TIM4CH3_O		PB8, GPIO_Mode_IN_PP
#define PB9_TIM4CH4_O		PB9, OUTPUT_AFPP

#define PB6_TIM4CH1_I		PB6, GPIO_Mode_AF_NP
#define PB9_TIM4CH2_I		PB7, GPIO_Mode_AF_NP
#define PB8_TIM4CH3_I		PB8, INPUT_FLOATING
#define PB9_TIM4CH4_I		PB9, GPIO_Mode_AF_NP

#define PA4_SPI1NSS		PA4, GPIO_Mode_IN_PP
#define PA5_SPI1CK		PA5, OUTPUT_AFPP
#define PA6_SPI1MISO		PA6, GPIO_Mode_AF_NP
#define PA7_SPI1MOSI		PA7, OUTPUT_AFPP

#define PB12_SPI2NSS_PIN	PB12
#define PB12_SPI2NSS		PB12_SPI2NSS_PIN, OUTPUT_PP
#define PB13_SPI2CK		PB13, OUTPUT_AFPP
#define PB14_SPI2MISO		PB14, INPUT_PU
#define PB15_SPI2MOSI		PB15, OUTPUT_AFPP

#define SERIAL1			USART1
#define SERIAL2			USART2
#define SERIAL3			USART3

#define SERIAL1_PA9_TX		PA9_UART1TX
#define SERIAL1_PA10_RX		PA10_UART1RX
#define SERIAL2_PA2_TX		PA2_UART2TX
#define SERIAL2_PA3_RX		PA3_UART2RX
#define SERIAL3_PB10_TX		PB10_UART3TX
#define SERIAL3_PB11_RX		PB11_UART3RX

#define PA8_MCO			PA8, OUTPUT_AFPP

#define PA0_ADC			PA0_IN_A
#define PA1_ADC			PA1_IN_A
#define PA2_ADC			PA2_IN_A
#define PA3_ADC			PA3_IN_A
#define PA4_ADC			PA4_IN_A
#define PA5_ADC			PA5_IN_A
#define PA6_ADC			PA6_IN_A
#define PA7_ADC			PA7_IN_A
#define PB0_ADC			PB0_IN_A

#define ADC_CH0_IO		PA0_ADC
#define ADC_CH1_IO		PA1_ADC
#define ADC_CH2_IO		PA2_ADC
#define ADC_CH3_IO		PA3_ADC
#define ADC_CH4_IO		PA4_ADC
#define ADC_CH5_IO		PA5_ADC
#define ADC_CH6_IO		PA6_ADC
#define ADC_CH7_IO		PA7_ADC
#define ADC_CH9_IO		PB0_ADC

#define ADC1_CH0		ADC1,0
#define ADC1_CH1		ADC1,1
#define ADC1_CH2		ADC1,2
#define ADC1_CH3		ADC1,3
#define ADC1_CH4		ADC1,4
#define ADC1_CH5		ADC1,5
#define ADC1_CH6		ADC1,6
#define ADC1_CH7		ADC1,7
#define ADC1_CH8		ADC1,8

#define ADC2_CH0		ADC2,0
#define ADC2_CH1		ADC2,1
#define ADC2_CH2		ADC2,2
#define ADC2_CH3		ADC2,3
#define ADC2_CH4		ADC2,4
#define ADC2_CH5		ADC2,5
#define ADC2_CH6		ADC2,6
#define ADC2_CH7		ADC2,7
#define ADC2_CH8		ADC2,8

#define PWM1			TIM1
#define PWM2			TIM2
#define PWM3			TIM3
#define PWM4			TIM4

#define TIM_uS			TIM4

#include "AppIODefs.h"

#define MCO_BITS (7<<27)
#define MCO_SYS (4<<24)
#define MCO_HSI (5<<24)
#define MCO_HSE (6<<24)
#define MCO_PLL (7<<24)

#define TIM1CLK Bit(11)		//APB2
#define TIM2CLK Bit(0)		//APB1
#define TIM3CLK Bit(1)		//APB1
#define TIM4CLK Bit(2)		//APB1

typedef TIM_TypeDef * t_PWMBase;

// system functions
void SysTickHook(void);
void PlatformInit(void);
void InitGPIOClocks(void);
void InitClockOutput(uint32_t Output);
uint32_t InitTimerClock(TIM_TypeDef *TIM);
void InituSCounter(void);
uint16_t ReaduSCounter(void);

// Generic IO funcs - can use the above I/O defs
void InitIOBit(GPIO_TypeDef* Port, uint8_t Pin, uint8_t Mode, bool OType, uint8_t Pull);
void WriteIOBit(GPIO_TypeDef* Port, uint16_t Pin, bool bState);
void WriteIOVect(GPIO_TypeDef* Port, uint16_t Vect, uint16_t Mask);
bool ReadIOBit(GPIO_TypeDef* Port, uint16_t Pin);
void ToggleIOBit(GPIO_TypeDef* Port, uint16_t Pin);

#endif
