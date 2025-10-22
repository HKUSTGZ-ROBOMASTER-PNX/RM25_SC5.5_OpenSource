/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "bsp_adc.h"
#include "bsp_20k_tim.h"
#include "bsp_led.h"
#include "bsp_hrtim.h"
#include "bsp_can.h"
#include "BuckBoostController.hpp"

void SystemClock_Config(void);

BuckBoostController buckBoostController;

extern "C" void FDCAN1_IT0_IRQHandler(void);

void FDCAN1_IT0_IRQHandler(void)
{
  HAL_FDCAN_IRQHandler(&hfdcan1);
}

FDCAN_RxHeaderTypeDef RxHeader1;
uint8_t can_rxbuf2[8];

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
  if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &RxHeader1, can_rxbuf2) != HAL_OK)
  {
    Error_Handler();
  }

  if (hfdcan == &hfdcan1)
  {
    if (RxHeader1.Identifier == 0xC5)
    {
      buckBoostController.supercap_set_packet.Receive(can_rxbuf2);
    }
  }
}

extern "C" void TIM8_UP_IRQHandler(void)
{
  buckBoostController.Update();
  LL_TIM_ClearFlag_UPDATE(TIM8);
}

extern "C" void SysTick_Handler(void)
{
  if(buckBoostController.supercap_fdb_packet.m_send_flag==true)
  {
    buckBoostController.supercap_fdb_packet.Send();
  }
}

uint32_t dma_count = 0;

extern "C" void DMA1_Channel1_IRQHandler(void)
{
	dma_count++;
  buckBoostController.multimeter.Calu();
  LL_DMA_ClearFlag_TC1(DMA1);
}

extern "C" void DMA1_Channel2_IRQHandler(void)
{
  LL_DMA_ClearFlag_TC2(DMA1);
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_2);

  NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 15, 0));

  LL_PWR_DisableUCPDDeadBattery();
  SystemClock_Config();

  for (int i = 0; i < 1000;)
  {
    i++;
  }

  bsp_20k_tim_init();
  bsp_led_init();
  bsp_can1_init();

  SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk; // Enable SysTick Interrupt
  SysTick->LOAD = 100000000 / 1000;
  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; // Enable SysTick

  buckBoostController.Init();

  while (1)
  {
  }
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_3);
  while (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_3)
  {
  }

  LL_RCC_HSE_Enable();
  /* Wait till HSE is ready */
  while (LL_RCC_HSE_IsReady() != 1)
  {
  }

  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_2, 50, LL_RCC_PLLR_DIV_2);
  LL_RCC_PLL_EnableDomain_SYS();
  LL_RCC_PLL_Enable();
  /* Wait till PLL is ready */
  while (LL_RCC_PLL_IsReady() != 1)
  {
  }

  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_2);
  /* Wait till System clock is ready */
  while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  }

  /* Insure 1us transition state at intermediate medium speed clock*/
  for (__IO uint32_t i = (100 >> 1); i != 0; i--)
    ;

  /* Set AHB prescaler*/
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

  LL_Init1msTick(100000000);

  LL_SetSystemCoreClock(100000000);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
