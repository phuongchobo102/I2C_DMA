#include "vga.h"
#include "stdio.h"
#include "string.h"

uint32_t lastTimeTaskEDID;

uint8_t i2c1ValueBuff128[128];
uint8_t flashBufferVGA128[128];
uint32_t flashBuff132[132];
uint8_t ret;
uint8_t i;
uint8_t flagI2CRxCplt = 0;
uint8_t iBufferCounter = 0;
uint8_t flagSlaveRxCplt = 0;
union crcValue
{
  uint32_t crcValue32;
  uint8_t crcValue8[4];
} crcValue;

//////////////////////////////////////////////////////////////// LOCAL FUNCTIONS //////////////////////////////////
/*
//  Description: READ I2C FROM vga device, compare first 5  bytes = 0x00ffffffff
*/
uint8_t is_VGA_detect()
{
  
  uint8_t ret;
  uint8_t compareBuffer[5] = {0x00, 0xff, 0xff, 0xff, 0xff};
  uint8_t c[1];
  
  if (iBufferCounter < 5)
  {
    ret = HAL_I2C_Master_Receive(&hi2c2, VGA_I2C_ADDRESS << 1, c, 1, 100); // TODO: change to ReceiveDMA
    if (ret == HAL_OK)
    {
      if (compareBuffer[iBufferCounter] == c[0])
      {
        i2c1ValueBuff128[iBufferCounter] = c[0];
        iBufferCounter++;
      }
      else
      {
        iBufferCounter = 0;
      }
    }
  }
  else if (iBufferCounter < 128)
  {
    ret = HAL_I2C_Master_Receive(&hi2c2, VGA_I2C_ADDRESS << 1, i2c1ValueBuff128 + iBufferCounter , 120, 100); // TODO: change to ReceiveDMA
    if (ret == HAL_OK)
    {
//      i2c1ValueBuff128[iBufferCounter] = c[0];
//      iBufferCounter++;
      iBufferCounter = 0;
      return 1;
    }
  }
  else
  { // collect 128 byte buffer done
    iBufferCounter = 0;
    return 1;
    
  }
  return 0;
  
  // #if defined(I2C2_DMA_ENABLE) && (I2C2_DMA_ENABLE == 1)
  //   ret = HAL_I2C_Master_Receive_DMA(&hi2c2, VGA_I2C_ADDRESS << 1, i2c1ValueBuff128, VGA_EDID_BYTE); // TODO: change to ReceiveDMA
  
  // #else
  //   ret = HAL_I2C_Master_Receive(&hi2c2, VGA_I2C_ADDRESS << 1, i2c1ValueBuff128, VGA_EDID_BYTE, 100); // TODO: change to ReceiveDMA
  //   if (ret == HAL_OK)
  //   {
  //     for (uint8_t i = 0; i < VGA_EDID_BYTE; i++) // print
  //     {
  //       printf("0x%02x ", i2c1ValueBuff128[i]);
  //       if (i % 8 == 7)
  //         printf("\r\n");
  //     }
  //     ret = strncmp(i2c1ValueBuff128, compareBuffer, 5);
  //     if (ret == 0)
  //     { // thid is VGA device
  //       printf("compare OK\r\n");
  //       return 1;
  //     }
  //   }
  // #endif
  
  //   //#endif /*defined(I2C2_DMA_ENABLE) && (I2C2_DMA_ENABLE == 1)*/
  //   return 0;
}

// void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *I2cHandle)
//{
//     /* Toggle LED2: Transfer in reception process is correct */
//     printf("I2C RX complete \r\n");
// }

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *I2cHandle)
{
  /** Error_Handler() function is called when error occurs.
  * 1- When Slave don't acknowledge it's address, Master restarts communication.
  * 2- When Master don't acknowledge the last data transferred, Slave don't care in this example.
  */
  if (HAL_I2C_GetError(I2cHandle) != HAL_I2C_ERROR_AF)
  {
    printf("I2C Error call back \r\n");
  }
}
/*
//  function:       bufer_compare :
//  parameters:             2 buffer
//  returns:            0 if same, >1 if different
//  description:     check buffer
*/
uint8_t bufer_compare(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t bfLen)
{
  while (bfLen--)
  {
    /* code */
    if ((*pBuffer1) != (*pBuffer2))
    {
      return bfLen;
    }
    pBuffer1++;
    pBuffer2++;
  }
  return 0;
}

