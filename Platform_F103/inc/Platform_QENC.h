/*
 * Platform_QENC.h
 *
 */

#ifndef INC_PLATFORM_QENC_H_
#define INC_PLATFORM_QENC_H_

#define ENC_EDGE_A 1
#define ENC_EDGE_B 2
#define ENC_EDGE_BOTH 3
#define ENC_POL_INV true
#define ENC_POL_NORMAL false

void QuadCounterInit(   TIM_TypeDef *TIMx, uint32_t MaxCount, uint8_t Edges, uint8_t Filter, bool Polarity);
int16_t QuadEncoderRead(TIM_TypeDef *TIMx);

#endif /* INC_PLATFORM_QENC_H_ */
