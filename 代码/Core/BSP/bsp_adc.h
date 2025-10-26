#ifndef BSP_ADC_H
#define BSP_ADC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
	
void bsp_adc1_init(void);
void bsp_adc1_DMA_init(uint16_t* buffer);
void bsp_adc2_init(void);
void bsp_adc2_DMA_init(uint16_t* buffer);
void bsp_adc3_init(void);
void bsp_adc3_DMA_init(uint16_t* buffer);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