/*
//  function:       read_flash_checked :
//  parameters:             none
//  returns:            0 if read and check fail
//  description:     read flash and check crc
*/
uint8_t read_flash_checked()
{
  Flash_Read_Data(START_FLASH_ADDR, (uint32_t *)flashBuff132, 132);
  for (i = 0; i < 132; i++) // Read 132 bytes and distribute them to buffer 128 & buffer 4
  {
    if (i < 128)
      flashBufferVGA128[i] = (uint8_t)flashBuff132[i];
    else
      crcValue.crcValue8[i - 128] = (uint8_t)flashBuff132[i];
  }
  //  check crc
  uint32_t tmpCRC = HAL_CRC_Accumulate(&hcrc, (uint32_t *)flashBufferVGA128, 128);
  if (tmpCRC != crcValue.crcValue32)
  {
    printf("fail check crc at read flash \r\n");
    return 0;
  }
  //    printf("correct crcValue\r\n");
  return 1;
}

/*
//  function: store_info_vga_flash :
//  parameters: buffer 128 bytes
//  returns: none - should return result
//  description:     store info to flash
*/
uint8_t store_info_vga_flash(uint8_t *bufferVGA)
{
  uint32_t buff132[132];
  printf("store_info_vga_flash\r\n");
  crcValue.crcValue32 = HAL_CRC_Accumulate(&hcrc, (uint32_t *)bufferVGA, sizeof(bufferVGA));
  for (uint8_t i = 0; i < 132; i++)
  {
    if (i < VGA_EDID_BYTE)
      buff132[i] = bufferVGA[i];
    else
      buff132[i] = crcValue.crcValue8[i - 128];
  }
  Flash_Write_Data(START_FLASH_ADDR, (uint32_t *)buff132, 132);
  printf("Write data to flash \r\n");
  if (!read_flash_checked())
  { // if check flash fail
    printf("flash write check failed 1\n\r");
    Flash_Write_Data(START_FLASH_ADDR, (uint32_t *)buff132, 132);
    if (!read_flash_checked())
    { // re write flash fail
      printf("flash write check failed 2\n\r");
      // raise evetn write fail
      return 0;
    }
  }
  return 1;
}

//////////////////////////////////////////////////////////////// PUBLIC FUNCTIONS //////////////////////////////////////////////////////////////////
/*
//  function: vga_init :
//  parameters: none
//  returns: none
//  description:     init VGA
*/
void vga_init()
{
  //    uint8_t ret = HAL_I2C_IsDeviceReady(&hi2c2, (uint16_t)(VGA_I2C_ADDRESS << 1), 3, 5);
  //    if (ret == HAL_OK)
  //    {
  ////        is_VGA_detect();
  ////        read_flash_checked();
  ////        if (bufer_compare(flashBufferVGA128, i2c1ValueBuff128))
  ////        {
  ////            printf("Diff found \r\n");
  ////            store_info_vga_flash(i2c1ValueBuff128);
  ////        }
  ////        else
  ////        {
  ////            printf("Diff not found \r\n");
  ////        }
  //    }
}

/*
//  function: vga_tasks :
//  parameters: none
//  returns: none
//  description:
*/
void vga_tasks()
{
  uint8_t c;
  if (HAL_GetTick() - lastTimeTaskEDID > 0)
  {
    lastTimeTaskEDID = HAL_GetTick();
    if (is_VGA_detect()) // if have VGA
    {
      read_flash_checked();
      if (bufer_compare(flashBufferVGA128, i2c1ValueBuff128, 128))
      {                                         // if VGA != localBuff
        store_info_vga_flash(i2c1ValueBuff128); // storage new VGA EDID
      }
    }
    
    //    if (flagI2CRxCplt)
    //    {
    //      flagI2CRxCplt = 0;
    //      printf("Rx complete \r\n");
    //    }
    
    // if(HAL_I2C_AddrCallback()){
    //   printf("I2C callback added\r\n");
    // }
             HAL_I2C_Slave_Transmit(&hi2c1, (uint8_t *)i2c1ValueBuff128, 128, 100);

    if (HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_READY)
    {
      // if (HAL_I2C_Slave_Receive(&hi2c1, &c, 1, 10) == HAL_OK)
      // {
      //   HAL_I2C_Slave_Transmit(&hi2c1, (uint8_t *)i2c1ValueBuff128, 128, 100);
      // }
      // HAL_I2C_Slave_Receive_IT(&hi2c1, &c, 1);
      HAL_I2C_Slave_Transmit_IT(&hi2c1, "hello", 5);
      if(flagSlaveRxCplt >0 ){
        flagSlaveRxCplt = 0;
        printf("Receive command from master, send out value \r\n");
      }
      
    }
  }
}

#if defined(I2C2_DMA_ENABLE) && (I2C2_DMA_ENABLE == 1)
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
  flagI2CRxCplt = 1;
}
#endif /*defined(I2C2_DMA_ENABLE) && (I2C2_DMA_ENABLE == 1)*/

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *I2cHandle){
  flagSlaveRxCplt = 1;
}

void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *I2cHandle){
  flagSlaveRxCplt = 1;
}