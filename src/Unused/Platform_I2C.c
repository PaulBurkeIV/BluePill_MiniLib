
#include "platform.h"
#include "Globals.h"

// returns true if the i2c is currently in a transaction
#define I2C_busy(I2Cx) (I2C_ReadRegister(I2Cx, I2C_Register_SR2) & I2C_FLAG_BUSY)

//static uint8_t I2C_Culprit[64];
//static uint8_t I2CCindx=0;

#define TMR_MAX 100 // i2c hangup timer in milliseconds for whole transaction

#define I2C1CLK Bit(21)
#define I2C2CLK Bit(22)

void I2CInit_Master( I2C_TypeDef I2C, uint32_t ClockRate)
{
switch ((uint32_t)I2C )
 {
 case (uint32_t)I2C1:
 			RCC->APB2RSTR |= I2C1CLK ; // Reset the peripheral
 			RCC->APB2RSTR &= ~I2C1CLK ;
 			RCC->APB2ENR |= I2C1CLK ; // Enable clock
 			break;
 case (uint32_t)I2C2:
 			RCC->APB2RSTR |= I2C2CLK ; // Reset the peripheral
 			RCC->APB2RSTR &= ~I2C2CLK ;
 			RCC->APB2ENR |= I2C2CLK ; // Enable clock
 			break;
 }


}

// This is a bodge to recover dynamically if the bus hangs - which it does

static void I2C_Recover(uint8_t Adr, const uint8_t *WhoDunnit)
{
u32 tm = MilliSeconds();
mprintf( "I2C Recovery 0x%02x, from %s\r\n", Adr, WhoDunnit );
while( (MilliSeconds() -tm) < 100)
 ;
SystemReset(WUR_I2C_LOCK);
}

bool I2C_Write(uint8_t Adr, uint8_t* s,  uint16_t nbytes)
{
u32 tmr = MilliSeconds();
    while(I2C_busy(I2C2))
	 if( (MilliSeconds()-tmr) > TMR_MAX)
      {
      I2C_Recover(Adr, "MasterBufferWrite");
      return FALSE;
      }

	/* Send START condition */
	I2C_GenerateSTART(I2C2, ENABLE);
	/* Test on EV5 and clear it */
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))
	 if( (MilliSeconds()-tmr) > TMR_MAX)
      {
      I2C_Recover(Adr, "MasterBufferWrite");
      return FALSE;
      }
	/* Send slave address for write */
	I2C_Send7bitAddress(I2C2,Adr, I2C_Direction_Transmitter);
	/* Test on EV6 and clear it */
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	 if( (MilliSeconds()-tmr) > TMR_MAX)
      {
      iprintf( "\r\nStatus Regs: 0x%08X\r\n", (I2C2->SR1) + ((I2C2->SR2)<<16 ));
      I2C_Recover(Adr, "MasterBufferWrite");
      return FALSE;
      }
	I2C_SendData(I2C2, *src);
	src++;
	nbytes--;
	/* While there is data to be written */
	while (nbytes--)
	{
		while ((I2C_GetLastEvent(I2C2) & 0x04) != 0x04)  /* Poll on BTF */
		 if( (MilliSeconds()-tmr) > TMR_MAX)
		  {
		  I2C_Recover(Adr, "MasterBufferWrite");
		  return FALSE;
		  }
		/* Send the current byte */
		I2C_SendData(I2C2, *src);
		/* Point to the next byte to be written */
		src++;
	}

	/* Test on EV8_2 and clear it, BTF = TxE = 1, DR and shift registers are
	empty */
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	 if( (MilliSeconds()-tmr) > TMR_MAX)
      {
      I2C_Recover(Adr, "MasterBufferWrite");
      return FALSE;
      }
	/* Send STOP condition */
	I2C_GenerateSTOP(I2C2, ENABLE);
	while ((I2C2->CR1&0x200) == 0x200)
	 ;
return TRUE;
}

/**
  * @brief  Receive a buffer of bytes from the slave.
  * @param src: Buffer for bytes to be received from the slave (MUST BE AT LEAST NumBytesToRead long).
  * @param nbytes: Number of bytes to be received from the slave.
  * @retval : None.
  */

