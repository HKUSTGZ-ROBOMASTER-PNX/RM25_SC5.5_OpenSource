#ifndef BSP_HRTIM_H_
#define BSP_HRTIM_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "stm32g4xx_ll_gpio.h"
#include "stm32g4xx_ll_rcc.h"
#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_hrtim.h"
#include "stm32g474xx.h"

void bsp_hrtim_init(void);
void bsp_hrtim_set(int);
void bsp_hrtim_on(void);
void bsp_hrtim_off(void);
void bsp_htrim_burst_on(void);
	
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
