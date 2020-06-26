/*
 * Platform_UART.h
 *
 *  Created on: 1 May 2020
 *      Author: AmlodipineFiend
 */

#ifndef APPINC_PLATFORM_UART_H_
#define APPINC_PLATFORM_UART_H_

#define UART_FORMATBITS 0x00ff
#define UART_BUFFBITS 0xff00
#define DATA_8N1 0
#define DATA_9N1 1
#define DATA_8E1 2
#define DATA_8O1 3

#define UART_DIRECT 0 // 1 for unbuffered direct UART- for really memory- constrained applications

#define COM1_BUFF_SIZE 256
#define COM2_BUFF_SIZE 256
#define COM3_BUFF_SIZE 256

void InitUART(USART_TypeDef *UART, uint32_t BaudRate, uint16_t Format);
bool UART_TxRdy( USART_TypeDef *UART);
uint16_t UART_RxRdy(USART_TypeDef *UART);
void WriteUART(USART_TypeDef *UART, uint16_t c);
uint16_t ReadUART(USART_TypeDef *UART);

#endif /* APPINC_PLATFORM_UART_H_ */
