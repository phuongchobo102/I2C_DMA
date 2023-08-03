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
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32f0xx_hal_crc.h"
#include "stm32f0xx_hal_i2c.h"

#include <stdio.h>
#include "IS31FL3218.h"
#include "aes.h"
#include "usb_sw_selector.h"
#include "system_switch.h"

#include "stm32_PMBUS_stack.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TEST_I2C1_SLAVE 	1    /*This one must enable to test I2C1 Slave or SMBus*/
#define TEST_SMBUS_IMPL
#define TEST_EDID_DELL_EXAMPLE

#define USE_PEC

//#define TEST_SMBUS_SELF_IMPL

#define EDID_COMMANDS_TABBLE_SIZE         		((uint8_t)128)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc;
DMA_HandleTypeDef hdma_adc;

CRC_HandleTypeDef hcrc;

SMBUS_HandleTypeDef hsmbus1;
I2C_HandleTypeDef hi2c2;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;

UART_HandleTypeDef huart1;

WWDG_HandleTypeDef hwwdg;

/* USER CODE BEGIN PV */
/* CRC handler declaration */
CRC_HandleTypeDef hcrc;

/* Used for storing CRC Value */
__IO uint32_t uwCRCValue = 0;
uint32_t uwExpectedCRCValue;

/******using for I2C2 switch pin*****************/
uint8_t I2C2_pin_switch=1;



uint8_t tx_buffer[64];     // Variable to store the output data
uint8_t report_buffer[64]; // Variable to receive the report buffer
uint8_t flag = 0;          // Variable to store the button flag
uint8_t flag_rx = 0;       // Variable to store the reception flag


#ifdef TEST_SMBUS_IMPL
uint8_t       *piobuf;
SMBUS_HandleTypeDef *phandle1;
SMBUS_StackHandleTypeDef *pcontext1;
SMBUS_StackHandleTypeDef context1;
uint32_t i2c1_smbus_lastTime;



