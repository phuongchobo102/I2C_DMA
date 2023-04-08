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
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//#define TEST_I2C1_SLAVE 	1
//#define TEST_I2C1_SMBUS_SLAVE 1


#ifdef TEST_I2C1_SMBUS_SLAVE
#define TEST_I2C1_SMBUS_SLAVE 	1
#endif/* TEST_I2C1_SMBUS_SLAVE*/
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





uint8_t tx_buffer[64];     // Variable to store the output data
uint8_t report_buffer[64]; // Variable to receive the report buffer
uint8_t flag = 0;          // Variable to store the button flag
uint8_t flag_rx = 0;       // Variable to store the reception flag

#ifdef TEST_I2C1_SMBUS_SLAVE

bool isI2C1Receive = true;
bool isI2C1Transmit = false;

uint8_t test_tx_buffer[128] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZ123456ABCDEFGHIJKLMNOPQRSTUVWXYZ123456ABCDEFGHIJKLMNOPQRSTUVWXYZ123456ABCDEFGHIJKLMNOPQRSTUVWXYZ123456"};
uint8_t test_rx_buffer[128] = {"abcdefghijklmnopqrstuvwxyz123456"};

typedef struct smbus_rx_struct {
	uint8_t u8_state;
	uint8_t u8_pre_state;
	uint16_t u16_counter;
	uint16_t u16_buffer_size;
	uint16_t u16_buffer_index;
}smbus_rx_t;

typedef struct smbus_tx_struct {
	uint8_t u8_state;
	uint8_t u8_pre_state;
	uint16_t u16_counter;
	uint16_t u16_buffer_size;
	uint16_t u16_buffer_index;
}smbus_tx_t;

typedef struct smbus_instance_struct {
	SMBUS_HandleTypeDef *p_hsmbus_inst;
	uint8_t u8_state;
	uint8_t u8_pre_state;
	uint8_t u8_smb_type;
	uint8_t u8_command;
	smbus_rx_t t_rx;
	smbus_tx_t t_tx;
}smbus_instance_t;

typedef enum smbus_slave_type_enum{
    SMBUS_SLAVE_TYPE_QUICK_CMD,
	SMBUS_SLAVE_TYPE_SEND_BYTE,
	SMBUS_SLAVE_TYPE_RECV_BYTE,
	SMBUS_SLAVE_TYPE_READ_BYTE,
	SMBUS_SLAVE_TYPE_WRITE_BYTE,
	SMBUS_SLAVE_TYPE_READ_BYTES,
	SMBUS_SLAVE_TYPE_WRITE_BYTES,
	SMBUS_SLAVE_TYPE_BLK_READ,
	SMBUS_SLAVE_TYPE_BLK_WRITE,
	SMBUS_SLAVE_TYPE_TOTAL
}smbus_slave_type_e;

typedef enum smbus_slave_state_enum{
    SMBUS_SLAVE_STATE_READY,
    SMBUS_SLAVE_STATE_LISTEN,
    SMBUS_SLAVE_STATE_ADDR_1ST_WRITE,
	SMBUS_SLAVE_STATE_ADDR_1ST_READ,
	SMBUS_SLAVE_STATE_RECEIVED_1ST_BYTE,
	SMBUS_SLAVE_STATE_ADDR_2ND_READ,
    SMBUS_SLAVE_STATE_DO_RX_FOR_WR_CMD,
	SMBUS_SLAVE_STATE_DO_TX_FOR_RD_CMD,
	SMBUS_SLAVE_STATE_WAIT_FOR_LISTENCPLT,
	SMBUS_SLAVE_STATE_LISTENCPLT
}smbus_slave_state_e;

typedef enum smbus_slave_rx_state_enum{
    SMBUS_SLAVE_RX_STATE_1ST,
	SMBUS_SLAVE_RX_STATE_NEXT,
	SMBUS_SLAVE_RX_STATE_END
}smbus_slave_rx_state_e;

typedef enum smbus_slave_tx_state_enum{
    SMBUS_SLAVE_TX_STATE_1ST,
	SMBUS_SLAVE_TX_STATE_NEXT,
	SMBUS_SLAVE_TX_STATE_END,
	SMBUS_SLAVE_TX_STATE_1ST_2BYTES,
	SMBUS_SLAVE_TX_STATE_NEXT_2BYTES,
	SMBUS_SLAVE_TX_STATE_END_2BYTES
}smbus_slave_tx_state_e;

//uint8_t i2C1ReceiveState = SMBUS_SLAVE_STATE_READY;

