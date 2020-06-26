/*
 * SPI_23S17.h
 *
 *  Created on: 6 May 2020
 *      Author: User
 */

#ifndef APPINC_SPI_23S17_H_
#define APPINC_SPI_23S17_H_


void MCP23S17_Init (SPI_TypeDef *SPI, GPIO_TypeDef* CS_Port, uint16_t CS_Pin);
uint16_t MCP23S17_Read (SPI_TypeDef *SPI, GPIO_TypeDef* CS_Port, uint16_t CS_Pin, uint16_t chan);
void MCP23S17_Write (SPI_TypeDef *SPI, GPIO_TypeDef* CS_Port, uint16_t CS_Pin, uint16_t chan, uint16_t val);
void MCP23S17DIR_Write (SPI_TypeDef *SPI, GPIO_TypeDef* CS_Port, uint16_t CS_Pin, uint16_t chan, uint16_t mask);
void MCP23S17PU_Write (SPI_TypeDef *SPI, GPIO_TypeDef* CS_Port, uint16_t CS_Pin, uint16_t chan, uint16_t mask);
void MCP23S17POL_Write (SPI_TypeDef *SPI, GPIO_TypeDef* CS_Port, uint16_t CS_Pin, uint16_t chan, uint16_t mask);
uint16_t MCP23S17DIR_Read (SPI_TypeDef *SPI, GPIO_TypeDef* CS_Port, uint16_t CS_Pin, uint16_t chan);

#endif