st_command_t const EDID_COMMANDS_TABLE[128] =
{
	{ 0x00, READ_OR_WRITE, 0, 1 /*256*/ },	//=>256 is not ok										/* code 00 */
	{ 0x01, READ_OR_WRITE, 0, 1 },	                        				/* code 01 */
	{ 0x02, READ_OR_WRITE, 0, 1 },	                    					/* code 02 */
	{ 0x03, READ_OR_WRITE, 0, 1 },	                             			/* code 03 */
	{ 0x04, READ_OR_WRITE, 0, 1 },                              			/* code 04 */
	{ 0x05, READ_OR_WRITE, 0, 1 },                      					/* code 05 */
	{ 0x06, READ_OR_WRITE, 0, 1 },                      					/* code 06 */
	{ 0x07, READ_OR_WRITE, 0, 1 },                        					/* code 07 */
	{ 0x08, READ_OR_WRITE, 0, 1 },                        					/* code 08 */
	{ 0x09, READ_OR_WRITE, 0, 1 },                        					/* code 09 */
	{ 0x0A, READ_OR_WRITE, 0, 1 },                      					/* code 0A */
	{ 0x0B, READ_OR_WRITE, 0, 1 },                      					/* code 0B */
	{ 0x0C, READ_OR_WRITE, 0, 1 },                        					/* code 0C */
	{ 0x0D, READ_OR_WRITE, 0, 1 },                        					/* code 0D */
	{ 0x0E, READ_OR_WRITE, 0, 1 },                        					/* code 0E */
	{ 0x0F, READ_OR_WRITE, 0, 1 },											/* code 0F */
	{ 0x10, READ_OR_WRITE, 0, 1 },											/* code 10 */
	{ 0x11, READ_OR_WRITE, 0, 1 },                          				/* code 11 */
	{ 0x12, READ_OR_WRITE, 0, 1 },                          				/* code 12 */
	{ 0x13, READ_OR_WRITE, 0, 1 },                          				/* code 13 */
	{ 0x14, READ_OR_WRITE, 0, 1 },                       					/* code 14 */
	{ 0x15, READ_OR_WRITE, 0, 1 },											/* code 15 */
	{ 0x16, READ_OR_WRITE, 0, 1 },                           				/* code 16 */
	{ 0x17, READ_OR_WRITE, 0, 1 },											/* code 17 */
	{ 0x18, READ_OR_WRITE, 0, 1 },                          				/* code 18 */
	{ 0x19, READ_OR_WRITE, 0, 1 },                                  		/* code 19 */
	{ 0x1A, READ_OR_WRITE, 0, 1 },                               			/* code 1A */
	{ 0x1B, READ_OR_WRITE, 0, 1 },                      					/* code 1B */
	{ 0x1C, READ_OR_WRITE, 0, 1 },                               			/* code 1C */
	{ 0x1D, READ_OR_WRITE, 0, 1 },                      					/* code 1D */
	{ 0x1E, READ_OR_WRITE, 0, 1 },                               			/* code 1E */
	{ 0x1F, READ_OR_WRITE, 0, 1 },                      					/* code 1F */
	{ 0x20, READ_OR_WRITE, 0, 1 },											/* code 20 */
	{ 0x21, READ_OR_WRITE, 0, 1 },											/* code 21 */
	{ 0x22, READ_OR_WRITE, 0, 1 },											/* code 22 */
	{ 0x23, READ_OR_WRITE, 0, 1 },											/* code 23 */
	{ 0x24, READ_OR_WRITE, 0, 1 },											/* code 24 */
	{ 0x25, READ_OR_WRITE, 0, 1 },											/* code 25 */
	{ 0x26, READ_OR_WRITE, 0, 1 },											/* code 26 */
	{ 0x27, READ_OR_WRITE, 0, 1 },											/* code 27 */
	{ 0x28, READ_OR_WRITE, 0, 1 },											/* code 28 */
	{ 0x29, READ_OR_WRITE, 0, 1 },											/* code 29 */
	{ 0x2A, READ_OR_WRITE, 0, 1 },											/* code 2A */
	{ 0x2B, READ_OR_WRITE, 0, 1 },											/* code 2B */
	{ 0x2C, READ_OR_WRITE, 0, 1 },											/* code 2C */
	{ 0x2D, READ_OR_WRITE, 0, 1 },											/* code 2D */
	{ 0x2E, READ_OR_WRITE, 0, 1 },											/* code 2E */
	{ 0x2F, READ_OR_WRITE, 0, 1 },											/* code 2F */
	{ 0x30, READ_OR_WRITE, 0, 1 },											/* code 30 */
	{ 0x31, READ_OR_WRITE, 0, 1 },											/* code 31 */
	{ 0x32, READ_OR_WRITE, 0, 1 },											/* code 32 */
	{ 0x33, READ_OR_WRITE, 0, 1 },											/* code 33 */
	{ 0x34, READ_OR_WRITE, 0, 1 },											/* code 34 */
	{ 0x35, READ_OR_WRITE, 0, 1 },											/* code 35 */
	{ 0x36, READ_OR_WRITE, 0, 1 },											/* code 36 */
	{ 0x37, READ_OR_WRITE, 0, 1 },											/* code 37 */
	{ 0x38, READ_OR_WRITE, 0, 1 },											/* code 38 */
	{ 0x39, READ_OR_WRITE, 0, 1 },											/* code 39 */
	{ 0x3A, READ_OR_WRITE, 0, 1 },											/* code 3A */
	{ 0x3B, READ_OR_WRITE, 0, 1 },											/* code 3B */
	{ 0x3C, READ_OR_WRITE, 0, 1 },											/* code 3C */
	{ 0x3D, READ_OR_WRITE, 0, 1 },											/* code 3D */
	{ 0x3E, READ_OR_WRITE, 0, 1 },											/* code 3E */
	{ 0x3F, READ_OR_WRITE, 0, 1 },											/* code 3F */
	{ 0x40, READ_OR_WRITE, 0, 1 },											/* code 40 */
	{ 0x41, READ_OR_WRITE, 0, 1 },											/* code 41 */
	{ 0x42, READ_OR_WRITE, 0, 1 },											/* code 42 */
	{ 0x43, READ_OR_WRITE, 0, 1 },											/* code 43 */
	{ 0x44, READ_OR_WRITE, 0, 1 },											/* code 44 */
	{ 0x45, READ_OR_WRITE, 0, 1 },											/* code 45 */
	{ 0x46, READ_OR_WRITE, 0, 1 },											/* code 46 */
	{ 0x47, READ_OR_WRITE, 0, 1 },											/* code 47 */
	{ 0x48, READ_OR_WRITE, 0, 1 },											/* code 48 */
	{ 0x49, READ_OR_WRITE, 0, 1 },											/* code 49 */
	{ 0x4A, READ_OR_WRITE, 0, 1 },											/* code 4A */
	{ 0x4B, READ_OR_WRITE, 0, 1 },											/* code 4B */
	{ 0x4C, READ_OR_WRITE, 0, 1 },											/* code 4C */
	{ 0x4D, READ_OR_WRITE, 0, 1 },											/* code 4D */
	{ 0x4E, READ_OR_WRITE, 0, 1 },											/* code 4E */
	{ 0x4F, READ_OR_WRITE, 0, 1 },											/* code 4F */
	{ 0x50, READ_OR_WRITE, 0, 1 },											/* code 50 */
	{ 0x51, READ_OR_WRITE, 0, 1 },											/* code 51 */
	{ 0x52, READ_OR_WRITE, 0, 1 },											/* code 52 */
	{ 0x53, READ_OR_WRITE, 0, 1 },											/* code 53 */
	{ 0x54, READ_OR_WRITE, 0, 1 },											/* code 54 */
	{ 0x55, READ_OR_WRITE, 0, 1 },											/* code 55 */
	{ 0x56, READ_OR_WRITE, 0, 1 },											/* code 56 */
	{ 0x57, READ_OR_WRITE, 0, 1 },											/* code 57 */
	{ 0x58, READ_OR_WRITE, 0, 1 },											/* code 58 */
	{ 0x59, READ_OR_WRITE, 0, 1 },											/* code 59 */
	{ 0x5A, READ_OR_WRITE, 0, 1 },											/* code 5A */
	{ 0x5B, READ_OR_WRITE, 0, 1 },											/* code 5B */
	{ 0x5C, READ_OR_WRITE, 0, 1 },											/* code 5C */
	{ 0x5D, READ_OR_WRITE, 0, 1 },											/* code 5D */
	{ 0x5E, READ_OR_WRITE, 0, 1 },											/* code 5E */
	{ 0x5F, READ_OR_WRITE, 0, 1 },											/* code 5F */
	{ 0x60, READ_OR_WRITE, 0, 1 },											/* code 60 */
	{ 0x61, READ_OR_WRITE, 0, 1 },											/* code 61 */
	{ 0x62, READ_OR_WRITE, 0, 1 },											/* code 62 */
	{ 0x63, READ_OR_WRITE, 0, 1 },											/* code 63 */
	{ 0x64, READ_OR_WRITE, 0, 1 },											/* code 64 */
	{ 0x65, READ_OR_WRITE, 0, 1 },											/* code 65 */
	{ 0x66, READ_OR_WRITE, 0, 1 },											/* code 66 */
	{ 0x67, READ_OR_WRITE, 0, 1 },											/* code 67 */
	{ 0x68, READ_OR_WRITE, 0, 1 },											/* code 68 */
	{ 0x69, READ_OR_WRITE, 0, 1 },											/* code 69 */
	{ 0x6A, READ_OR_WRITE, 0, 1 },											/* code 6A */
	{ 0x6B, READ_OR_WRITE, 0, 1 },											/* code 6B */
	{ 0x6C, READ_OR_WRITE, 0, 1 },											/* code 6C */
	{ 0x6D, READ_OR_WRITE, 0, 1 },											/* code 6D */
	{ 0x6E, READ_OR_WRITE, 0, 1 },											/* code 6E */
	{ 0x6F, READ_OR_WRITE, 0, 1 },											/* code 6F */
	{ 0x70, READ_OR_WRITE, 0, 1 },											/* code 70 */
	{ 0x71, READ_OR_WRITE, 0, 1 },											/* code 71 */
	{ 0x72, READ_OR_WRITE, 0, 1 },											/* code 72 */
	{ 0x73, READ_OR_WRITE, 0, 1 },											/* code 73 */
	{ 0x74, READ_OR_WRITE, 0, 1 },											/* code 74 */
	{ 0x75, READ_OR_WRITE, 0, 1 },											/* code 75 */
	{ 0x76, READ_OR_WRITE, 0, 1 },											/* code 76 */
	{ 0x77, READ_OR_WRITE, 0, 1 },											/* code 77 */
	{ 0x78, READ_OR_WRITE, 0, 1 },											/* code 78 */
	{ 0x79, READ_OR_WRITE, 0, 1 },											/* code 79 */
	{ 0x7A, READ_OR_WRITE, 0, 1 },											/* code 7A */
	{ 0x7B, READ_OR_WRITE, 0, 1 },											/* code 7B */
	{ 0x7C, READ_OR_WRITE, 0, 1 },											/* code 7C */
	{ 0x7D, READ_OR_WRITE, 0, 1 },											/* code 7D */
	{ 0x7E, READ_OR_WRITE, 0, 1 },											/* code 7E */
	{ 0x7F, READ_OR_WRITE, 0, 1 },											/* code 7F */
//	{ 0x80, READ_OR_WRITE, 0, 1 },											/* code 80 */
//	{ 0x81, READ_OR_WRITE, 0, 1 },											/* code 81 */
//	{ 0x82, READ_OR_WRITE, 0, 1 },											/* code 82 */
//	{ 0x83, READ_OR_WRITE, 0, 1 },											/* code 83 */
//	{ 0x84, READ_OR_WRITE, 0, 1 },											/* code 84 */
//	{ 0x85, READ_OR_WRITE, 0, 1 },											/* code 85 */
//	{ 0x86, READ_OR_WRITE, 0, 1 },											/* code 86 */
//	{ 0x87, READ_OR_WRITE, 0, 1 },											/* code 87 */
//	{ 0x88, READ_OR_WRITE, 0, 1 },											/* code 88 */
//	{ 0x89, READ_OR_WRITE, 0, 1 },											/* code 89 */
//	{ 0x8A, READ_OR_WRITE, 0, 1 },											/* code 8A */
//	{ 0x8B, READ_OR_WRITE, 0, 1 },											/* code 8B */
//	{ 0x8C, READ_OR_WRITE, 0, 1 },											/* code 8C */
//	{ 0x8D, READ_OR_WRITE, 0, 1 },											/* code 8D */
//	{ 0x8E, READ_OR_WRITE, 0, 1 },											/* code 8E */
//	{ 0x8F, READ_OR_WRITE, 0, 1 },											/* code 8F */
//	{ 0x90, READ_OR_WRITE, 0, 1 },											/* code 90 */
//	{ 0x91, READ_OR_WRITE, 0, 1 },											/* code 91 */
//	{ 0x92, READ_OR_WRITE, 0, 1 },											/* code 92 */
//	{ 0x93, READ_OR_WRITE, 0, 1 },											/* code 93 */
//	{ 0x94, READ_OR_WRITE, 0, 1 },											/* code 94 */
//	{ 0x95, READ_OR_WRITE, 0, 1 },											/* code 95 */
//	{ 0x96, READ_OR_WRITE, 0, 1 },											/* code 96 */
//	{ 0x97, READ_OR_WRITE, 0, 1 },											/* code 97 */
//	{ 0x98, READ_OR_WRITE, 0, 1 },											/* code 98 */
//	{ 0x99, READ_OR_WRITE, 0, 1 },											/* code 99 */
//	{ 0x9A, READ_OR_WRITE, 0, 1 },											/* code 9A */
//	{ 0x9B, READ_OR_WRITE, 0, 1 },											/* code 9B */
//	{ 0x9C, READ_OR_WRITE, 0, 1 },											/* code 9C */
//	{ 0x9D, READ_OR_WRITE, 0, 1 },											/* code 9D */
//	{ 0x9E, READ_OR_WRITE, 0, 1 },											/* code 9E */
//	{ 0x9F, READ_OR_WRITE, 0, 1 },											/* code 9F */
//	{ 0xA0, READ_OR_WRITE, 0, 1 },											/* code A0 */
//	{ 0xA1, READ_OR_WRITE, 0, 1 },											/* code A1 */
//	{ 0xA2, READ_OR_WRITE, 0, 1 },											/* code A2 */
//	{ 0xA3, READ_OR_WRITE, 0, 1 },											/* code A3 */
//	{ 0xA4, READ_OR_WRITE, 0, 1 },											/* code A4 */
//	{ 0xA5, READ_OR_WRITE, 0, 1 },											/* code A5 */
//	{ 0xA6, READ_OR_WRITE, 0, 1 },											/* code A6 */
//	{ 0xA7, READ_OR_WRITE, 0, 1 },											/* code A7 */
//	{ 0xA8, READ_OR_WRITE, 0, 1 },											/* code A8 */
//	{ 0xA9, READ_OR_WRITE, 0, 1 },											/* code A9 */
//	{ 0xAA, READ_OR_WRITE, 0, 1 },											/* code AA */
//	{ 0xAB, READ_OR_WRITE, 0, 1 },											/* code AB */
//	{ 0xAC, READ_OR_WRITE, 0, 1 },											/* code AC */
//	{ 0xAD, READ_OR_WRITE, 0, 1 },											/* code AD */
//	{ 0xAE, READ_OR_WRITE, 0, 1 },											/* code AE */
//	{ 0xAF, READ_OR_WRITE, 0, 1 },											/* code AF */
//	{ 0xB0, READ_OR_WRITE, 0, 1 },											/* code B0 */
//	{ 0xB1, READ_OR_WRITE, 0, 1 },											/* code B1 */
//	{ 0xB2, READ_OR_WRITE, 0, 1 },											/* code B2 */
//	{ 0xB3, READ_OR_WRITE, 0, 1 },											/* code B3 */
//	{ 0xB4, READ_OR_WRITE, 0, 1 },											/* code B4 */
//	{ 0xB5, READ_OR_WRITE, 0, 1 },											/* code B5 */
//	{ 0xB6, READ_OR_WRITE, 0, 1 },											/* code B6 */
//	{ 0xB7, READ_OR_WRITE, 0, 1 },											/* code B7 */
//	{ 0xB8, READ_OR_WRITE, 0, 1 },											/* code B8 */
//	{ 0xB9, READ_OR_WRITE, 0, 1 },											/* code B9 */
//	{ 0xBA, READ_OR_WRITE, 0, 1 },											/* code BA */
//	{ 0xBB, READ_OR_WRITE, 0, 1 },											/* code BB */
//	{ 0xBC, READ_OR_WRITE, 0, 1 },											/* code BC */
//	{ 0xBD, READ_OR_WRITE, 0, 1 },											/* code BD */
//	{ 0xBE, READ_OR_WRITE, 0, 1 },											/* code BE */
//	{ 0xBF, READ_OR_WRITE, 0, 1 },											/* code BF */
//	{ 0xC0, READ_OR_WRITE, 0, 1 },											/* code C0 */
//	{ 0xC1, READ_OR_WRITE, 0, 1 },											/* code C1 */
//	{ 0xC2, READ_OR_WRITE, 0, 1 },											/* code C2 */
//	{ 0xC3, READ_OR_WRITE, 0, 1 },											/* code C3 */
//	{ 0xC4, READ_OR_WRITE, 0, 1 },											/* code C4 */
//	{ 0xC5, READ_OR_WRITE, 0, 1 },											/* code C5 */
//	{ 0xC6, READ_OR_WRITE, 0, 1 },											/* code C6 */
//	{ 0xC7, READ_OR_WRITE, 0, 1 },											/* code C7 */
//	{ 0xC8, READ_OR_WRITE, 0, 1 },											/* code C8 */
//	{ 0xC9, READ_OR_WRITE, 0, 1 },											/* code C9 */
//	{ 0xCA, READ_OR_WRITE, 0, 1 },											/* code CA */
//	{ 0xCB, READ_OR_WRITE, 0, 1 },											/* code CB */
//	{ 0xCC, READ_OR_WRITE, 0, 1 },											/* code CC */
//	{ 0xCD, READ_OR_WRITE, 0, 1 },											/* code CD */
//	{ 0xCE, READ_OR_WRITE, 0, 1 },											/* code CE */
//	{ 0xCF, READ_OR_WRITE, 0, 1 },											/* code CF */
//	{ 0xD0, READ_OR_WRITE, 0, 1 },											/* code D0 */
//	{ 0xD1, READ_OR_WRITE, 0, 1 },											/* code D1 */
//	{ 0xD2, READ_OR_WRITE, 0, 1 },											/* code D2 */
//	{ 0xD3, READ_OR_WRITE, 0, 1 },											/* code D3 */
//	{ 0xD4, READ_OR_WRITE, 0, 1 },											/* code D4 */
//	{ 0xD5, READ_OR_WRITE, 0, 1 },											/* code D5 */
//	{ 0xD6, READ_OR_WRITE, 0, 1 },											/* code D6 */
//	{ 0xD7, READ_OR_WRITE, 0, 1 },											/* code D7 */
//	{ 0xD8, READ_OR_WRITE, 0, 1 },											/* code D8 */
//	{ 0xD9, READ_OR_WRITE, 0, 1 },											/* code D9 */
//	{ 0xDA, READ_OR_WRITE, 0, 1 },											/* code DA */
//	{ 0xDB, READ_OR_WRITE, 0, 1 },											/* code DB */
//	{ 0xDC, READ_OR_WRITE, 0, 1 },											/* code DC */
//	{ 0xDD, READ_OR_WRITE, 0, 1 },											/* code DD */
//	{ 0xDE, READ_OR_WRITE, 0, 1 },											/* code DE */
//	{ 0xDF, READ_OR_WRITE, 0, 1 },											/* code DF */
//	{ 0xE0, READ_OR_WRITE, 0, 1 },											/* code E0 */
//	{ 0xE1, READ_OR_WRITE, 0, 1 },											/* code E1 */
//	{ 0xE2, READ_OR_WRITE, 0, 1 },											/* code E2 */
//	{ 0xE3, READ_OR_WRITE, 0, 1 },											/* code E3 */
//	{ 0xE4, READ_OR_WRITE, 0, 1 },											/* code E4 */
//	{ 0xE5, READ_OR_WRITE, 0, 1 },											/* code E5 */
//	{ 0xE6, READ_OR_WRITE, 0, 1 },											/* code E6 */
//	{ 0xE7, READ_OR_WRITE, 0, 1 },											/* code E7 */
//	{ 0xE8, READ_OR_WRITE, 0, 1 },											/* code E8 */
//	{ 0xE9, READ_OR_WRITE, 0, 1 },											/* code E9 */
//	{ 0xEA, READ_OR_WRITE, 0, 1 },											/* code EA */
//	{ 0xEB, READ_OR_WRITE, 0, 1 },											/* code EB */
//	{ 0xEC, READ_OR_WRITE, 0, 1 },											/* code EC */
//	{ 0xED, READ_OR_WRITE, 0, 1 },											/* code ED */
//	{ 0xEE, READ_OR_WRITE, 0, 1 },											/* code EE */
//	{ 0xEF, READ_OR_WRITE, 0, 1 },											/* code EF */
//	{ 0xF0, READ_OR_WRITE, 0, 1 },											/* code F0 */
//	{ 0xF1, READ_OR_WRITE, 0, 1 },											/* code F1 */
//	{ 0xF2, READ_OR_WRITE, 0, 1 },											/* code F2 */
//	{ 0xF3, READ_OR_WRITE, 0, 1 },											/* code F3 */
//	{ 0xF4, READ_OR_WRITE, 0, 1 },											/* code F4 */
//	{ 0xF5, READ_OR_WRITE, 0, 1 },											/* code F5 */
//	{ 0xF6, READ_OR_WRITE, 0, 1 },											/* code F6 */
//	{ 0xF7, READ_OR_WRITE, 0, 1 },											/* code F7 */
//	{ 0xF8, READ_OR_WRITE, 0, 1 },											/* code F8 */
//	{ 0xF9, READ_OR_WRITE, 0, 1 },											/* code F9 */
//	{ 0xFA, READ_OR_WRITE, 0, 1 },											/* code FA */
//	{ 0xFB, READ_OR_WRITE, 0, 1 },											/* code FB */
//	{ 0xFC, READ_OR_WRITE, 0, 1 },											/* code FC */
//	{ 0xFD, READ_OR_WRITE, 0, 1 },											/* code FD */
//	{ 0xFE, READ_OR_WRITE, 0, 1 },											/* code FE */
//	{ 0xFF, READ_OR_WRITE, 0, 1 },											/* code FF */
};


