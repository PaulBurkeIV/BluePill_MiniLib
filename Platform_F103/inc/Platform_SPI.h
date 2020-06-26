
#ifndef PLATFORM_SPI_H_
#define PLATFORM_SPI_H_

void SPI_Init(SPI_TypeDef *SPI);
void SPI_Init(SPI_TypeDef *SPI);
uint8_t SPI_Read(SPI_TypeDef *SPI);
void SPI_Write(SPI_TypeDef *SPI, uint8_t v);
void SPI_Select_Address(GPIO_TypeDef* port, uint16_t pin, bool state);

#endif
