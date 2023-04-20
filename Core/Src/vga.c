#include "vga.h"
#include "stdio.h"
#include "string.h"

uint32_t lastTimeTaskEDID;

uint8_t i2c1ValueBuff128[256];
#ifndef DUMMY_ASUS_EDID
uint8_t flashBufferVGA128[256];
#else
uint8_t flashBufferVGA128[256] = 
{
    0x00 ,0xff ,0xff ,0xff ,0xff ,0xff ,0xff ,0x00 ,0x06 ,0xb3 ,0x03 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 , 
    0x34 ,0x1c ,0x01 ,0x03 ,0x80 ,0x50 ,0x2d ,0x78 ,0x0a ,0xee ,0x91 ,0xa3 ,0x54 ,0x4c ,0x99 ,0x26 , 
    0x0f ,0x50 ,0x54 ,0xbf ,0xef ,0x00 ,0xd1 ,0xc0 ,0x81 ,0x00 ,0x81 ,0x40 ,0x95 ,0x00 ,0x81 ,0xfc ,
    0xb3 ,0x00 ,0x81 ,0x80 ,0x71 ,0x40 ,0x01 ,0x1d ,0x00 ,0x72 ,0x51 ,0xd0 ,0x1e ,0x20 ,0x6e ,0x28 ,
    0x55 ,0x00 ,0xc4 ,0x8e ,0x21 ,0x00 ,0x00 ,0x1e ,0x64 ,0x19 ,0x00 ,0x40 ,0x41 ,0x00 ,0x26 ,0x30 ,
    0x18 ,0x87 ,0x36 ,0x00 ,0xc4 ,0x8e ,0x21 ,0x00 ,0x00 ,0x18 ,0x00 ,0x00 ,0x00 ,0xfc ,0x00 ,0x41 ,
    0x53 ,0x55 ,0x53 ,0x20 ,0x53 ,0x32 ,0x0a ,0x20 ,0x20 ,0x20 ,0x20 ,0x20 ,0x00 ,0x00 ,0x00 ,0xfd ,
    0x00 ,0x19 ,0x78 ,0x1a ,0x66 ,0x0f ,0x00 ,0x0a ,0x20 ,0x20 ,0x20 ,0x20 ,0x20 ,0x20 ,0x01 ,0xbd ,

    0x02 ,0x03 ,0x34 ,0xf1 ,0x54 ,0x10 ,0x84 ,0x21 ,0x20 ,0x13 ,0x05 ,0x14 ,0x1f ,0x12 ,0x03 ,0x07 ,
    0x16 ,0x01 ,0x02 ,0x11 ,0x22 ,0x27 ,0x0f ,0x1e ,0x2f ,0x23 ,0x09 ,0x1f ,0x07 ,0x72 ,0x03 ,0x0c ,
    0x00 ,0x10 ,0x00 ,0xb8 ,0x2d ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x83 ,0x01 ,0x00 ,0x00 ,0x01 ,0x1d ,0x00 ,0x72 ,0x51 ,0xd0 ,0x1e ,0x20 ,0x6e ,0x28 ,0x55 ,0x00 ,
    0xc4 ,0x8e ,0x21 ,0x00 ,0x00 ,0x1f ,0x8c ,0x0a ,0xd0 ,0x8a ,0x20 ,0xe0 ,0x2d ,0x10 ,0x10 ,0x3e ,
    0x96 ,0x00 ,0xc4 ,0x8e ,0x21 ,0x00 ,0x00 ,0x19 ,0xd6 ,0x09 ,0x80 ,0xa0 ,0x20 ,0xe0 ,0x2d ,0x10 ,
    0x10 ,0x60 ,0xa2 ,0x00 ,0x12 ,0x8e ,0x21 ,0x00 ,0x00 ,0x18 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,
    0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0xfb
};
#endif /* DUMMY_ASUS_EDID */
uint32_t flashBuff132[260];
uint8_t ret;
uint16_t i;
uint8_t flagI2CRxCplt = 0;
uint16_t iBufferCounter = 0;
uint8_t flagSlaveRxCplt = 0;
uint8_t flagTxCplt = 0;
uint8_t flagRxCplt = 0;
uint8_t smVgaDetectStatus = 0;
uint8_t edidHeader[8] = { 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00 };
uint8_t c[1];
uint32_t smVgaDetectTimeOut = 0;

union crcValue
{
  uint32_t crcValue32;
  uint8_t crcValue8[4];
} crcValue;

uint8_t dummyReceive;
//////////////////////////////////////////////////////////////// LOCAL FUNCTIONS //////////////////////////////////
/*
//  Description: READ I2C FROM vga device, compare first 5  bytes = 0x00ffffffff
*/
void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c) {
	flagTxCplt = 1;
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c) {
	flagRxCplt = 1;
}
uint8_t is_VGA_detect2() {
	switch (smVgaDetectStatus) {
	case 0: //receive 1 byte
		ret = HAL_I2C_Master_Receive_DMA(&hi2c2, VGA_I2C_ADDRESS << 1, c, 1); // TODO: change to ReceiveDMA
		if (ret == HAL_OK) {
			smVgaDetectStatus = 1;
			smVgaDetectTimeOut = HAL_GetTick();
		}
		break;
	case 1: //wait respone
		if (flagRxCplt > 0) {
                  flagRxCplt = 0;
			if (edidHeader[iBufferCounter] == c[0]) {
				i2c1ValueBuff128[iBufferCounter] = c[0];
				iBufferCounter++;
				if (iBufferCounter >= 8) {
					smVgaDetectStatus = 2;
				} else
					smVgaDetectStatus = 0;
			} else {
				smVgaDetectStatus = 0;
				iBufferCounter = 0;
			}
		} else if (HAL_GetTick() - smVgaDetectTimeOut > 1000) {
			smVgaDetectStatus = 0;
			iBufferCounter = 0;
		}
		break;
	case 2:
		ret = HAL_I2C_Master_Receive_DMA(&hi2c2, VGA_I2C_ADDRESS << 1,
				i2c1ValueBuff128 + iBufferCounter, (VGA_BYTE - 8));
		//	    ret = HAL_I2C_Master_Receive(&hi2c2, VGA_I2C_ADDRESS << 1, i2c1ValueBuff128 + iBufferCounter , (VGA_BYTE - 8) , HAL_MAX_DELAY); // TODO: change to ReceiveDMA
		if (ret == HAL_OK) {
			smVgaDetectStatus = 3;
			iBufferCounter = 0;
		} else {
			smVgaDetectStatus = 0;
			iBufferCounter = 0;
		}
		break;
	case 3:
		if (flagRxCplt > 0) {
                  flagRxCplt = 0;
			smVgaDetectStatus = 0;
			iBufferCounter = 0;
			return 1;
		}
		break;
	}
	return 0;
}

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
      if (bufer_compare(flashBufferVGA128, i2c1ValueBuff128, 20 /*VGA_BYTE*/))
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

  if(HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_READY){
    // HAL_I2C_Slave_Receive_DMA(&hi2c1, &dummyReceive, 1);
    HAL_I2C_Slave_Transmit_DMA(&hi2c1, flashBufferVGA128, VGA_BYTE);
  }
}