#if defined(TEST_EDID_DELL_EXAMPLE)
uint8_t atest_edid[128] = {
		0x00,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x1e,0x6d,0x33,0x5b,0xfa,0xf0,0x04,0x00,0x07,0x1d,0x01,0x03,0x10,0x30,0x1b,0x78,0xea,0x31,0x35,0xa5,0x55,0x4e,0xa1,0x26,0x0c,0x50,0x54,0xa5,0x4b,0x00,0x01,0x01,0x81,0x80,0x95,0x00,0xb3,0x00,0x01,0x01,0x01,0x01,0x81,0xc0,0x01,0x01,0x02,0x3a,0x80,0x18,0x71,0x38,0x2d,0x40,0x58,0x2c,0x45,0x00,0xe0,0x0e,0x11,0x00,0x00,0x1e,0x00,0x00,0x00,0xfd,0x00,0x28,0x4b,0x1e,0x55,0x12,0x00,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0xfc,0x00,0x4d,0x45,0x47,0x52,0x45,0x20,0x53,0x43,0x52,0x45,0x45,0x4e,0x0a,0x00,0x00,0x00,0xff,0x00,0x39,0x30,0x37,0x4e,0x54,0x59,0x54,0x39,0x48,0x38,0x33,0x34,0x0a,0x00,0xb1}; //DEL2011_A37291D73923
