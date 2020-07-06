/*
 * Platform_EVTIM.h
 *
 *  Created on: 28 Jun 2020
 *      Author: User
 */

#ifndef INC_PLATFORM_EVTIM_H_
#define INC_PLATFORM_EVTIM_H_


typedef struct
{
TIM_TypeDef *TIM;
t_DMA_Channel *DMA;
uint16_t Div;
uint16_t *Buff;
uint16_t OutCnt;
uint16_t BufSz;
uint8_t Channel;
} t_EventTimer;

void InitEventTimer(t_EventTimer *EventTimer );
uint16_t EventTimerDataRdy(t_EventTimer *EventTimer);
uint16_t ReadEventTimer(t_EventTimer *EventTimer);
void InvertEventTimer( t_EventTimer *EventTimer );

#endif /* INC_PLATFORM_EVTIM_H_ */
