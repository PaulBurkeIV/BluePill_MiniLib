/*
 * Platform_ADC.h
 *
 *  Created on: 5 May 2020
 *      Author: User
 */

#ifndef APPINC_PLATFORM_ADC_H_
#define APPINC_PLATFORM_ADC_H_

void ADCInit( ADC_TypeDef *ADC);
void StartADC( ADC_TypeDef *ADC, uint8_t channel);
bool ADCRdy(ADC_TypeDef *ADC);
uint16_t ReadADC(ADC_TypeDef *ADC);

#endif /* APPINC_PLATFORM_ADC_H_ */
