/*
 * Platform_CAN.h
 *
 *  Created on: 6 May 2020
 *      Author: User
 */

#ifndef APPINC_PLATFORM_CAN_H_
#define APPINC_PLATFORM_CAN_H_


typedef struct
{
uint32_t ID;
bool Extended;
bool Remote;
int8_t Len;
uint8_t Data[8];
} t_CANRXMsg;


void InitCAN(void);
t_CANRXMsg *CAN_Rx(void);
uint8_t CAN_Tx(bool ext, uint32_t ID, bool remote, uint8_t *msg, uint8_t len);
uint8_t CAN_Status(void);
void CAN_Abort( uint8_t mailbox);

#endif /* APPINC_PLATFORM_CAN_H_ */
