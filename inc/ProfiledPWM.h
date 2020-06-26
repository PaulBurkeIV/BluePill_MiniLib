/*
 * ProfiledPWM.h
 *
 */

#ifndef __PROFPWM_H_
#define __PROFPWM_H_

#ifndef PWM_RATE
#define PWM_RATE 8
#endif

typedef struct {
		t_PWMBase PWM;
		uint16_t PhysChann;
		uint32_t Max;
		uint32_t Min;
		uint32_t Current;
		uint32_t Setpoint;
		uint32_t Rate;
		uint16_t (*Profile)(uint16_t val); // points to user profile function
		} t_ProfPWMChannel;

void ProfPWM_PinInit(GPIO_TypeDef* Port, uint8_t Pin, uint8_t Mode, bool OType, uint8_t Pull);
void ProfPWM_BaseInit(t_PWMBase PWM, uint32_t Freq, uint32_t Resolution);
void ProfPWM_ChannelInit(uint8_t Channel, t_PWMBase PWM, uint16_t PhysChann, uint16_t Min, uint16_t Max, uint16_t InitVal, bool Inv);
void ProfPWM_Set(uint8_t Channel, uint16_t Position, uint32_t Rate);
void ProfPWM_Function(uint8_t Channel, uint16_t (*Profile)(uint16_t));
uint16_t ProfPWM_Read( uint8_t Channel);
void ProfPWM_Halt(uint8_t Channel, uint16_t Value);
void ProfilePWM(void);

#endif /* __PROFPWM_H_ */
