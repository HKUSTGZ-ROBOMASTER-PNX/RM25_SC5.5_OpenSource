#ifndef BSP_LED_H
#define BSP_LED_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
	
void bsp_led_init(void);
void bsp_led_on(void);
void bsp_led_off(void);
void bsp_led_toggle(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
