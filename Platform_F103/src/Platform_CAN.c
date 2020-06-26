
/*
 * CAN.c
 *
 *  Created on: 10 Aug 2015
 *      Author: Doktor Faustus
 */
#include "Platform.h"
#include "Globals.h"


void CAN_NVIC_Config(void);

#define CANCLK Bit(25)
#define CAN_SJW_4tq 3
#define CAN_BS1_11tq 10
#define CAN_BS2_4tq 3
#define CAN_Prescaler 18

#define CAN_IT_FMP0 Bit(1)

#define NCANRXMSGS 8
t_CANRXMsg CAN1RxMessage[NCANRXMSGS];
uint32_t CAN1RxIn;
uint32_t CAN1RxOut;

void InitCAN(void)
{
RCC->APB1RSTR |= CANCLK;
RCC->APB1RSTR &= ~CANCLK ;
RCC->APB1ENR |= CANCLK ;


CAN1->MCR &= ~Bit(1);
CAN1->MCR |= CAN_MCR_INRQ;
while( !(CAN1->MSR & CAN_MSR_INAK) )
  {
  }

  /* CAN Baudrate = 125kBps*/
CAN1->BTR = (CAN_SJW_4tq << 24) | (CAN_BS1_11tq << 16) | (CAN_BS2_4tq << 20) |  (CAN_Prescaler-1);
CAN1->MCR &= ~CAN_MCR_INRQ;
 while( CAN1->MSR & CAN_MSR_INAK)
  {
  }
CAN1->FMR |= 1;
CAN1->FM1R = 0;
CAN1->FS1R = 1;
CAN1->FA1R = 1;
CAN1->sFilterRegister[0].FR1 = 0;
CAN1->sFilterRegister[0].FR2 = 0;
CAN1->FMR &= ~1;

CAN1RxIn = 0;
CAN1RxOut = 0;
CAN1->IER |= CAN_IT_FMP0;

NVIC->ISER[0] |= Bit(20);
}


uint8_t CAN_Tx(bool ext, uint32_t ID, bool remote, uint8_t *msg, uint8_t len)
{
uint32_t MailBox;
uint8_t i;
uint32_t tmpmsg;

MailBox = CAN1->TSR;

if( MailBox & Bit(26))
 {
 MailBox = 1;
 }
else if( MailBox & Bit(27))
 {
 MailBox = 2;
 }
else if( MailBox & Bit(28))
 {
 MailBox = 3;
 }
else
 {
 MailBox = 0;
 }

if (MailBox)
 {
 MailBox -= 1;
 CAN1->sTxMailBox[MailBox].TIR = 0;
 CAN1->sTxMailBox[MailBox].TDTR = 0;

 if( remote)
  {
  CAN1->sTxMailBox[MailBox].TIR |= Bit(1);
  }

  /* Set up the Id */
 if (!ext)
  {
  CAN1->sTxMailBox[MailBox].TIR |= (ID << 21) & 0xffe00000;
  }
 else
  {
  CAN1->sTxMailBox[MailBox].TIR |= ((ID << 3) & 0xfffffff8) | Bit(2);
  }
 CAN1->sTxMailBox[MailBox].TDTR = len & 0x0f;

  /* Set up the data field */
 tmpmsg = msg[0] | (msg[1]<<8) | (msg[2]<<16) | (msg[3]<<24) ;
 CAN1->sTxMailBox[MailBox].TDLR = tmpmsg;
 tmpmsg = msg[4] | (msg[5]<<8) | (msg[6]<<16) | (msg[7]<<24) ;
 CAN1->sTxMailBox[MailBox].TDHR = tmpmsg;
 CAN1->sTxMailBox[MailBox].TIR |= 1;
 }
return MailBox;
}


t_CANRXMsg *CAN_Rx(void)
{
t_CANRXMsg *msg = 0;
if( CAN1RxIn != CAN1RxOut)
 {
 msg = &CAN1RxMessage[CAN1RxOut];
 CAN1RxOut++;
 if( CAN1RxOut > (NCANRXMSGS-1))
  {
  CAN1RxOut = 0;
  }
 }
return msg;
}

#define NMSGMASK 3
#define RELEASEMSG Bit(5)

void USB_LP_CAN1_RX0_IRQHandler(void)
{
uint32_t i;
uint8_t *dptr;
union {uint8_t c[4]; uint32_t i;} dtmp;


t_CANRXMsg *msg = &CAN1RxMessage[CAN1RxIn];

msg->Len = CAN1->sFIFOMailBox[0].RDTR & 0x0f;

if (CAN1->sFIFOMailBox[0].RIR & Bit(1))
 {
 msg->Remote = true;
 }
else
 {
 msg->Remote = false;
 }

if (CAN1->sFIFOMailBox[0].RIR & Bit(2))
 {
 msg->Extended = true;
 msg->ID = CAN1->sFIFOMailBox[0].RIR >> 3;
 }
else
 {
 msg->Extended = false;
 msg->ID = CAN1->sFIFOMailBox[0].RIR >> 21;
 }

dptr = &msg->Data[0];

dtmp.i = CAN1->sFIFOMailBox[0].RDLR;
for(i=0; i<4; i++)
 {
 dptr[i] =  dtmp.c[i];
 }
dtmp.i = CAN1->sFIFOMailBox[0].RDHR;
for(i=0; i<4; i++)
 {
 dptr[i+4] =  dtmp.c[i];
 }

CAN1RxIn++;
if( CAN1RxIn > (NCANRXMSGS-1))
 {
 CAN1RxIn = 0;
 }
CAN1->RF0R |= RELEASEMSG;
}

uint8_t CAN_Status(void)
{
uint8_t Sts = 0;
if( !(CAN1->TSR & Bit(1)))
 {
 Sts |= 1;
 }
if( !(CAN1->TSR & Bit(9)))
 {
 Sts |= 2;
 }
if( !(CAN1->TSR & Bit(17)))
 {
 Sts |= 4;
 }
}

void CAN_Abort( uint8_t mailbox)
{
if( mailbox & 1)
 {
 CAN1->TSR |= Bit(7);
 }
if( mailbox & 2)
 {
 CAN1->TSR |= Bit(19);
 }
if( mailbox & 4)
 {
 CAN1->TSR |= Bit(23);
 }
}
