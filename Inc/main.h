/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void u_main(void);

void USART1_IRQHandler_Calk(void);

void USART2_IRQHandler_Calk(void);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define KEY3_Pin GPIO_PIN_2
#define KEY3_GPIO_Port GPIOC
#define SPI1_RES_Pin GPIO_PIN_4
#define SPI1_RES_GPIO_Port GPIOA
#define SPI1_DC_Pin GPIO_PIN_6
#define SPI1_DC_GPIO_Port GPIOA
#define LD4_Pin GPIO_PIN_0
#define LD4_GPIO_Port GPIOB
#define LD5_Pin GPIO_PIN_1
#define LD5_GPIO_Port GPIOB
#define SPI2_SCS_Pin GPIO_PIN_12
#define SPI2_SCS_GPIO_Port GPIOB
#define W5500_INT_Pin GPIO_PIN_6
#define W5500_INT_GPIO_Port GPIOG
#define W5500_RST_Pin GPIO_PIN_8
#define W5500_RST_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */

#define __noinit__ __attribute__((zero_init))// 变量不初始化为0
#define SRAM__ __attribute__((section ("ExtSram"))) 


/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
