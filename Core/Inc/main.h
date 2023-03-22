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
#include "elinkswitch.h"
#include "elinkswitch_gpio.h"
#include "flash.h"
#include "vga.h"

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
#define LED_SHDN_Pin GPIO_PIN_1
#define LED_SHDN_GPIO_Port GPIOB
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
#define VGA1_PLUG_DT_Pin GPIO_PIN_9
#define VGA1_PLUG_DT_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define ELS_BUTTON1_GPIO_PORT           BUTTON1_GPIO_Port
#define ELS_BUTTON1_PIN                 BUTTON1_Pin
#define ELS_BUTTON2_GPIO_PORT           BUTTON2_GPIO_Port
#define ELS_BUTTON2_PIN                 BUTTON2_Pin
#define ELS_BUTTON3_GPIO_PORT           BUTTON3_GPIO_Port
#define ELS_BUTTON3_PIN                 BUTTON3_Pin
#define ELS_BUTTON4_GPIO_PORT           BUTTON4_GPIO_Port
#define ELS_BUTTON4_PIN                 BUTTON4_Pin

#define BUTTON_EDID_Pin GPIO_PIN_15
#define BUTTON_EDID_GPIO_Port GPIOA
#define ELS_BUTTON_EDID_GPIO_PORT           BUTTON_EDID_GPIO_Port
#define ELS_BUTTON_EDID_PIN                 BUTTON_EDID_Pin

#define USB_SW_SEL0_GPIO_Port           SW_SEL0_GPIO_Port
#define USB_SW_SEL0_Pin                 SW_SEL0_Pin
#define USB_SW_SEL1_GPIO_Port           SW_SEL1_GPIO_Port
#define USB_SW_SEL1_Pin                 SW_SEL1_Pin

#define ELS_USB_SENSE0_GPIO_PORT
#define ELS_USB_SENSE0_PIN


#define USB1_SENSE_Pin GPIO_PIN_1
#define USB1_SENSE_GPIO_Port GPIOA
#define USB2_SENSE_Pin GPIO_PIN_3
#define USB2_SENSE_GPIO_Port GPIOA
#define USB3_SENSE_Pin GPIO_PIN_4
#define USB3_SENSE_GPIO_Port GPIOA
#define USB4_SENSE_Pin GPIO_PIN_6
#define USB4_SENSE_GPIO_Port GPIOA

#define ELS_USB_SENSE1_GPIO_PORT        USB1_SENSE_GPIO_Port
#define ELS_USB_SENSE1_PIN              USB1_SENSE_Pin
#define ELS_USB_SENSE2_GPIO_PORT        USB2_SENSE_GPIO_Port
#define ELS_USB_SENSE2_PIN              USB2_SENSE_Pin
#define ELS_USB_SENSE3_GPIO_PORT        USB3_SENSE_GPIO_Port
#define ELS_USB_SENSE3_PIN              USB3_SENSE_Pin
#define ELS_USB_SENSE4_GPIO_PORT        USB4_SENSE_GPIO_Port
#define ELS_USB_SENSE4_PIN              USB4_SENSE_Pin

    extern ADC_HandleTypeDef hadc;
// extern uint8_t channelSelect ;
// extern uint8_t edidStatus;
// extern uint8_t vgaStatus[4];
// extern uint8_t usbStatus[4];
// extern uint16_t adcValue[4]; 
#define I2C_TIMING    0x00B01A4B //  0x00A51314

uint8_t get_current_channel();
void set_current_channel(uint8_t channel);
uint8_t get_current_edid();
void set_current_edid(uint8_t value);

uint8_t get_current_vga(uint8_t channel);
uint8_t get_current_usb(uint8_t channel);
void set_current_vga_status(uint8_t channel, uint8_t status);
void set_current_usb_status(uint8_t channel, uint8_t status);

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
