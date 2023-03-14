#include "stdio.h"
#include "IS31FL3218.h"
#include "main.h"

#define GREEN_OFFSET 1
#define RED_OFFSET 2
#define BLUE_OFFSET 3
#define LED_EDID_OFFSET 12

uint32_t lastTimeScanLed;

uint8_t lastChannelSelectLed = 10;
uint8_t lastedidStatus = 1;
uint8_t lastvgaStatus[4];
uint8_t lastusbStatus[4];

unsigned char abm_tab[64] = // 64 step auto to breath
    {
        0, 1, 2, 3, 4, 5, 6, 7,
        8, 10, 12, 14, 16, 18, 20, 22,
        24, 26, 29, 32, 35, 38, 41, 44,
        47, 50, 53, 57, 61, 65, 69, 73,
        77, 81, 85, 89, 94, 99, 104, 109,
        114, 119, 124, 129, 134, 140, 146, 152,
        158, 164, 170, 176, 182, 188, 195, 202,
        209, 216, 223, 230, 237, 244, 251, 255};

extern I2C_HandleTypeDef hi2c2;

//////////////////////////////////////////////////////////////// PRIVATE FUNCTIONS //////////////////////////////////////////////////////////////////
/**
 * @brief set led
 * @param index = 1~16, value
 * @retval None
 */
void set_led(uint8_t index, uint8_t value)
{
  uint8_t myBuf = value;
  HAL_I2C_Mem_Write(&hi2c2, ((uint16_t)ADD_IS32FL2318), index, I2C_MEMADD_SIZE_8BIT, &myBuf, 1, 10); // ALL LED CTROL(Init pwm register)
  myBuf = 0;
  HAL_I2C_Mem_Write(&hi2c2, ((uint16_t)ADD_IS32FL2318), 0x16, I2C_MEMADD_SIZE_8BIT, &myBuf, 1, 10); // UP DATA TO REGISTER18 CHANNELS LED DRIVER EVALUATION BOARD GUIDE
  myBuf = 0x01;
  HAL_I2C_Mem_Write(&hi2c2, ((uint16_t)ADD_IS32FL2318), 0x00, I2C_MEMADD_SIZE_8BIT, &myBuf, 1, 10); // CONFIGURES REGISTER to make Enable
}

/**
 * @brief channel select -> led , set led red indicate which channel to select
 * @param
 * @retval None
 */
void update_led_button()
{
  if (lastChannelSelectLed != get_current_channel())
  {
    lastChannelSelectLed = get_current_channel();// channelSelect;
    for (uint8_t i = 0; i < 4; i++)
    {
      if (get_current_channel() == i)
        set_led(i * 3 + RED_OFFSET, 109);
      else
        set_led(i * 3 + RED_OFFSET, 0);
    }
  }
}

/**
 * @brief led for USB detect
 * @param
 * @retval None
 */
void update_led_usb()
{
  for (uint8_t i = 0; i < 4; i++)
  {
    if (lastusbStatus[i] != get_current_usb(i))
    {
      lastusbStatus[i] = get_current_usb(i);
      if (get_current_usb(i))
        set_led(i * 3 + GREEN_OFFSET, 109);
      else
        set_led(i * 3 + GREEN_OFFSET, 0);
    }
  }
}

/**
 * @brief led for VGA detect
 * @param
 * @retval None
 */
void update_led_vga()
{
  for (uint8_t i = 0; i < 4; i++)
  {
    if (lastvgaStatus[i] != get_current_vga(i) )
    {
      lastvgaStatus[i] = get_current_vga(i);
      if (get_current_vga(i))
        set_led(i * 3 + BLUE_OFFSET, 109);
      else
        set_led(i * 3 + BLUE_OFFSET, 0);
    }
  }
}

/**
 * @brief led for EDID
 * @param
 * @retval None
 */
