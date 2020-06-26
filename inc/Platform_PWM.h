/*
 * PlatformPWM.h
 *
 *  Created on: 2 May 2020
 *      Author: AmlodipineFiend
 */

#ifndef APPINC_PLATFORM_PWM_H_
#define APPINC_PLATFORM_PWM_H_

void InitPWMTimebase(TIM_TypeDef *TIM, uint32_t Frequency, uint32_t Resolution);
void InitPWMChannel(TIM_TypeDef *TIM, uint8_t Channel, uint16_t InitValue, bool Inverted);
void PWM_Start(TIM_TypeDef *TIM);
void PWM_Stop(TIM_TypeDef *TIM);
void SetPWM(TIM_TypeDef *TIM, uint8_t Channel, uint16_t Value);

#endif /* APPINC_PLATFORM_PWM_H_ */
