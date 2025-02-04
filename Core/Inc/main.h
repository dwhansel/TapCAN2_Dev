/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32h7xx_hal.h"

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

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define TIM7_CLK_SRC APB1
#define WIFI_ENABLE_Pin GPIO_PIN_3
#define WIFI_ENABLE_GPIO_Port GPIOE
#define TOUCH_INT_Pin GPIO_PIN_13
#define TOUCH_INT_GPIO_Port GPIOC
#define HWID0_Pin GPIO_PIN_14
#define HWID0_GPIO_Port GPIOF
#define HWID1_Pin GPIO_PIN_15
#define HWID1_GPIO_Port GPIOF
#define WIFI_BOOT_Pin GPIO_PIN_8
#define WIFI_BOOT_GPIO_Port GPIOE
#define GPI2_3V_Pin GPIO_PIN_8
#define GPI2_3V_GPIO_Port GPIOD
#define GPO3_3V_Pin GPIO_PIN_9
#define GPO3_3V_GPIO_Port GPIOD
#define GPO4_3V_Pin GPIO_PIN_11
#define GPO4_3V_GPIO_Port GPIOD
#define GPO5_3V_Pin GPIO_PIN_13
#define GPO5_3V_GPIO_Port GPIOD
#define CAN1_TERM_EN_Pin GPIO_PIN_14
#define CAN1_TERM_EN_GPIO_Port GPIOD
#define CAN2_TERM_EN_Pin GPIO_PIN_15
#define CAN2_TERM_EN_GPIO_Port GPIOD
#define TOUCH_RSTn_Pin GPIO_PIN_8
#define TOUCH_RSTn_GPIO_Port GPIOG
#define BEEPBEEP_Pin GPIO_PIN_8
#define BEEPBEEP_GPIO_Port GPIOA
#define GPI1_3V_Pin GPIO_PIN_3
#define GPI1_3V_GPIO_Port GPIOD
#define LCD_RSTn_Pin GPIO_PIN_9
#define LCD_RSTn_GPIO_Port GPIOG

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
