#include "vga.h"
#include "stdio.h"
#include "string.h"

uint32_t lastTimeTaskEDID;

uint8_t i2c1ValueBuff128[256];
uint8_t flashBufferVGA128[256];
uint32_t flashBuff132[260];
uint8_t ret;
uint16_t i;
uint8_t flagI2CRxCplt = 0;
uint16_t iBufferCounter = 0;
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
  uint8_t compareBuffer[8] = {0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00};
  uint8_t c[1];
  
  if (iBufferCounter < 8)
  {
    ret = HAL_I2C_Master_Receive(&hi2c2, VGA_I2C_ADDRESS << 1, c, 1, HAL_MAX_DELAY); // TODO: change to ReceiveDMA
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
  else if (iBufferCounter < VGA_BYTE)
  {
    ret = HAL_I2C_Master_Receive(&hi2c2, VGA_I2C_ADDRESS << 1, i2c1ValueBuff128 + iBufferCounter , (VGA_BYTE - 8) , HAL_MAX_DELAY); // TODO: change to ReceiveDMA
    if (ret == HAL_OK)
    {
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
  Flash_Read_Data(START_FLASH_ADDR, (uint32_t *)flashBuff132, VGA_BYTE/*VGA_CRC_BYTE*/);
//    Flash_Read_Data(START_FLASH_ADDR, (uint32_t *)flashBufferVGA128, VGA_BYTE /*VGA_CRC_BYTE*/);

  for (i = 0; i < VGA_BYTE/*VGA_CRC_BYTE*/; i++) // Read 132 bytes and distribute them to buffer 128 & buffer 4
  {
    if (i < VGA_BYTE)
      flashBufferVGA128[i] = (uint8_t)flashBuff132[i];
    else
      crcValue.crcValue8[i - VGA_BYTE] = (uint8_t)flashBuff132[i];
  }
//    check crc
   uint32_t tmpCRC = HAL_CRC_Accumulate(&hcrc, (uint32_t *)flashBufferVGA128, VGA_BYTE);
   if (tmpCRC != crcValue.crcValue32)
   {
     printf("fail check crc at read flash \r\n");
     return 0;
   }
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
  uint32_t buff132[VGA_CRC_BYTE];
  printf("store_info_vga_flash\r\n");
  crcValue.crcValue32 = HAL_CRC_Accumulate(&hcrc, (uint32_t *)bufferVGA, sizeof(bufferVGA));
  for (i = 0; i < VGA_CRC_BYTE; i++)
  {
    if (i < VGA_BYTE)
      buff132[i] = bufferVGA[i];
    else
      buff132[i] = crcValue.crcValue8[i - VGA_BYTE];
  }
   Flash_Write_Data(START_FLASH_ADDR, (uint32_t *)buff132, VGA_CRC_BYTE);
//    Flash_Write_Data(START_FLASH_ADDR, (uint32_t *)bufferVGA, VGA_BYTE);

   printf("Write data to flash \r\n");
   if (!read_flash_checked())
   { // if check flash fail
     printf("flash write check failed 1\n\r");
     Flash_Write_Data(START_FLASH_ADDR, (uint32_t *)buff132, VGA_CRC_BYTE);
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
//  HAL_I2C_IsDeviceReady
  HAL_I2C_IsDeviceReady(&hi2c2, 160, 3, 100);
  read_flash_checked();
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
  if (HAL_GetTick() - lastTimeTaskEDID > 1000)
  {
    lastTimeTaskEDID = HAL_GetTick();
    if (is_VGA_detect()) // if have VGA
    {
      read_flash_checked();
      if (bufer_compare(flashBufferVGA128, i2c1ValueBuff128, VGA_BYTE))
      {                                         // if VGA != localBuff
        store_info_vga_flash(i2c1ValueBuff128); // storage new VGA EDID
      }
    }
    else {
      printf("fail read VGA device");
    }
    
    //    if (flagI2CRxCplt)
    //    {
    //      flagI2CRxCplt = 0;
    //      printf("Rx complete \r\n");
    //    }
    
  }
}