void update_led_edid()
{
  if (lastedidStatus != get_current_edid())
  {
    lastedidStatus = get_current_edid();
    if (get_current_edid())
    {
      set_led(LED_EDID_OFFSET + BLUE_OFFSET, 0);
      set_led(LED_EDID_OFFSET + 3 + BLUE_OFFSET, 100);
    }
    else
    {

      set_led(LED_EDID_OFFSET + BLUE_OFFSET, 100);
      set_led(LED_EDID_OFFSET + 3 + BLUE_OFFSET, 0);
    }
  }
}

//////////////////////////////////////////////////////////////// PUBLIC FUNCTIONS //////////////////////////////////////////////////////////////////
/**
 * @brief led init call before loop
 * @param None
 * @retval None
 */
void init_IS31FL3218(void)
{
  uint8_t i;
  uint8_t myBuf;

  HAL_GPIO_WritePin(LED_SHDN_GPIO_Port, LED_SHDN_Pin, GPIO_PIN_SET);
  printf("init_IS31FL3218 \r\n");

  //  HAL_I2C_Mem_Write(&hi2c2, ((uint16_t) ADD_IS32FL2318), 0x17, I2C_MEMADD_SIZE_8BIT, &myBuf, 1, 10); //Reset

  myBuf = 0xFF;
  HAL_I2C_Mem_Write(&hi2c2, ((uint16_t)ADD_IS32FL2318), 0x13, I2C_MEMADD_SIZE_8BIT, &myBuf, 1, 10);

  myBuf = 0xFF;
  HAL_I2C_Mem_Write(&hi2c2, ((uint16_t)ADD_IS32FL2318), 0x14, I2C_MEMADD_SIZE_8BIT, &myBuf, 1, 10);

  myBuf = 0xFF;
  HAL_I2C_Mem_Write(&hi2c2, ((uint16_t)ADD_IS32FL2318), 0x15, I2C_MEMADD_SIZE_8BIT, &myBuf, 1, 10);

  for (i = 0x01; i <= 0x12; i++)
  {
    myBuf = 0;
    HAL_I2C_Mem_Write(&hi2c2, ((uint16_t)ADD_IS32FL2318), i, I2C_MEMADD_SIZE_8BIT, &myBuf, 1, 10); // ALL LED CTROL(Init pwm register)
  }

  myBuf = 0;
  HAL_I2C_Mem_Write(&hi2c2, ((uint16_t)ADD_IS32FL2318), 0x16, I2C_MEMADD_SIZE_8BIT, &myBuf, 1, 10); // UP DATA TO REGISTER18 CHANNELS LED DRIVER EVALUATION BOARD GUIDE

  myBuf = 0x01;
  HAL_I2C_Mem_Write(&hi2c2, ((uint16_t)ADD_IS32FL2318), 0x00, I2C_MEMADD_SIZE_8BIT, &myBuf, 1, 10); // CONFIGURES REGISTER to make Enable
}

/**
 * @brief led task call in loop
 * @param None
 * @retval None
 */
void led_task()
{
  if (HAL_GetTick() - lastTimeScanLed > 557)
  {
    lastTimeScanLed = HAL_GetTick();
    update_led_button();
    update_led_usb();
    update_led_vga();
    update_led_edid();
  }
}

/**
 * @brief test led
 * @param None
 * @retval None
 */
void test_LED(void)
{
  uint8_t myBuf, i = 64;
  init_IS31FL3218();
  while (i)
  {
    for (uint8_t j = 1; j <= 0x12; j++) // the loop is write value to pwm register address
    {
      HAL_I2C_Mem_Write(&hi2c2, ((uint16_t)0xA8), j, I2C_MEMADD_SIZE_8BIT, &abm_tab[i - 1], 1, 10); // write pwm value of Automatic breathing
    }
    myBuf = 0x00;
    HAL_I2C_Mem_Write(&hi2c2, ((uint16_t)0xA8), 0x16, I2C_MEMADD_SIZE_8BIT, &myBuf, 1, 10); // updata pwm value to pwm register
    i--;                                                                                    // i minus 1
    printf("i = %d \r\n", i);
    //    delay(10); //delay 10ms
  }
}