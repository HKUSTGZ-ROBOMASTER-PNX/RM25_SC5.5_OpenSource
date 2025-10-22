#ifndef BSP_20K_TIM_H
#define BSP_20K_TIM_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "stm32g4xx_ll_tim.h"
#include "stm32g4xx_ll_rcc.h"
#include "stm32g4xx_ll_bus.h"
#include "stm32g474xx.h"
	
void bsp_20k_tim_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