smbus_instance_t t_smbus_i2c1;// = {.p_hsmbus_inst = &hsmbus1};
#endif /*TEST_I2C1_SMBUS_SLAVE*/

// extern the USB handler
extern USBD_HandleTypeDef hUsbDeviceFS;




uint32_t id_ic = 0;

uint32_t test_id_ic = 0x55aa;
uint8_t channelSelect = 0;
uint8_t edidStatus;
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

uint8_t get_current_edid()
{
  return edidStatus;
}
void set_current_edid(uint8_t value)
{
  edidStatus = value;
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
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
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
//  MX_DMA_Init();
//  MX_USB_DEVICE_Init();
//  MX_ADC_Init();
  MX_I2C1_SMBUS_Init();
//  MX_I2C2_Init();
//  MX_SPI1_Init();
//  MX_SPI2_Init();
//  MX_USART1_UART_Init();
//  MX_CRC_Init();
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
//  system_switch_init();
//  elinkswitch_init();
//  elsgpio_init();
//  authenKVM_init();
//  HAL_ADCEx_Calibration_Start(&hadc);
//  init_IS31FL3218();
//  usb_kvm_switch_init();
//  system_switch_init();
  //    test_LED();
#ifdef ENABLE_WATCHDOG
  MX_WWDG_Init();
#endif

#ifdef TEST_I2C1_SMBUS_SLAVE
  t_smbus_i2c1.p_hsmbus_inst = &hsmbus1;
  t_smbus_i2c1.t_rx.u16_buffer_size = sizeof(test_rx_buffer);
  t_smbus_i2c1.t_rx.u16_counter = 0;
  t_smbus_i2c1.t_tx.u16_buffer_size = sizeof(test_tx_buffer);
  t_smbus_i2c1.t_tx.u16_counter = 0;
#endif/* TEST_I2C1_SMBUS_SLAVE*/
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//    elsgpio_task();
//    led_task();
//    elinkswitch_task();
//    vga_tasks();
//    system_switch_tasks();
//
//    authenKVM();
 
#ifdef ENABLE_WATCHDOG

    system_switch_tasks();
//    HAL_Delay(1);

  //*
      if (HAL_WWDG_Refresh(&WwdgHandle) != HAL_OK)
      {
        Error_Handler();
      }
#endif

#ifdef TEST_I2C1_SMBUS_SLAVE
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

      if(isI2C1Receive)
      {
    	  if(HAL_SMBUS_EnableListen_IT(&hsmbus1) != HAL_OK)
    	  {
				  /* Transfer error in transmission process */
				  Error_Handler();
		  }

//    	  i2C1ReceiveState = SMBUS_SLAVE_STATE_LISTEN;
    	  t_smbus_i2c1.u8_state = SMBUS_SLAVE_STATE_LISTEN;
//		  if(HAL_SMBUS_Slave_Receive_IT(&hsmbus1, (uint8_t *)test_rx_buffer, 1/*sizeof(test_rx_buffer)*/, SMBUS_FIRST_FRAME) != HAL_OK)
//		  {
//				  /* Transfer error in transmission process */
//				  Error_Handler();
//		  }
		  isI2C1Receive = false;
		  /* Device is ready */
		  //while(hsmbus1.State != HAL_SMBUS_STATE_READY);
//          while(HAL_SMBUS_GetState(&hsmbus1) != HAL_SMBUS_STATE_READY);
      }else if(isI2C1Transmit)
      {
        if(HAL_SMBUS_EnableListen_IT(&hsmbus1) != HAL_OK)
    	  {
                          /* Transfer error in transmission process */
                          Error_Handler();
          }
    	  if(HAL_SMBUS_Slave_Transmit_IT(&hsmbus1, (uint8_t *)test_tx_buffer, sizeof(test_tx_buffer), SMBUS_SOFTEND_MODE) != HAL_OK)
		  {
				  /* Transfer error in transmission process */
				  Error_Handler();
		  }
    	  isI2C1Transmit = false;
    	  //while(hsmbus1.State != HAL_SMBUS_STATE_READY);
        while(HAL_SMBUS_GetState(&hsmbus1) != HAL_SMBUS_STATE_READY); 
      }
#endif /*TEST_I2C1_SMBUS_SLAVE*/
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
#ifndef TEST_I2C1_SMBUS_SLAVE
  HAL_GPIO_WritePin(GPIOC, SW_SEL0_Pin | SW_SEL1_Pin, GPIO_PIN_RESET);
#else
  ///Phu
  HAL_GPIO_WritePin(GPIOC, SW_SEL0_Pin , GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOC, SW_SEL1_Pin, GPIO_PIN_RESET);
#endif /* !TEST_I2C1_SMBUS_SLAVE*/
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, TLC_LATCH_Pin | LED_SHDN_Pin | GD_PWR_CTRL_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
#ifndef TEST_I2C1_SMBUS_SLAVE
  HAL_GPIO_WritePin(GPIOA, KVMSW_EN_Pin | BUTTON_EDID_Pin, GPIO_PIN_RESET);
#else
  ///Phu
  HAL_GPIO_WritePin(GPIOA, KVMSW_EN_Pin , GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOA, BUTTON_EDID_Pin, GPIO_PIN_RESET);
#endif /* !TEST_I2C1_SMBUS_SLAVE*/

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
  GPIO_InitStruct.Pull = GPIO_NOPULL;
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
//		isI2C1Receive = true;
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
//		isI2C1Transmit = true;
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
//            isI2C1Transmit = true;
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

#ifdef TEST_I2C1_SMBUS_SLAVE
void HAL_SMBUS_MasterTxCpltCallback(SMBUS_HandleTypeDef *hsmbus)
{
    if(hsmbus->Instance==I2C1)
    {
        printf("\r\n SMB1 MasterTxCplt \r\n");
    }else
    {
        printf("\r\n SMB2 MasterTxCplt \r\n");
    }
}

void HAL_SMBUS_MasterRxCpltCallback(SMBUS_HandleTypeDef *hsmbus)
{
    if(hsmbus->Instance==I2C1)
    {
        printf("\r\n SMB1 MasterRxCplt \r\n");
    }else
    {
        printf("\r\n SMB2 MasterRxCplt \r\n");
    }
}

void HAL_SMBUS_SlaveTxCpltCallback(SMBUS_HandleTypeDef *hsmbus)
{
    if(hsmbus->Instance==I2C1)
    {
//        printf("Tx\r\n");
//        if(i2C1ReceiveState == SMBUS_SLAVE_STATE_DO_READ)
//        {
//        	i2C1ReceiveState = SMBUS_SLAVE_STATE_WAIT_FOR_LISTENCPLT;
//        }
        if(t_smbus_i2c1.u8_state == SMBUS_SLAVE_STATE_ADDR_2ND_READ)
        {

			//ToDo:Should check command to determin whether this is Read byte, Read word, Read word 64
//        	printf("1");//Reached
			//For this case we simply assume this is get edid(READ 128 BYTE and do transmit 128 byte
			if(t_smbus_i2c1.u8_command == SMBUS_SLAVE_TYPE_READ_BYTES)
			{
//				printf("2");//Reached
				/*Reduce 2, because this is Read word*/
				t_smbus_i2c1.t_tx.u16_counter -= 2;
				if(t_smbus_i2c1.t_tx.u16_counter == 0)
				{
					printf("A");
					/*End receive here*/
					t_smbus_i2c1.t_tx.u8_pre_state = t_smbus_i2c1.t_tx.u8_state;
					/*Mark end receive*/
					t_smbus_i2c1.t_tx.u8_state = SMBUS_SLAVE_TX_STATE_END_2BYTES;
					/*Do nothing and wait for complete*/

					t_smbus_i2c1.u8_pre_state = t_smbus_i2c1.u8_state;
					t_smbus_i2c1.u8_state = SMBUS_SLAVE_STATE_WAIT_FOR_LISTENCPLT;
				}else{
//					printf("B");
					if(t_smbus_i2c1.t_tx.u8_state == SMBUS_SLAVE_TX_STATE_1ST_2BYTES)
					{
						t_smbus_i2c1.t_tx.u8_pre_state = t_smbus_i2c1.t_tx.u8_state ;
						t_smbus_i2c1.t_tx.u8_state = SMBUS_SLAVE_TX_STATE_NEXT_2BYTES;
					}
					/*Increase index to 2 step to store data in buffer*/
					t_smbus_i2c1.t_tx.u16_buffer_index += 2;
					if(HAL_SMBUS_Slave_Transmit_IT(&hsmbus1, (uint8_t *)(&test_tx_buffer[t_smbus_i2c1.t_tx.u16_buffer_index]), 2, SMBUS_LAST_FRAME_NO_PEC) != HAL_OK)
					{
						Error_Handler();
					}
				}

			}else if(t_smbus_i2c1.u8_command == SMBUS_SLAVE_TYPE_READ_BYTE)
			{
				printf("3");
				/*Reduce 1, because this is Read word*/
				t_smbus_i2c1.t_tx.u16_counter--;
				if(t_smbus_i2c1.t_tx.u16_counter == 0)
				{
					/*End receive here*/
					t_smbus_i2c1.t_tx.u8_pre_state = t_smbus_i2c1.t_tx.u8_state;
					/*Mark end receive*/
					t_smbus_i2c1.t_tx.u8_state = SMBUS_SLAVE_TX_STATE_END;
					/*Do nothing and wait for complete*/

					t_smbus_i2c1.u8_pre_state = t_smbus_i2c1.u8_state;
					t_smbus_i2c1.u8_state = SMBUS_SLAVE_STATE_WAIT_FOR_LISTENCPLT;
				}else{
					/*Error here, should be 0*/
				}
			}else
			{
				printf("Tx Excpt 1\r\n");
			}
        }else
        {
			t_smbus_i2c1.t_tx.u16_counter--;
			if(t_smbus_i2c1.t_tx.u16_counter == 0)
			{
				printf("4");
				/*End receive here*/
				t_smbus_i2c1.t_tx.u8_pre_state = t_smbus_i2c1.t_tx.u8_state;
				/*Mark end receive*/
				t_smbus_i2c1.t_tx.u8_state = SMBUS_SLAVE_RX_STATE_END;
				//Warning: need to check index
				//t_smbus_i2c1.t_rx.u16_buffer_index
				if(t_smbus_i2c1.u8_state == SMBUS_SLAVE_STATE_ADDR_1ST_WRITE )
				{
					/*Check whether read/write command byte or data byte of send byte, by wait for next callback
					 * from either HAL_SMBUS_AddrCallback or HAL_SMBUS_ListenCpltCallback */
					t_smbus_i2c1.u8_command = test_rx_buffer[t_smbus_i2c1.t_rx.u16_buffer_index];
					t_smbus_i2c1.u8_pre_state = t_smbus_i2c1.u8_state;
					t_smbus_i2c1.u8_state = SMBUS_SLAVE_STATE_RECEIVED_1ST_BYTE;
				}
			}else{
				printf("5");
				if(t_smbus_i2c1.t_tx.u8_state == SMBUS_SLAVE_RX_STATE_1ST)
				{
					t_smbus_i2c1.t_tx.u8_pre_state = t_smbus_i2c1.t_tx.u8_state ;
					t_smbus_i2c1.t_tx.u8_state = SMBUS_SLAVE_RX_STATE_NEXT;
				}
				/*Increase index to store data in buffer*/
				t_smbus_i2c1.t_rx.u16_buffer_index++;
				if(HAL_SMBUS_Slave_Receive_IT(&hsmbus1, (uint8_t *)(&test_rx_buffer[t_smbus_i2c1.t_rx.u16_buffer_index]), 1/*sizeof(test_rx_buffer)*/, SMBUS_SOFTEND_MODE) != HAL_OK)
				{
						  /* Transfer error in transmission process */
					Error_Handler();
				}
			}

        }
    }else
    {
        printf("\r\n SMB2 SlaveTxCplt \r\n");
    }
}

void HAL_SMBUS_SlaveRxCpltCallback(SMBUS_HandleTypeDef *hsmbus)
{
    if(hsmbus->Instance==I2C1)
    {
//        printf("Rx\r\n");
        if(t_smbus_i2c1.u8_state == SMBUS_SLAVE_STATE_ADDR_1ST_WRITE )
		{
			/*Check whether read/write command byte or data byte of send byte, by wait for next callback
			 * from either HAL_SMBUS_AddrCallback or HAL_SMBUS_ListenCpltCallback */
			t_smbus_i2c1.u8_command = test_rx_buffer[t_smbus_i2c1.t_rx.u16_buffer_index];
			t_smbus_i2c1.u8_pre_state = t_smbus_i2c1.u8_state;
			t_smbus_i2c1.u8_state = SMBUS_SLAVE_STATE_RECEIVED_1ST_BYTE;

			//////
			/*For this case we just assume that there should be Write Word or Read Word.
			 * After this, If the AddrCallBack rises then It must be Read Word, If SlaveRxCpltCallback
			 * rise then it must be Write Word
			 * */
			t_smbus_i2c1.t_rx.u8_state = SMBUS_SLAVE_RX_STATE_1ST;
			t_smbus_i2c1.t_rx.u16_counter = 128;
			t_smbus_i2c1.t_rx.u16_buffer_index = 0;
			if(HAL_SMBUS_Slave_Receive_IT(&hsmbus1, (uint8_t *)(&test_rx_buffer[t_smbus_i2c1.t_rx.u16_buffer_index]), 2/*sizeof(test_rx_buffer)*/, SMBUS_LAST_FRAME_NO_PEC ) != HAL_OK)
			{
					  /* Transfer error in transmission process */
				Error_Handler();
			}


		}else if((t_smbus_i2c1.u8_state == SMBUS_SLAVE_STATE_RECEIVED_1ST_BYTE ) && \
				(t_smbus_i2c1.u8_pre_state == SMBUS_SLAVE_STATE_ADDR_1ST_WRITE ))
		{
			/*This is write byte or write word */
			//Todo: For this case just assume that is write word accordingly SMBus
			t_smbus_i2c1.u8_pre_state = SMBUS_SLAVE_STATE_RECEIVED_1ST_BYTE;
			t_smbus_i2c1.u8_state = SMBUS_SLAVE_STATE_DO_RX_FOR_WR_CMD;

			t_smbus_i2c1.t_rx.u8_state = SMBUS_SLAVE_RX_STATE_1ST;
			t_smbus_i2c1.t_rx.u16_counter -= 2;
			t_smbus_i2c1.t_rx.u16_buffer_index = 2;

			if(HAL_SMBUS_Slave_Receive_IT(&hsmbus1, (uint8_t *)(&test_rx_buffer[t_smbus_i2c1.t_rx.u16_buffer_index]), 2/*sizeof(test_rx_buffer)*/, SMBUS_LAST_FRAME_NO_PEC ) != HAL_OK)
			{
					  /* Transfer error in transmission process */
				Error_Handler();
			}

		}else if(t_smbus_i2c1.u8_state == SMBUS_SLAVE_STATE_DO_RX_FOR_WR_CMD )
		{
			t_smbus_i2c1.t_rx.u16_counter -= 2;
			if(t_smbus_i2c1.t_rx.u16_counter == 0)
			{
				/*End receive here */
				t_smbus_i2c1.t_rx.u8_pre_state = t_smbus_i2c1.t_rx.u8_state;
				/*Mark end receive */
				t_smbus_i2c1.t_rx.u8_state = SMBUS_SLAVE_RX_STATE_END;

				t_smbus_i2c1.u8_pre_state = t_smbus_i2c1.u8_state;
				t_smbus_i2c1.u8_state = SMBUS_SLAVE_STATE_WAIT_FOR_LISTENCPLT;
			}else
			{
				/*Continue to receive word*/
				if(t_smbus_i2c1.t_rx.u8_state == SMBUS_SLAVE_RX_STATE_1ST)
				{
					t_smbus_i2c1.t_rx.u8_pre_state = t_smbus_i2c1.t_rx.u8_state ;
					t_smbus_i2c1.t_rx.u8_state = SMBUS_SLAVE_RX_STATE_NEXT;
				}
				/*Increase index to store data in buffer*/
				t_smbus_i2c1.t_rx.u16_counter -= 2;
				t_smbus_i2c1.t_rx.u16_buffer_index += 2;
				if(HAL_SMBUS_Slave_Receive_IT(&hsmbus1, (uint8_t *)(&test_rx_buffer[t_smbus_i2c1.t_rx.u16_buffer_index]), 2/*sizeof(test_rx_buffer)*/, SMBUS_LAST_FRAME_NO_PEC) != HAL_OK)
				{
						  /* Transfer error in transmission process */
					Error_Handler();
				}

			}
		}else if(t_smbus_i2c1.u8_state == SMBUS_SLAVE_STATE_ADDR_1ST_READ )
		{
			/*This is receive byte*/
			//ToDo: store this byte to process
			t_smbus_i2c1.u8_pre_state = t_smbus_i2c1.u8_state;
			t_smbus_i2c1.u8_state = SMBUS_SLAVE_STATE_RECEIVED_1ST_BYTE;
		}else
		{
			printf("Rx Unk\r\n");
		}


//        isI2C1Transmit = true;
//        t_smbus_i2c1.t_rx.u16_counter--;
//        if(t_smbus_i2c1.t_rx.u16_counter == 0)
//        {
//        	/*End receive here*/
//        	t_smbus_i2c1.t_rx.u8_pre_state = t_smbus_i2c1.t_rx.u8_state;
//        	/*Mark end receive*/
//        	t_smbus_i2c1.t_rx.u8_state = SMBUS_SLAVE_RX_STATE_END;
//        	//Warning: need to check index
//        	//t_smbus_i2c1.t_rx.u16_buffer_index
//        	if(t_smbus_i2c1.u8_state == SMBUS_SLAVE_STATE_ADDR_2ND_READ )
//        	{
//        		/*Check whether read/write command byte or data byte of send byte, by wait for next callback
//        		 * from either HAL_SMBUS_AddrCallback or HAL_SMBUS_ListenCpltCallback */
//        		t_smbus_i2c1.u8_command = test_rx_buffer[t_smbus_i2c1.t_rx.u16_buffer_index];
//        		t_smbus_i2c1.u8_pre_state = t_smbus_i2c1.u8_state;
//        		t_smbus_i2c1.u8_state = SMBUS_SLAVE_STATE_DO_TX_FOR_RD_CMD;
//
//
//        	}else if(t_smbus_i2c1.u8_state == SMBUS_SLAVE_STATE_ADDR_1ST_READ )
//        	{
//        		/*This is receive byte*/
//        		//ToDo: store this byte to process
//        		t_smbus_i2c1.u8_pre_state = t_smbus_i2c1.u8_state;
//        		t_smbus_i2c1.u8_state = SMBUS_SLAVE_STATE_RECEIVED_1ST_BYTE;
//        	}
//        }else{
//        	if(t_smbus_i2c1.t_rx.u8_state == SMBUS_SLAVE_RX_STATE_1ST)
//        	{
//        		t_smbus_i2c1.t_rx.u8_pre_state = t_smbus_i2c1.t_rx.u8_state ;
//        		t_smbus_i2c1.t_rx.u8_state = SMBUS_SLAVE_RX_STATE_NEXT;
//        	}
//        	/*Increase index to store data in buffer*/
//        	t_smbus_i2c1.t_rx.u16_buffer_index++;
//        	if(HAL_SMBUS_Slave_Receive_IT(&hsmbus1, (uint8_t *)(&test_rx_buffer[t_smbus_i2c1.t_rx.u16_buffer_index]), 1/*sizeof(test_rx_buffer)*/, SMBUS_SOFTEND_MODE) != HAL_OK)
//			{
//					  /* Transfer error in transmission process */
//				Error_Handler();
//			}
//        }




//        if(i2C1ReceiveState == SMBUS_SLAVE_STATE_ADDR_1ST_WRITE)
//        {
//        	i2C1ReceiveState = SMBUS_SLAVE_STATE_RECEIVED_1ST_BYTE;
//        	//ToDo: Filter command here
//        	if(HAL_SMBUS_EnableListen_IT(&hsmbus1) != HAL_OK)
//        	{
//				  /* Transfer error in transmission process */
//				  Error_Handler();
//        	}
//
//        }
//        else if(i2C1ReceiveState == SMBUS_SLAVE_STATE_DO_WRITE)
//        {
//        	i2C1ReceiveState = SMBUS_SLAVE_STATE_WAIT_FOR_LISTENCPLT;
//        }else
//        {
//        	 printf("\r\n Err \r\n");
//        }
    }else
    {
        printf("\r\n SMB2 SlaveRxCplt \r\n");
    }
}

void HAL_SMBUS_AddrCallback(SMBUS_HandleTypeDef *hsmbus, uint8_t TransferDirection, uint16_t AddrMatchCode)
{
    if(hsmbus->Instance==I2C1)
    {
//        printf("\r\ndr=%d,Addr=0x%x\r\n",TransferDirection,AddrMatchCode);
//    	printf("\r\ndr=%d,Addr=0x%x\r\n",TransferDirection,AddrMatchCode);
        if(TransferDirection == 0)
        {
//        	printf("Wr ");
            /* Write direction*/
//            if(i2C1ReceiveState == SMBUS_SLAVE_STATE_LISTEN)
        	if(t_smbus_i2c1.u8_state == SMBUS_SLAVE_STATE_LISTEN)
            {
//            	printf("1\r\n");
//            	i2C1ReceiveState = SMBUS_SLAVE_STATE_ADDR_1ST_WRITE;
            	t_smbus_i2c1.u8_pre_state = t_smbus_i2c1.u8_state;
            	t_smbus_i2c1.u8_state = SMBUS_SLAVE_STATE_ADDR_1ST_WRITE;
            	t_smbus_i2c1.t_rx.u8_state = SMBUS_SLAVE_RX_STATE_1ST;
            	t_smbus_i2c1.t_rx.u16_counter = 1;
            	t_smbus_i2c1.t_rx.u16_buffer_index = 0;
				if(HAL_SMBUS_Slave_Receive_IT(&hsmbus1, (uint8_t *)test_rx_buffer, 1/*sizeof(test_rx_buffer)*/, SMBUS_FIRST_FRAME ) != HAL_OK)
				{
						  /* Transfer error in transmission process */
					Error_Handler();
				}
            }else /*if(t_smbus_i2c1.u8_state == SMBUS_SLAVE_STATE_RECEIVED_1ST_BYTE)*/
        	{
            	/*This is illegad state according to SMBUS*/
//            	i2C1ReceiveState = SMBUS_SLAVE_STATE_DO_WRITE;
//            	if(HAL_SMBUS_Slave_Receive_IT(&hsmbus1, (uint8_t *)test_rx_buffer, 1/*sizeof(test_rx_buffer)*/, SMBUS_SOFTEND_MODE) != HAL_OK)
//				{
//						  /* Transfer error in transmission process */
//					Error_Handler();
//				}
//            	printf("Err:Recv wr dir after 1st byte,state=%d\r\n",t_smbus_i2c1.u8_state);
            	/*Reset to listen state*/
            	t_smbus_i2c1.u8_state = SMBUS_SLAVE_STATE_LISTEN;
        	}
//        	else if(i2C1ReceiveState == SMBUS_SLAVE_STATE_ADDR_1ST_WRITE)
//        	{
//        		printf("sd1stfr\r\n");
//        		i2C1ReceiveState = SMBUS_SLAVE_STATE_ADDR_2ND_WRITE;
//        		if(HAL_SMBUS_Slave_Receive_IT(&hsmbus1, (uint8_t *)test_rx_buffer, 1/*sizeof(test_rx_buffer)*/, SMBUS_NEXT_FRAME) != HAL_OK)
//				{
//						  /* Transfer error in transmission process */
//					Error_Handler();
//				}
//        	}else
//        	{
//
//        		printf("\r\n nohl,state=%d\r\n",i2C1ReceiveState);
//        	}

        }else
        {
            /* Read direction */
//        	printf("Rd ");
//        	if(i2C1ReceiveState == I2C1RECEIVESTATE_RECEIVED_CMD)
        	if(t_smbus_i2c1.u8_state == SMBUS_SLAVE_STATE_RECEIVED_1ST_BYTE)
        	{
//        		printf("1\r\n");
        		t_smbus_i2c1.u8_pre_state = t_smbus_i2c1.u8_state;
        		t_smbus_i2c1.u8_state = SMBUS_SLAVE_STATE_ADDR_2ND_READ;

        		//ToDo:Should check command to determine whether this is Read byte, Read word, Read word 64

        		//For this case we simply assume this is get edid(READ 128 BYTE and do transmit 128 byte
        		t_smbus_i2c1.u8_command = SMBUS_SLAVE_TYPE_READ_BYTES;

        		t_smbus_i2c1.t_tx.u16_counter = 128;
        		t_smbus_i2c1.t_tx.u16_buffer_index = 0;
        		t_smbus_i2c1.t_tx.u8_state = SMBUS_SLAVE_TX_STATE_1ST_2BYTES;


        		if(HAL_SMBUS_Slave_Transmit_IT(&hsmbus1, (uint8_t *)test_tx_buffer, 2, SMBUS_LAST_FRAME_NO_PEC) != HAL_OK)
				{
					  /* Transfer error in transmission process */
					  Error_Handler();
				}

        	}
        	else if(t_smbus_i2c1.u8_state == SMBUS_SLAVE_STATE_LISTEN)
        	{
//        		printf("2\r\n");
        		/*This is receive byte case*/
        		t_smbus_i2c1.u8_pre_state = t_smbus_i2c1.u8_state;
        		t_smbus_i2c1.u8_state = SMBUS_SLAVE_STATE_ADDR_1ST_READ;
        		t_smbus_i2c1.u8_smb_type = SMBUS_SLAVE_TYPE_RECV_BYTE;

        		//ToDo: Get appropritate byte and lay on transmit buffer in here

        		t_smbus_i2c1.t_tx.u16_counter = 1;
        		t_smbus_i2c1.t_tx.u16_buffer_index = 0;
        		if(HAL_SMBUS_Slave_Transmit_IT(&hsmbus1, (uint8_t *)test_tx_buffer, 1, SMBUS_FIRST_AND_LAST_FRAME_NO_PEC) != HAL_OK)
				  {
						  /* Transfer error in transmission process */
						  Error_Handler();
				  }
        	}else if(t_smbus_i2c1.u8_state == SMBUS_SLAVE_STATE_ADDR_1ST_WRITE){
        		printf("3\r\n");
        		t_smbus_i2c1.u8_pre_state = t_smbus_i2c1.u8_state;
				t_smbus_i2c1.u8_state = SMBUS_SLAVE_STATE_ADDR_2ND_READ;

				//ToDo:Should check command to determine whether this is Read byte, Read word, Read word 64

				//For this case we simply assume this is get edid(READ 128 BYTE and do transmit 128 byte
				t_smbus_i2c1.u8_command = SMBUS_SLAVE_TYPE_READ_BYTES;

				t_smbus_i2c1.t_tx.u16_counter = 128;
				t_smbus_i2c1.t_tx.u16_buffer_index = 0;
				t_smbus_i2c1.t_tx.u8_state = SMBUS_SLAVE_TX_STATE_1ST_2BYTES;


				if(HAL_SMBUS_Slave_Transmit_IT(&hsmbus1, (uint8_t *)test_tx_buffer, 2, SMBUS_LAST_FRAME_NO_PEC) != HAL_OK)
				{
					  /* Transfer error in transmission process */
					  Error_Handler();
				}
        	}else{
        		printf("4\r\n");
        	}
        }
//        HAL_SMBUS_DisableListen_IT(hsmbus);
//        isI2C1Transmit = true;
    }else
    {
        printf("\r\n SMB2 Addr \r\n");
    }
}

void HAL_SMBUS_ListenCpltCallback(SMBUS_HandleTypeDef *hsmbus)
{
    if(hsmbus->Instance==I2C1)
    {
        printf("LstC\r\n");

		if(t_smbus_i2c1.u8_state == SMBUS_SLAVE_STATE_WAIT_FOR_LISTENCPLT)
		{
//			printf("Cplt\r\n");
			printf("Cplt ");
			if(t_smbus_i2c1.u8_pre_state ==  SMBUS_SLAVE_STATE_DO_RX_FOR_WR_CMD)
			{
				printf("RX for Wr CMD\r\n");
			}else if(t_smbus_i2c1.u8_pre_state == SMBUS_SLAVE_STATE_ADDR_2ND_READ)
			{
				printf("TX for Rd CMD\r\n");
			}else if(t_smbus_i2c1.u8_command == SMBUS_SLAVE_TYPE_READ_BYTE)
			{
				printf("Read byte\r\n");
			}else
			{
				printf("\r\n");
			}
			t_smbus_i2c1.u8_pre_state = SMBUS_SLAVE_STATE_LISTENCPLT;
			t_smbus_i2c1.u8_state = SMBUS_SLAVE_STATE_LISTEN;
		}else if(t_smbus_i2c1.u8_state == SMBUS_SLAVE_STATE_RECEIVED_1ST_BYTE)
        {
			printf("SendByte Case\r\n");
			t_smbus_i2c1.u8_pre_state = SMBUS_SLAVE_STATE_LISTENCPLT;
			t_smbus_i2c1.u8_state = SMBUS_SLAVE_STATE_LISTEN;
        }else
        {
        	printf("Unknown state=%d, pre-state=%d\r\n",t_smbus_i2c1.u8_state , t_smbus_i2c1.u8_pre_state);
        }
		isI2C1Receive = true;
//        if(i2C1ReceiveState == SMBUS_SLAVE_STATE_ADDR_1ST_WRITE)
//        {
////        	if(HAL_SMBUS_Slave_Receive_IT(&hsmbus1, (uint8_t *)test_rx_buffer, 1/*sizeof(test_rx_buffer)*/, SMBUS_NEXT_FRAME) != HAL_OK)
////			{
////					  /* Transfer error in transmission process */
////				Error_Handler();
////			}
//        	i2C1ReceiveState = SMBUS_SLAVE_STATE_LISTEN;
//        	isI2C1Receive = true;
//        	printf("1st\r\n");
//        }if(i2C1ReceiveState == SMBUS_SLAVE_STATE_ADDR_2ND_WRITE)
//        {
////        	if(HAL_SMBUS_Slave_Receive_IT(&hsmbus1, (uint8_t *)test_rx_buffer, 1/*sizeof(test_rx_buffer)*/, SMBUS_NEXT_FRAME) != HAL_OK)
////			{
////					  /* Transfer error in transmission process */
////				Error_Handler();
////			}
//        	i2C1ReceiveState = SMBUS_SLAVE_STATE_LISTEN;
//        	isI2C1Receive = true;
//        	printf("2nd\r\n");
//        }else if(i2C1ReceiveState == SMBUS_SLAVE_STATE_WAIT_FOR_LISTENCPLT)
//        {
//
//        }
    }else
    {
        printf("\r\n SMB2 ListenCplt \r\n");
    }
}
#endif /*TEST_I2C1_SMBUS_SLAVE*/
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
