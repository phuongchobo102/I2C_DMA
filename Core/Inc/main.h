/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

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
#define BUTTON1_Pin GPIO_PIN_13
#define BUTTON1_GPIO_Port GPIOC
#define SW_SEL0_Pin GPIO_PIN_14
#define SW_SEL0_GPIO_Port GPIOC
#define SW_SEL1_Pin GPIO_PIN_15
#define SW_SEL1_GPIO_Port GPIOC
#define BUTTON2_Pin GPIO_PIN_0
#define BUTTON2_GPIO_Port GPIOA
#define BUTTON3_Pin GPIO_PIN_2
#define BUTTON3_GPIO_Port GPIOA
#define TLC_LATCH_Pin GPIO_PIN_0
#define TLC_LATCH_GPIO_Port GPIOB
#define TLC_BLANK_Pin GPIO_PIN_1
#define TLC_BLANK_GPIO_Port GPIOB
#define GD_PWR_CTRL_Pin GPIO_PIN_2
#define GD_PWR_CTRL_GPIO_Port GPIOB
#define KVMSW_EN_Pin GPIO_PIN_8
#define KVMSW_EN_GPIO_Port GPIOA
#define BUTTON_EDID_Pin GPIO_PIN_15
#define BUTTON_EDID_GPIO_Port GPIOA
#define VGA4_PLUG_DT_Pin GPIO_PIN_3
#define VGA4_PLUG_DT_GPIO_Port GPIOB
#define VGA3_PLUG_DT_Pin GPIO_PIN_4
#define VGA3_PLUG_DT_GPIO_Port GPIOB
#define BUTTON4_Pin GPIO_PIN_5
#define BUTTON4_GPIO_Port GPIOB
#define VGA2_PLUG_DT_Pin GPIO_PIN_8
#define VGA2_PLUG_DT_GPIO_Port GPIOB
#define VGA2_PLUG_DTB9_Pin GPIO_PIN_9
#define VGA2_PLUG_DTB9_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
