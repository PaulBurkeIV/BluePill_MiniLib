/*
 * SPI_25LCxx.h
 *
 *  Created on: 6 May 2020
 *      Author: User
 */

#ifndef APPINC_SPI_25LCXX_H_
#define APPINC_SPI_25LCXX_H_

/*
25LC080 	8K 	1,024 x 8 	16
25LC160 	16K 	2,048 x 8 	16
25LC320	 	32K 	4,096 x 8	32
25LC640		64K 	8,192 x 8	32
25LC128		128K	16,384 x 8	64
25LC256		256K 	32,768 x 8	64
*/

#define SPIMEM_PAGE_SIZE_16 	16
#define SPIMEM_PAGE_SIZE_32 	32
#define SPIMEM_PAGE_SIZE_64 	64
#define SPIMEM_PAGE_SIZE 	SPIMEM_PAGE_SIZE_64
#define SPIMEM_SIZE 		32768

void SPI_25LCxx_Init (SPI_TypeDef *SPI, GPIO_TypeDef* CS_Port, uint16_t CS_Pin);
uint8_t Read25LCxxStatus(SPI_TypeDef *SPI, GPIO_TypeDef* Port, uint16_t Pin);
void Write25LCxxEnable(SPI_TypeDef *SPI, GPIO_TypeDef* Port, uint16_t Pin);
uint16_t Write25LCxxByte(SPI_TypeDef *SPI, GPIO_TypeDef* Port, uint16_t Pin, uint8_t v, uint16_t Adr);
uint16_t Read25LCxx(SPI_TypeDef *SPI, GPIO_TypeDef* Port, uint16_t Pin, uint8_t *dst, uint16_t StartAdr, uint16_t nbytes);
void Write25LCxx(SPI_TypeDef *SPI, GPIO_TypeDef* Port, uint16_t Pin, uint8_t *src, uint16_t StartAdr, uint16_t nbytes);



#endif /* APPINC_SPI_25LCXX_H_ */