#endif /*TEST_EDID_DELL_EXAMPLE*/
#endif 				/*TEST_SMBUS_IMPL*/
#ifdef TEST_I2C1_SLAVE
//
//bool isI2C1Receive = false;
//bool isI2C1Transmit = false;
//
//uint8_t test_tx_buffer[128] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZ123456ABCDEFGHIJKLMNOPQRSTUVWXYZ123456ABCDEFGHIJKLMNOPQRSTUVWXYZ123456ABCDEFGHIJKLMNOPQRSTUVWXYZ123456"};
//uint8_t test_rx_buffer[128] = {"abcdefghijklmnopqrstuvwxyz123456"};
//
//typedef struct smbus_rx_struct {
//	uint8_t u8_state;
//	uint8_t u8_pre_state;
//	uint16_t u16_counter;
//	uint16_t u16_buffer_size;
//	uint16_t u16_buffer_index;
//}smbus_rx_t;
//
//typedef struct smbus_tx_struct {
//	uint8_t u8_state;
//	uint8_t u8_pre_state;
//	uint16_t u16_counter;
//	uint16_t u16_buffer_size;
//	uint16_t u16_buffer_index;
//}smbus_tx_t;
//
//typedef struct smbus_instance_struct {
//	SMBUS_HandleTypeDef *p_hsmbus_inst;
//	uint8_t u8_state;
//	uint8_t u8_pre_state;
//	uint8_t u8_smb_type;
//	uint8_t u8_command;
//	smbus_rx_t t_rx;
//	smbus_tx_t t_tx;
//}smbus_instance_t;
//
//typedef enum smbus_slave_type_enum{
//    SMBUS_SLAVE_TYPE_QUICK_CMD,
//	SMBUS_SLAVE_TYPE_SEND_BYTE,
//	SMBUS_SLAVE_TYPE_RECV_BYTE,
//	SMBUS_SLAVE_TYPE_READ_BYTE,
//	SMBUS_SLAVE_TYPE_WRITE_BYTE,
//	SMBUS_SLAVE_TYPE_READ_BYTES,
//	SMBUS_SLAVE_TYPE_WRITE_BYTES,
//	SMBUS_SLAVE_TYPE_BLK_READ,
//	SMBUS_SLAVE_TYPE_BLK_WRITE,
//	SMBUS_SLAVE_TYPE_TOTAL
//}smbus_slave_type_e;
//
//typedef enum smbus_slave_state_enum{
//    SMBUS_SLAVE_STATE_READY,
//    SMBUS_SLAVE_STATE_LISTEN,
//    SMBUS_SLAVE_STATE_ADDR_1ST_WRITE,
//	SMBUS_SLAVE_STATE_ADDR_1ST_READ,
//	SMBUS_SLAVE_STATE_RECEIVED_1ST_BYTE,
//	SMBUS_SLAVE_STATE_ADDR_2ND_READ,
//    SMBUS_SLAVE_STATE_DO_RX_FOR_WR_CMD,
//	SMBUS_SLAVE_STATE_DO_TX_FOR_RD_CMD,
//	SMBUS_SLAVE_STATE_WAIT_FOR_LISTENCPLT,
//	SMBUS_SLAVE_STATE_LISTENCPLT
//}smbus_slave_state_e;
//
//typedef enum smbus_slave_rx_state_enum{
//    SMBUS_SLAVE_RX_STATE_1ST,
//	SMBUS_SLAVE_RX_STATE_NEXT,
//	SMBUS_SLAVE_RX_STATE_END
//}smbus_slave_rx_state_e;
//
//typedef enum smbus_slave_tx_state_enum{
//    SMBUS_SLAVE_TX_STATE_1ST,
//	SMBUS_SLAVE_TX_STATE_NEXT,
//	SMBUS_SLAVE_TX_STATE_END,
//	SMBUS_SLAVE_TX_STATE_1ST_2BYTES,
//	SMBUS_SLAVE_TX_STATE_NEXT_2BYTES,
//	SMBUS_SLAVE_TX_STATE_END_2BYTES
//}smbus_slave_tx_state_e;
//
////uint8_t i2C1ReceiveState = SMBUS_SLAVE_STATE_READY;
//
//smbus_instance_t t_smbus_i2c1;// = {.p_hsmbus_inst = &hsmbus1};
#endif /*TEST_I2C1_SLAVE*/

// extern the USB handler
extern USBD_HandleTypeDef hUsbDeviceFS;




uint32_t id_ic = 0;

uint32_t test_id_ic = 0x55aa;
uint8_t channelSelect = 0;
//uint8_t edidStatus=1;
/////////////////////////////////
//uint8_t edid_test=1;

edid_status_t edidStatus=EDID_VGA_1;
uint8_t vgaStatus[4];
uint8_t usbStatus[4];

//

uint32_t REVID = 0;
uint32_t DEVID = 0;

uint32_t UID[4] = {0};


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC_Init(void);
static void MX_I2C1_SMBUS_Init(void);
static void MX_I2C2_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI2_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_CRC_Init(void);
static void MX_WWDG_Init(void);
/* USER CODE BEGIN PFP */
#ifdef TEST_SMBUS_IMPL
void I2C1_SMBUS_Stack_Init(void);
void I2C1_SMBUS_Stack_Task(void);
HAL_StatusTypeDef STACK_SMBUS_ExecuteCommand( SMBUS_StackHandleTypeDef *pStackContext );
static void Error_Check( SMBUS_StackHandleTypeDef *pStackContext);
#endif /*TEST_SMBUS_IMPL*/
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t get_current_channel()
{
  return channelSelect;
}

void set_current_channel(uint8_t channel)
{
  if (channel >= 0 && channel <= 4)
    channelSelect = channel;
}

edid_status_t get_current_edid()
{
  return edidStatus;
}
void set_current_edid(edid_status_t value)
{
  edidStatus = ++value;
  if (edidStatus==4) // turn back value
	  edidStatus=EDID_VGA_1;
}

uint8_t get_current_vga(uint8_t channel)
{
  if (channel < 5 && channel >= 0)
    return vgaStatus[channel];
  return 0;
}
void set_current_vga_status(uint8_t channel, uint8_t status)
{
  if (channel < 5 && channel >= 0)
    vgaStatus[channel] = status;
}

uint8_t get_current_usb(uint8_t channel)
{
  if (channel < 5 && channel >= 0)
    return usbStatus[channel];
  return 0;
}

//#define ENABLE_WATCHDOG

void set_current_usb_status(uint8_t channel, uint8_t status)
{
  if (channel < 5 && channel >= 0)
    usbStatus[channel] = status;
}



