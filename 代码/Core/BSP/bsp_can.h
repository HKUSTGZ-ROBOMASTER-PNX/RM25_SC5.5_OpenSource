/**
  ****************************************************************
  * @file    bsp_can.h
  * @author  TofuLemon(5thzhengjiarandom@gmail.com)
  * @date    2020-Jan-13
  ****************************************************************
  */

#ifndef BSP_CAN_H_
#define BSP_CAN_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "stm32g4xx_hal.h"
#include "stm32g4xx_ll_rcc.h"
	
extern FDCAN_HandleTypeDef hfdcan1;

uint8_t bsp_can1_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSP_CAN_H_ */