bool I2C_Master_BufferRead(uint8_t Adr, uint8_t* dst, uint16_t nbytes)
{
	 u32 tmr = MilliSeconds();

	// 'thread safe'

//iprintf("a");
	while(I2C_busy(I2C2))
	 if( (MilliSeconds()-tmr) > TMR_MAX)
      {
      I2C_Recover(Adr, "Master_BufferRead");
      return FALSE;
      }
	/* Send START condition */
	I2C_GenerateSTART(I2C2, ENABLE);
	/* Test on EV5 and clear it */
//iprintf("b");
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))
     {
	 if( (MilliSeconds()-tmr) > TMR_MAX)
		{
        I2C_Recover(Adr, "Master_BufferRead");
		return FALSE;
		}
     }
    I2C_Send7bitAddress(I2C2,Adr, I2C_Direction_Receiver); 	/* Send slave address for read */
//iprintf("c");
	while (!I2C_GetFlagStatus(I2C2, I2C_FLAG_ADDR))
		{
		if( (MilliSeconds()-tmr) > TMR_MAX)
		  {
		  I2C_Recover(Adr, "Master_BufferRead");
		  return FALSE;
		  }
		 }
//iprintf("d");
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
		{
		if( (MilliSeconds()-tmr) > TMR_MAX)
		  {
          I2C_Recover(Adr, "Master_BufferRead");
		  return FALSE;
		  }
		 }
	/* While there is data to be Read */
//iprintf("e");
	while (nbytes--)
	{
	 while ((I2C_GetLastEvent(I2C2) & 0x04) != 0x04)
		{
		if( (MilliSeconds()-tmr) > TMR_MAX)
		  {
          I2C_Recover(Adr, "Master_BufferRead");
		  return FALSE;
		  }
		 }
	 *dst = I2C_ReceiveData(I2C2);
	 dst++;
	 if( nbytes == 1)		//
       {
		/* Send STOP condition and disable ACK just after 2nd last byte read*/
		/* See Ref Manual p 578 */
		/* Clear ACK */
		I2C_GenerateSTOP(I2C2, ENABLE);
		I2C_AcknowledgeConfig(I2C2, DISABLE);
	   }
	}
while ((I2C2->CR1&0x200) == 0x200)
 ;
I2C_AcknowledgeConfig(I2C2, ENABLE);
//iprintf("f\r\n");
return TRUE;
}

// Return pointer to last 64 I2C addresses that hung bus

//uint8_t *I2CReadOffenders(void)
//{
//return &I2C_Culprit[0];
//}

// Read a "Register" whatever that is

uint16_t I2C_Reg_Read(uint8_t Adr)
{
u32 tmr = MilliSeconds();

	while(I2C_busy(I2C2));
	 if( (MilliSeconds()-tmr) > TMR_MAX)
      {
      I2C_Recover(Adr, "Reg_Read");
      return FALSE;
      }

	u32 RegValue=0;

	  /*----- Reception Phase -----*/
	  /* Send Re-STRAT condition */
	  I2C_GenerateSTART(I2C2, ENABLE);

	  /* Test on EV and clear it */
	  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))  /* EV */
	    if( (MilliSeconds()-tmr) > TMR_MAX)
         {
         I2C_Recover(Adr, "Reg_Read");
         return FALSE;
         }

	  /* Send slave address for read */
	  I2C_Send7bitAddress(I2C2, Adr, I2C_Direction_Receiver);

	  /* Test on EV6 and clear it */
	  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))  /* EV */
	    if( (MilliSeconds()-tmr) > TMR_MAX)
         {
         I2C_Recover(Adr, "Reg_Read");
         return FALSE;
         }
	  /* Test on EV and clear it */
	  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED))  /* EV */
	    if( (MilliSeconds()-tmr) > TMR_MAX)
         {
         I2C_Recover(Adr, "Reg_Read");
         return FALSE;
         }
	  /* Store I2C2 received data high*/
	  RegValue = (uint16_t)(I2C_ReceiveData(I2C2) << 8);
	  /* Disable I2C2 acknowledgement */
	  I2C_AcknowledgeConfig(I2C2, DISABLE);
	  while ((I2C2->CR1&0x200) == 0x200)
	    if( (MilliSeconds()-tmr) > TMR_MAX)
         {
         I2C_Recover(Adr, "Reg_Read");
         return FALSE;
         }
	  /* Send I2C1 STOP Condition */
	  I2C_GenerateSTOP(I2C2, ENABLE);

	  /* Test on EV and clear it */
	  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED))  /* EV */
	    if( (MilliSeconds()-tmr) > TMR_MAX)
         {
         I2C_Recover(Adr, "Reg_Read");
         return FALSE;
         }

	  /* Store I2C1 received data */
	  RegValue |= I2C_ReceiveData(I2C2);

	  /* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
	  	while ((I2C2->CR1&0x200) == 0x200)
	  	 if( (MilliSeconds()-tmr) > TMR_MAX)
          {
          I2C_Recover(Adr, "Reg_Read");
          return FALSE;
          }

	  	/* Enable Acknowledgement to be ready for another reception */
	  	I2C_AcknowledgeConfig(I2C2, ENABLE);
	  return (RegValue);
}