#ifdef TEST_SMBUS_IMPL
void I2C1_SMBUS_Stack_Init(void)
{
  hsmbus1.pBuffPtr = context1.Buffer;

  phandle1 = &hsmbus1;
//
////  /HAL_SMBUS_Init( phandle1 );
//
    context1.CMD_table = (st_command_t *) & EDID_COMMANDS_TABLE[0];
    context1.CMD_tableSize = EDID_COMMANDS_TABBLE_SIZE;
//  //#ifndef TEST4
//  //#ifndef TEST5
//  //  /* Most tests do not use actual PMBUS commands */
//  //  context1.CMD_table = (st_command_t *) & PMBUS_COMMANDS_TEST[0];
//  //  context1.CMD_tableSize = PMBUS_CMD_TBL_SIZE;
//  //#endif /* TEST4 */
//  //#endif /* TEST5 */
    context1.Device = &hsmbus1;//phandle1;
//    //ToDo: check the preset byte
    context1.SRByte = 0x55U;
    context1.CurrentCommand = NULL;
//  #ifdef ARP
//    context1.StateMachine = SMBUS_SMS_NONE;
//  #ifdef DEV_PSA
//    context1.OwnAddress = SMBUS_ADDR_DEVICE;
//  #else /* DEV_PSA */
//    context1.OwnAddress = 0U;
//  #endif /* DEV_PSA */
//  #ifndef  HOST1
//    context1.ARP_UDID = (uint8_t *) &UDID;
//  #endif /* HOST1 */
//  #else /* ARP */
////    context1.StateMachine = SMBUS_SMS_ARP_AR;
	context1.OwnAddress = hsmbus1.Init.OwnAddress1;//SMBUS_ADDR_DEVICE;
//  #endif /* ARP */
#ifdef USE_PEC
context1.StateMachine |= SMBUS_SMS_PEC_ACTIVE;
#endif
    pcontext1 = &context1;
//
    STACK_SMBUS_Init( pcontext1 );

//    if(HAL_SMBUS_EnableListen_IT(&hsmbus1) != HAL_OK)
//    	  {
//				  /* Transfer error in transmission process */
//				  Error_Handler();
//		  }

    /*Read buffer 1st time*/

    piobuf = STACK_SMBUS_GetBuffer( pcontext1 );
    if (piobuf != NULL )
    {
      for (uint8_t index = 0U; index < STACK_NBYTE_SIZE; index++)
      {
        piobuf[index] = (uint8_t)index;
      }
    }

    i2c1_smbus_lastTime = HAL_GetTick();
}

void I2C1_SMBUS_Stack_Task(void)
{
	if (HAL_GetTick() - i2c1_smbus_lastTime > 200)
	{
	  i2c1_smbus_lastTime = HAL_GetTick();



	  /* Periodically checking error state of the stack */
		Error_Check( &context1 );

		/* Periodically checking for Quick command */
		if ( context1.StateMachine & ( SMBUS_SMS_QUICK_CMD_W | SMBUS_SMS_QUICK_CMD_R ) )
		{
			/* Todo: Indicate status */
//	  	  	BSP_LED_On(LED_GREEN);
			context1.StateMachine &= ~( SMBUS_SMS_QUICK_CMD_W | SMBUS_SMS_QUICK_CMD_R );
		}

		/* Todo: Indicate status */
//	  	BSP_LED_Off(LED_GREEN);

		/* Cleaning arbitration loss flag */
		context1.StateMachine &= ~SMBUS_SMS_ERR_ARLO;

//	  	if (buttonpress == 1U)
//	  	{
//#ifdef ALERT
//	  	  STACK_SMBUS_SendAlert( pcontext1 );
//#else /* ALERT */
//	  	  STACK_SMBUS_NotifyHost( pcontext1 );
//#endif /* DEV_ALERT */
//	  	  buttonpress = 0U;
//	  	}
	}
}

/**
  * @brief  a version of the default SMBUS implementation with support for
  *       EDID
  * @param  pStackContext : Pointer to a SMBUS_StackHandleTypeDef structure that contains
  *                the configuration information for the specified SMBUS.
  * @retval None
  */
void STACK_SMBUS_LocateCommand( SMBUS_StackHandleTypeDef *pStackContext )
{
  uint8_t       commandCode = pStackContext->Buffer[0];
#ifdef DENSE_CMD_TBL
  uint32_t      current, low, top;
#endif

#ifdef ARP
  if ( pStackContext->StateMachine & SMBUS_SMS_ARP_AM )
  {
    STACK_SMBUS_LocateCommandARP( pStackContext, commandCode );
  }
  else
#endif /* ARP treatment */

#if 0
    if (commandCode == 0)
    {

    	uint8_t       *piobuf = NULL;

		/* accessing the IO buffer */
		piobuf = STACK_SMBUS_GetBuffer( pStackContext );
		 /*
		Copy data to SMBus buffer
		*/
		memcpy(piobuf, flashBufferVGA128, 256);
    }
    else
#endif
    {
      /*
        Code searching for command based on command code
        */
#ifdef DENSE_CMD_TBL

      /*
        initializing the command code search - the table must have all commands sorted, but there may be gaps
       */
      low = 0U;
      top = pStackContext->CMD_tableSize - 1U;
      pStackContext->CurrentCommand = NULL;

      while ( top >= low )
      {
        /*
          Pick interval half
         */
        current = ( low + top ) >> 1U;
        if (pStackContext->CMD_table[current].cmnd_code == commandCode)
        {
          /*
            we have found our command
           */
          pStackContext->CurrentCommand = &(pStackContext->CMD_table[current]);
          return;
        }
        else if (pStackContext->CMD_table[current].cmnd_code < commandCode)
        {
          /*
            Look at upper half
          */
          low = current + 1U;
        }
        else
        {
          top = current - 1U;
        }
      }
#else
      /*
      Simple command table - command code equals the table index
      */
      pStackContext->CurrentCommand = &(pStackContext->CMD_table[commandCode]);

      /* a sanity check */
      if ( pStackContext->CurrentCommand->cmnd_code != commandCode )
      {
        pStackContext->CurrentCommand = NULL;
      }
#endif /* DENSE_CMD_TBL */
    }
}
/**
  * @brief  Callback function notifying slave about command incoming, implementation
    is supporting extended command defined by PMBus
  * @param  pStackContext : Pointer to a SMBUS_StackHandleTypeDef structure that contains
  *                the configuration information for the specified SMBUS.
  * @retval HAL_StatusTypeDef response code. Equal STACK_OK if success, any other value means problem
  */
