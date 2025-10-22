#include "bsp_led.h"
#include "stm32g4xx_ll_gpio.h"
#include "stm32g4xx_ll_rcc.h"
#include "stm32g4xx_ll_bus.h"
#include "stm32g474xx.h"

void bsp_led_init(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);

    GPIO_InitStruct.Pin = LL_GPIO_PIN_13;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
		GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    LL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void bsp_led_on(void)
{
    LL_GPIO_SetOutputPin(GPIOC,LL_GPIO_PIN_13);
}

void bsp_led_off(void)
{
    LL_GPIO_ResetOutputPin(GPIOC,LL_GPIO_PIN_13);
}

void bsp_led_toggle(void)
{
    LL_GPIO_TogglePin(GPIOC,LL_GPIO_PIN_13);
}