#if 0
uint16_t I2C_Reg_Read(uint8_t Adr)
{
  u32 RegValue=0;

  /*----- Reception Phase -----*/
  /* Send Re-STRAT condition */
  I2C_GenerateSTART(I2C2, ENABLE);

  /* Test on EV and clear it */
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))  /* EV */
  {
  }

  /* Send slave address for read */
  I2C_Send7bitAddress(I2C2, Adr, I2C_Direction_Receiver);

  /* Test on EV6 and clear it */
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))  /* EV */
  {
  }


  /* Test on EV and clear it */
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED))  /* EV */
  {
  }
  //iprintf("5 \r\n");
  /* Store I2C2 received data high*/
  RegValue = (uint16_t)(I2C_ReceiveData(I2C2) << 8);

  while ((I2C_GetLastEvent(I2C2) & 0x0040) != 0x000040); /* Poll on RxNE */
  iprintf("5a \r\n");
  /* Disable I2C2 acknowledgement */
  I2C_AcknowledgeConfig(I2C2, DISABLE);
  while ((I2C2->CR1&0x200) == 0x200);
 //iprintf("5b \r\n");
  /* Send I2C1 STOP Condition */
  I2C_GenerateSTOP(I2C2, ENABLE);

  /* Test on EV and clear it */
  while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED))  /* EV */
  {
  }
  //iprintf("6 \r\n");
  /* Store I2C1 received data */
  RegValue |= I2C_ReceiveData(I2C2);
 // iprintf("7 \r\n");
  /* Return register value */
  //return (RegValue >> 7);
  return (RegValue);
}
#endif



/**
  * @brief  Read a byte from the slave.
  * @param ne.
  * @retval : The read data byte.
  */


uint8_t I2C_Master_BufferReadByte(uint8_t  Adr)
{


u32 tmr = MilliSeconds();
	uint8_t Data;
	__IO uint32_t temp;

	// 'thread safe'
	while(I2C_busy(I2C2))
	    if( (MilliSeconds()-tmr) > TMR_MAX)
         {
         I2C_Recover(Adr, "Master_BufferReadByte");
         return FALSE;
         }



//	 Send START condition
	I2C_GenerateSTART(I2C2, ENABLE);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))
	    if( (MilliSeconds()-tmr) > TMR_MAX)
         {
         I2C_Recover(Adr, "Master_BufferReadByte");
         return FALSE;
         }

//	 Send EEPROM address for read
	I2C_Send7bitAddress(I2C2, Adr, I2C_Direction_Receiver);
//	 Wait until ADDR is set
	while (!I2C_GetFlagStatus(I2C2, I2C_FLAG_ADDR))
	    if( (MilliSeconds()-tmr) > TMR_MAX)
         {
         I2C_Recover(Adr, "Master_BufferReadByte");
         return FALSE;
         }

//	 Clear ACK
	I2C_AcknowledgeConfig(I2C2, DISABLE);
//	__disable_irq();
//	 Clear ADDR flag
	temp = I2C2->SR2;
//	 Program the STOP
	I2C_GenerateSTOP(I2C2, ENABLE);
//	__enable_irq();
	while ((I2C_GetLastEvent(I2C2) & 0x0040) != 0x000040)  //Poll on RxNE
	    if( (MilliSeconds()-tmr) > TMR_MAX)
         {
         I2C_Recover(Adr, "Master_BufferReadByte");
         return FALSE;
         }
//	 Read the data
	Data = I2C_ReceiveData(I2C2);
//	 Make sure that the STOP bit is cleared by Hardware before CR1 write access
	while ((I2C2->CR1&0x200) == 0x200)
	    if( (MilliSeconds()-tmr) > TMR_MAX)
         {
         I2C_Recover(Adr, "Master_BufferReadByte");
         return FALSE;
         }
//	 Enable Acknowledgement to be ready for another reception
	I2C_AcknowledgeConfig(I2C2, ENABLE);

	return(Data);

}