HAL_StatusTypeDef STACK_SMBUS_ExecuteCommand( SMBUS_StackHandleTypeDef *pStackContext )
{
  uint8_t       *piobuf = NULL;

  /* accessing the IO buffer */
  piobuf = STACK_SMBUS_GetBuffer( pStackContext );

  if ( piobuf == NULL )
  {
    pStackContext->StateMachine |= SMBUS_SMS_ERROR;
  }
//  else if ( pStackContext->CurrentCommand == (st_command_t *)&HOST_NOTIFY_PROTOCOL )
//  {
//    /* host notify command */
//    if ( pStackContext->Buffer[0] == SMBUS_ADDR_DEFAULT )
//    {
//      /* Usually the Host notify is used for ARP, but may not be limited to it */
//#ifdef ARP
//#ifdef HOST1
//      /* case of ARP notify */
//      ARP_process = 1U;
//#endif /* HOST1 */
//#endif /* ARP */
//    }
//  }
  else    /* Normal device command execution */
  {
    /* Zone config command example implementation */
#ifdef PMBUS13
    if ( pStackContext->CurrentCommand->cmnd_code == PMBC_ZONE_CONFIG )
    {
      pStackContext->TheZone.writeZone = pStackContext->Buffer[1];
      pStackContext->TheZone.readZone = pStackContext->Buffer[2];
    }
    else if ( pStackContext->CurrentCommand->cmnd_code == PMBC_ZONE_ACTIVE )
    {
      pStackContext->TheZone.activeWriteZone = pStackContext->Buffer[1];
      pStackContext->TheZone.activeReadZone = pStackContext->Buffer[2];
    }

#endif /* PMBUS13 */
    /*
      first step is to see if we have a case of extended command
    */
//    if ( pStackContext->CurrentCommand->cmnd_code == PMBC_PMBUS_COMMAND_EXT )
//    {
////      BSP_LED_On(LED_GREEN);
//    }
//    else /* regular command case */
//    {
////      BSP_LED_On(LED_GREEN);
//      if ((pStackContext->CurrentCommand->cmnd_query & BLOCK ) == BLOCK )
//      {
//        *piobuf = (pStackContext->CurrentCommand->cmnd_master_Rx_size) - 1U;
//        /* byte size of reply for block read command */
//        /* One byte for size, rest are [size] of data */
//      }
//    }
    if ((pStackContext->CurrentCommand->cmnd_query & READ_OR_WRITE ) == READ_OR_WRITE )
	{
//    	*piobuf = (pStackContext->CurrentCommand->cmnd_master_Rx_size) - 1U;
    	bool isFind = false;
    	static uint8_t dummy = 0;
    	for(uint8_t i = 0; i < 256; i++)
    	{
    		//if(pStackContext->CurrentCommand == (st_command_t *)&EDID_COMMANDS_TABLE[i])
    		//if(pStackContext->CurrentCommand->cmnd_code == ((st_command_t *)&EDID_COMMANDS_TABLE[i])->cmnd_code)
    		if(pStackContext->CurrentCommand->cmnd_code == EDID_COMMANDS_TABLE[i].cmnd_code)
    		{
    			if ((pStackContext->CurrentCommand->cmnd_query & READ ) == READ )
    			{
    				isFind = true;
#ifdef TEST_EDID_DELL_EXAMPLE
    				*piobuf = atest_edid[i];
//    				if(i != 0)
//    				{
//    					*piobuf = flashBufferVGA128[i];
//    				}
    				/*Case i=0 is handled in STACK_SMBUS_LocateCommand	*/
					return STACK_OK;
//    				memcpy(piobuf,&atest_edid[i],pStackContext->CurrentCommand->cmnd_master_Rx_size);
//    				piobuf++;
//    				*piobuf = atest_edid[i];
//    				printf("%x\r\n",*piobuf);
//    				printf("m\n");
#endif /*TEST_EDID_DELL_EXAMPLE*/
    				//ToDo: copy data into piobuff to transfer to host
//    				*piobuf =
    			}else if ((pStackContext->CurrentCommand->cmnd_query & WRITE ) == WRITE )
    			{
    				//ToDo: get data from piobuff and then process it
//    				*piobuf =
    			}else
    			{
    				pStackContext->StateMachine |=  SMBUS_SMS_IGNORED;
    			}
                        break;
    		}
    	}
    	if(!isFind)
    	{
    		*piobuf = dummy;
    		dummy++;
            printf("n\r\n");
    	}
    }else
    {
            printf("u\r\n");
    }

  }
  return STACK_OK;
}

/**
  * @brief  Stub of an error treatment function - set to ignore most errors
  * @param  pStackContext : Pointer to a SMBUS_StackHandleTypeDef structure that contains
  *                the configuration information for the specified SMBUS.
  * @retval None
  */
static void Error_Check( SMBUS_StackHandleTypeDef *pStackContext)
{
  if ( ( STACK_SMBUS_IsBlockingError(pStackContext) ) || ( STACK_SMBUS_IsCmdError( pStackContext ) ) )
  {
    /* No action, error symptoms are ignored */
    pStackContext->StateMachine &= ~(SMBUS_ERROR_CRITICAL | SMBUS_COM_ERROR);
  }
  else if ((pStackContext->StateMachine & SMBUS_SMS_ERR_PECERR ) ==
           SMBUS_SMS_ERR_PECERR ) /* PEC error, we won't wait for any more action */
  {
    pStackContext->StateMachine |= SMBUS_SMS_READY;
    pStackContext->CurrentCommand = NULL;
    pStackContext->StateMachine &= ~(SMBUS_SMS_ACTIVE_MASK | SMBUS_SMS_ERR_PECERR);
  }
}
#endif  /*TEST_SMBUS_IMPL*/
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */

void __write(){}
 void __lseek(){}
 void __close(){}
 int remove(char const* a){
 return 1;
 }
