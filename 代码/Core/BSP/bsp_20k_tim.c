#include "bsp_20k_tim.h"

void bsp_20k_tim_init()
{
    LL_TIM_InitTypeDef TIM_InitStruct = {0};
    LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM8);

    NVIC_SetPriority(TIM8_UP_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),1, 0));
    NVIC_EnableIRQ(TIM8_UP_IRQn);

    TIM_InitStruct.Prescaler = 10-1;
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
    TIM_InitStruct.Autoreload = 200-1;
		//TIM_InitStruct.RepetitionCounter = 5-1;
    TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    LL_TIM_Init(TIM8, &TIM_InitStruct);
    LL_TIM_EnableARRPreload(TIM8);
    LL_TIM_SetClockSource(TIM8,LL_TIM_CLOCKSOURCE_INTERNAL);
		LL_TIM_SetUpdateSource(TIM8,LL_TIM_UPDATESOURCE_REGULAR);
		LL_TIM_GenerateEvent_UPDATE(TIM8);

    LL_TIM_OC_EnablePreload(TIM8, LL_TIM_CHANNEL_CH1);

    TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_ACTIVE;
    TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_ENABLE;
    TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.CompareValue = 10;
    TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
    LL_TIM_OC_Init(TIM8, LL_TIM_CHANNEL_CH1, &TIM_OC_InitStruct);

    LL_TIM_SetTriggerOutput(TIM8,LL_TIM_TRGO_CC1IF);
	LL_TIM_EnableAllOutputs(TIM8);
	
	LL_TIM_EnableIT_UPDATE(TIM8);

    LL_TIM_DisableCounter(TIM8);
}