int main(void)
{
  /* USER CODE BEGIN 1 */
  char tmp = 1;
  char str;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USB_DEVICE_Init();
  MX_ADC_Init();
  MX_I2C1_SMBUS_Init();
  MX_I2C2_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_USART1_UART_Init();
  MX_CRC_Init();
  /* USER CODE BEGIN 2 */
  REVID = HAL_GetREVID();
  DEVID = HAL_GetDEVID();
  UID[0] = HAL_GetUIDw0();
  UID[1] = HAL_GetUIDw1();
  UID[2] = HAL_GetUIDw2();

  str = UID[0];

  printf("Hello CUSTOMHID_F070 from IAR \n\r");
  printf("Continue.... \n\r");
  printf("UID[0] %d.... \n\r", UID[0]);
  printf("UID[1] %d.... \n\r", UID[1]);
  printf("UID[2] %d.... \n\r", UID[2]);

  //  test_AES(); while(1);
  //  process_usb_msg();

  // init the system
  system_switch_init();
  elinkswitch_init();
  elsgpio_init();
  authenKVM_init();
  HAL_ADCEx_Calibration_Start(&hadc);
  init_IS31FL3218();
  usb_kvm_switch_init();
  system_switch_init();
  vga_init();
  //    test_LED();
#ifdef ENABLE_WATCHDOG
  MX_WWDG_Init();
#endif

//#ifdef TEST_I2C1_SLAVE
//  t_smbus_i2c1.p_hsmbus_inst = &hsmbus1;
//  t_smbus_i2c1.t_rx.u16_buffer_size = sizeof(test_rx_buffer);
//  t_smbus_i2c1.t_rx.u16_counter = 0;
//  t_smbus_i2c1.t_tx.u16_buffer_size = sizeof(test_tx_buffer);
//  t_smbus_i2c1.t_tx.u16_counter = 0;
//#endif/* TEST_I2C1_SLAVE*/
#ifdef TEST_SMBUS_IMPL
//  hsmbus1.pBuffPtr = context1.Buffer;
//
//  phandle1 = &hsmbus1;
////
//////  /HAL_SMBUS_Init( phandle1 );
////
//    context1.CMD_table = (st_command_t *) & EDID_COMMANDS_TABLE[0];
//    context1.CMD_tableSize = EDID_COMMANDS_TABBLE_SIZE;
////  //#ifndef TEST4
////  //#ifndef TEST5
////  //  /* Most tests do not use actual PMBUS commands */
////  //  context1.CMD_table = (st_command_t *) & PMBUS_COMMANDS_TEST[0];
////  //  context1.CMD_tableSize = PMBUS_CMD_TBL_SIZE;
////  //#endif /* TEST4 */
////  //#endif /* TEST5 */
//    context1.Device = &hsmbus1;//phandle1;
////    //ToDo: check the preset byte
//    context1.SRByte = 0x55U;
//    context1.CurrentCommand = NULL;
////  #ifdef ARP
////    context1.StateMachine = SMBUS_SMS_NONE;
////  #ifdef DEV_PSA
////    context1.OwnAddress = SMBUS_ADDR_DEVICE;
////  #else /* DEV_PSA */
////    context1.OwnAddress = 0U;
////  #endif /* DEV_PSA */
////  #ifndef  HOST1
////    context1.ARP_UDID = (uint8_t *) &UDID;
////  #endif /* HOST1 */
////  #else /* ARP */
//////    context1.StateMachine = SMBUS_SMS_ARP_AR;
//	context1.OwnAddress = hsmbus1.Init.OwnAddress1;//SMBUS_ADDR_DEVICE;
////  #endif /* ARP */
//#ifdef USE_PEC
//context1.StateMachine |= SMBUS_SMS_PEC_ACTIVE;
//#endif
//    pcontext1 = &context1;
////
//    STACK_SMBUS_Init( pcontext1 );
//
////    if(HAL_SMBUS_EnableListen_IT(&hsmbus1) != HAL_OK)
////    	  {
////				  /* Transfer error in transmission process */
////				  Error_Handler();
////		  }
//
//    /*Read buffer 1st time*/
//
//    piobuf = STACK_SMBUS_GetBuffer( pcontext1 );
//    if (piobuf != NULL )
//    {
//      for (uint8_t index = 0U; index < STACK_NBYTE_SIZE; index++)
//      {
//        piobuf[index] = (uint8_t)index;
//      }
//    }
//
//    i2c1_smbus_lastTime = HAL_GetTick();
  I2C1_SMBUS_Stack_Init();
#endif /*TEST_SMBUS_IMPL*/
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    elsgpio_task();
    led_task();
    elinkswitch_task();
    vga_tasks();
    system_switch_tasks();

    authenKVM();
 
#ifdef ENABLE_WATCHDOG

    system_switch_tasks();
//    HAL_Delay(1);

  //*
      if (HAL_WWDG_Refresh(&WwdgHandle) != HAL_OK)
      {
        Error_Handler();
      }
#endif

#ifdef TEST_I2C1_SLAVE
//      if(isI2C1Receive)
//      {
//		  if(HAL_I2C_Slave_Receive_DMA(&hi2c1, (uint8_t *)test_rx_buffer, sizeof(test_rx_buffer)) != HAL_OK)
//		  {
//				  /* Transfer error in transmission process */
//				  Error_Handler();
//		  }
//		  isI2C1Receive = false;
//		  while(HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY);
//		  while(HAL_I2C_GetError(&hi2c1) == HAL_I2C_ERROR_AF);
//      }else if(isI2C1Transmit)
//      {
//    	  if(HAL_I2C_Slave_Transmit_DMA(&hi2c1, (uint8_t*)test_tx_buffer, sizeof(test_tx_buffer))!= HAL_OK)
//    	  {
//			  /* Transfer error in transmission process */
//			  Error_Handler();
//    	  }
//    	  isI2C1Transmit = false;
//    	  while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY);
//      }

//      if(isI2C1Receive)
//      {
//    	  if(HAL_SMBUS_EnableListen_IT(&hsmbus1) != HAL_OK)
//    	  {
//				  /* Transfer error in transmission process */
//				  Error_Handler();
//		  }
//
////    	  i2C1ReceiveState = SMBUS_SLAVE_STATE_LISTEN;
//    	  t_smbus_i2c1.u8_state = SMBUS_SLAVE_STATE_LISTEN;
////		  if(HAL_SMBUS_Slave_Receive_IT(&hsmbus1, (uint8_t *)test_rx_buffer, 1/*sizeof(test_rx_buffer)*/, SMBUS_FIRST_FRAME) != HAL_OK)
////		  {
////				  /* Transfer error in transmission process */
////				  Error_Handler();
////		  }
//		  isI2C1Receive = false;
//		  /* Device is ready */
//		  //while(hsmbus1.State != HAL_SMBUS_STATE_READY);
////          while(HAL_SMBUS_GetState(&hsmbus1) != HAL_SMBUS_STATE_READY);
//      }else if(isI2C1Transmit)
//      {
//        if(HAL_SMBUS_EnableListen_IT(&hsmbus1) != HAL_OK)
//    	  {
//                          /* Transfer error in transmission process */
//                          Error_Handler();
//          }
//    	  if(HAL_SMBUS_Slave_Transmit_IT(&hsmbus1, (uint8_t *)test_tx_buffer, sizeof(test_tx_buffer), SMBUS_SOFTEND_MODE) != HAL_OK)
//		  {
//				  /* Transfer error in transmission process */
//				  Error_Handler();
//		  }
//    	  isI2C1Transmit = false;
//    	  //while(hsmbus1.State != HAL_SMBUS_STATE_READY);
//        while(HAL_SMBUS_GetState(&hsmbus1) != HAL_SMBUS_STATE_READY);
//      }
#endif /*TEST_I2C1_SLAVE*/

#ifdef TEST_SMBUS_IMPL
      I2C1_SMBUS_Stack_Task();
#endif	/*TEST_SMBUS_IMPL*/
  }

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSI14
                              |RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.HSI14CalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB|RCC_PERIPHCLK_USART1
                              |RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC_Init(void)
{

  /* USER CODE BEGIN ADC_Init 0 */

  /* USER CODE END ADC_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC_Init 1 */

  /* USER CODE END ADC_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.LowPowerAutoPowerOff = DISABLE;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.DMAContinuousRequests = DISABLE;
  hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = 3;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = 4;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_6;
  sConfig.Rank = 6;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC_Init 2 */

  /* USER CODE END ADC_Init 2 */

}

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
  hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
  hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
  hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_SMBUS_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hsmbus1.Instance = I2C1;
  hsmbus1.Init.Timing = 0x2000090E;
  hsmbus1.Init.AnalogFilter = SMBUS_ANALOGFILTER_ENABLE;
  hsmbus1.Init.OwnAddress1 = 0xA0;
  hsmbus1.Init.AddressingMode = SMBUS_ADDRESSINGMODE_7BIT;
  hsmbus1.Init.DualAddressMode = SMBUS_DUALADDRESS_DISABLE;
  hsmbus1.Init.OwnAddress2 = 0;
  hsmbus1.Init.OwnAddress2Masks = SMBUS_OA2_NOMASK;
  hsmbus1.Init.GeneralCallMode = SMBUS_GENERALCALL_DISABLE;
  hsmbus1.Init.NoStretchMode = SMBUS_NOSTRETCH_DISABLE;
  hsmbus1.Init.PacketErrorCheckMode = SMBUS_PEC_DISABLE;
  hsmbus1.Init.PeripheralMode = SMBUS_PERIPHERAL_MODE_SMBUS_SLAVE;
  hsmbus1.Init.SMBusTimeout = 0x00008061;
  if (HAL_SMBUS_Init(&hsmbus1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */
#ifdef USE_PEC
  hsmbus1.Init.PacketErrorCheckMode = SMBUS_PEC_ENABLE;
#endif /* USE_PEC */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = I2C_TIMING ;//0x20303E5D;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 7;
  hspi2.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi2.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief WWDG Initialization Function
  * @param None
  * @retval None
  */
static void MX_WWDG_Init(void)
{

  /* USER CODE BEGIN WWDG_Init 0 */

  /* USER CODE END WWDG_Init 0 */

  /* USER CODE BEGIN WWDG_Init 1 */

  /* USER CODE END WWDG_Init 1 */
  hwwdg.Instance = WWDG;
  hwwdg.Init.Prescaler = WWDG_PRESCALER_8;
  hwwdg.Init.Window = 0x7F;
  hwwdg.Init.Counter = 0x7F;
  hwwdg.Init.EWIMode = WWDG_EWI_DISABLE;
  if (HAL_WWDG_Init(&hwwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN WWDG_Init 2 */

  /* USER CODE END WWDG_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
#ifndef TEST_I2C1_SLAVE
  HAL_GPIO_WritePin(GPIOC, SW_SEL0_Pin | SW_SEL1_Pin, GPIO_PIN_RESET);
#else
  ///Phu
  HAL_GPIO_WritePin(GPIOC, SW_SEL0_Pin , GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOC, SW_SEL1_Pin, GPIO_PIN_RESET);
#endif /* !TEST_I2C1_SLAVE*/
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, TLC_LATCH_Pin | LED_SHDN_Pin | GD_PWR_CTRL_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
#ifndef TEST_I2C1_SLAVE
  HAL_GPIO_WritePin(GPIOA, KVMSW_EN_Pin | BUTTON_EDID_Pin, GPIO_PIN_RESET);
#else
  ///Phu
  HAL_GPIO_WritePin(GPIOA, KVMSW_EN_Pin , GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOA, BUTTON_EDID_Pin, GPIO_PIN_RESET);
#endif /* !TEST_I2C1_SLAVE*/

  /*Configure GPIO pin : BUTTON1_Pin */
  GPIO_InitStruct.Pin = BUTTON1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(BUTTON1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SW_SEL0_Pin SW_SEL1_Pin */
  GPIO_InitStruct.Pin = SW_SEL0_Pin|SW_SEL1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : BUTTON2_Pin BUTTON3_Pin */
  GPIO_InitStruct.Pin = BUTTON2_Pin | BUTTON3_Pin | BUTTON_EDID_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : TLC_LATCH_Pin TLC_BLANK_Pin GD_PWR_CTRL_Pin */
  GPIO_InitStruct.Pin = LED_SHDN_Pin | GD_PWR_CTRL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : KVMSW_EN_Pin BUTTON_EDID_Pin */
  GPIO_InitStruct.Pin = KVMSW_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : VGA4_PLUG_DT_Pin VGA3_PLUG_DT_Pin VGA2_PLUG_DT_Pin VGA2_PLUG_DTB9_Pin */
  GPIO_InitStruct.Pin = VGA4_PLUG_DT_Pin | VGA3_PLUG_DT_Pin | VGA2_PLUG_DT_Pin | VGA1_PLUG_DT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : BUTTON4_Pin */
  GPIO_InitStruct.Pin = BUTTON4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(BUTTON4_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

///**
// * @brief CRC Initialization Function
// * @param None
// * @retval None
// */
//static void MX_CRC_Init(void)
//{
//
//  /* USER CODE BEGIN CRC_Init 0 */
//
//  /* USER CODE END CRC_Init 0 */
//
//  /* USER CODE BEGIN CRC_Init 1 */
//
//  /* USER CODE END CRC_Init 1 */
//  hcrc.Instance = CRC;
//  hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
//  hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
//  hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
//  hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
//  if (HAL_CRC_Init(&hcrc) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  /* USER CODE BEGIN CRC_Init 2 */
//
//  /* USER CODE END CRC_Init 2 */
//}


void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if(hi2c->Instance==I2C1)
	{
		printf("\r\n Master TxCplt I2C1 \r\n");
	}else if(hi2c->Instance==I2C2)
	{
		printf("\r\n Master TxCplt I2C2 \r\n");
	}
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if(hi2c->Instance==I2C1)
	{
		printf("\r\n Master RxCplt I2C1 \r\n");
//		if(i2c_test_phase == I2C_TEST_PHASE_6)
//		{
//			i2c_test_phase = I2C_TEST_PHASE_1;
//		}
	}else if(hi2c->Instance==I2C2)
	{
		printf("\r\n Master RxCplt I2C2 \r\n");
	}
}

void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if(hi2c->Instance==I2C1)
	{
		printf("\r\n Slave TxCplt I2C1 \r\n");
#ifdef TEST_I2C1_SLAVE
//		isI2C1Receive = true;
#endif /*TEST_I2C1_SLAVE*/
	}else if(hi2c->Instance==I2C2)
	{
		printf("\r\n Slave TxCplt I2C2 \r\n");
	}
}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if(hi2c->Instance==I2C1)
	{
		printf("\r\n Slave RxCplt I2C1 \r\n");
#ifdef TEST_I2C1_SLAVE
//		isI2C1Transmit = true;
#endif /*TEST_I2C1_SLAVE*/
//		if(HAL_I2C_Slave_Transmit_DMA(&hi2c1, (uint8_t *)aI2CSlvTxBuffer, TXSLVBUFFERSIZE) != HAL_OK)
		{

		}
	}else if(hi2c->Instance==I2C2)
	{
		printf("\r\n Slave RxCplt I2C2 \r\n");
	}
}

void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode)
{
	if(hi2c->Instance==I2C1)
	{
		printf("\r\n Addr I2C1 \r\n");
	}else if(hi2c->Instance==I2C2)
	{
		printf("\r\n Addr I2C2 \r\n");
	}
}

void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if(hi2c->Instance==I2C1)
	{
		printf("\r\n Listen TxCplt I2C1 \r\n");
	}else if(hi2c->Instance==I2C2)
	{
		printf("\r\n Listen TxCplt I2C2 \r\n");
	}
}

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if(hi2c->Instance==I2C1)
	{
		printf("\r\n Mem TxCplt I2C1 \r\n");
	}else if(hi2c->Instance==I2C2)
	{
		printf("\r\n Mem TxCplt I2C2 \r\n");
	}
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if(hi2c->Instance==I2C1)
	{
		printf("\r\n Mem RxCplt I2C1 \r\n");
	}else if(hi2c->Instance==I2C2)
	{
		printf("\r\n Mem RxCplt I2C2 \r\n");
	}
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
	if(hi2c->Instance==I2C1)
	{
		printf("\r\n Error I2C1 \r\n");
	}else if(hi2c->Instance==I2C2)
	{
		printf("\r\n Error I2C2 \r\n");
	}
}

void HAL_I2C_AbortCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if(hi2c->Instance==I2C1)
	{
		printf("\r\n Abort I2C1 \r\n");
	}else if(hi2c->Instance==I2C2)
	{
		printf("\r\n Abort I2C2 \r\n");
	}
}

void HAL_I2C_SlaveRxCpltNonReceiveStopCallback(I2C_HandleTypeDef *hi2c)
{
  if(hi2c->Instance==I2C1)
    {
            printf("\r\n RxCpltNonStop \r\n");
            //__HAL_I2C_RESET_HANDLE_STATE(hi2c);
#ifdef TEST_I2C1_SLAVE
//            isI2C1Transmit = true;
#endif /*TEST_I2C1_SLAVE*/
    }
}



///////////////////////////////////////

//void HAL_SMBUS_EV_IRQHandler(SMBUS_HandleTypeDef *hsmbus)
//{
//	if(hsmbus->Instance==I2C1)
//	{
//		printf("\r\n SMB1 EV_IRQ \r\n");
//	}else
//	{
//		printf("\r\n SMB2 EV_IRQ \r\n");
//	}
//}
//
//void HAL_SMBUS_ER_IRQHandler(SMBUS_HandleTypeDef *hsmbus)
//{
//    if(hsmbus->Instance==I2C1)
//    {
//        printf("\r\n SMB1 ER_IRQ \r\n");
//    }else
//    {
//        printf("\r\n SMB2 ER_IRQ \r\n");
//    }
//}=> Defined already
#ifdef TEST_SMBUS_SELF_IMPL
void HAL_SMBUS_MasterTxCpltCallback(SMBUS_HandleTypeDef *hsmbus)
{
//    if(hsmbus->Instance==I2C1)
//    {
//        printf("\r\n SMB1 MasterTxCplt \r\n");
//    }else
//    {
//        printf("\r\n SMB2 MasterTxCplt \r\n");
//    }
	Defer_HAL_SMBUS_MasterTxCpltCallback(hsmbus);
}

void HAL_SMBUS_MasterRxCpltCallback(SMBUS_HandleTypeDef *hsmbus)
{
//    if(hsmbus->Instance==I2C1)
//    {
//        printf("\r\n SMB1 MasterRxCplt \r\n");
//    }else
//    {
//        printf("\r\n SMB2 MasterRxCplt \r\n");
//    }
	Defer_HAL_SMBUS_MasterRxCpltCallback( hsmbus);
}

void HAL_SMBUS_SlaveTxCpltCallback(SMBUS_HandleTypeDef *hsmbus)
{
//	printf("T");
	Defer_HAL_SMBUS_SlaveTxCpltCallback(hsmbus);
}

void HAL_SMBUS_SlaveRxCpltCallback(SMBUS_HandleTypeDef *hsmbus)
{

//	printf("R");
	Defer_HAL_SMBUS_SlaveRxCpltCallback(hsmbus);
}

void HAL_SMBUS_AddrCallback(SMBUS_HandleTypeDef *hsmbus, uint8_t TransferDirection, uint16_t AddrMatchCode)
{

//	printf("A");
	Defer_HAL_SMBUS_AddrCallback(hsmbus,  TransferDirection,  AddrMatchCode);
}

void HAL_SMBUS_ListenCpltCallback(SMBUS_HandleTypeDef *hsmbus)
{

//	printf("C\r\n");
	HAL_SMBUS_ListenCpltCallback(hsmbus);
}
#endif  /*TEST_SMBUS_SELF_IMPL*/
//void HAL_SMBUS_ErrorCallback(SMBUS_HandleTypeDef *hsmbus)
//{
//    if(hsmbus->Instance==I2C1)
//    {
//        printf("\r\n SMB1 Error \r\n");
//    }else
//    {
//        printf("\r\n SMB2 Error \r\n");
//    }
//}=> Defined already




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

#ifdef  USE_FULL_ASSERT
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
