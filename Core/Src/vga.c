#include "vga.h"
#include "stdio.h"
#include "string.h"

uint32_t lastTimeTaskEDID;
int ii=0;
uint32_t lastTimedetect;
uint16_t iBufferCounter1;
uint8_t ret1=0;
uint8_t ret2=0;
int counter_VGA_1=0;
int counter_VGA_2=0;
uint16_t iBufferCounter2;
uint8_t i2c1ValueBuff128_VGA1[VGA_BYTE];
uint8_t i2c1ValueBuff128_VGA2[VGA_BYTE];
uint8_t i2c1ValueBuff128[VGA_BYTE]=
{
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x06, 0xb3, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x06, 0xb3, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

};
#ifndef DUMMY_ASUS_EDID
uint32_t flashBufferVGA128[VGA_BYTE];
#else
uint8_t flashBufferVGA128[256] =
    {
        0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x06, 0xb3, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x34, 0x1c, 0x01, 0x03, 0x80, 0x50, 0x2d, 0x78, 0x0a, 0xee, 0x91, 0xa3, 0x54, 0x4c, 0x99, 0x26,
        0x0f, 0x50, 0x54, 0xbf, 0xef, 0x00, 0xd1, 0xc0, 0x81, 0x00, 0x81, 0x40, 0x95, 0x00, 0x81, 0xfc,
        0xb3, 0x00, 0x81, 0x80, 0x71, 0x40, 0x01, 0x1d, 0x00, 0x72, 0x51, 0xd0, 0x1e, 0x20, 0x6e, 0x28,
        0x55, 0x00, 0xc4, 0x8e, 0x21, 0x00, 0x00, 0x1e, 0x64, 0x19, 0x00, 0x40, 0x41, 0x00, 0x26, 0x30,
        0x18, 0x87, 0x36, 0x00, 0xc4, 0x8e, 0x21, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x41,
        0x53, 0x55, 0x53, 0x20, 0x53, 0x32, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xfd,
        0x00, 0x19, 0x78, 0x1a, 0x66, 0x0f, 0x00, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0xbd,

        0x02, 0x03, 0x34, 0xf1, 0x54, 0x10, 0x84, 0x21, 0x20, 0x13, 0x05, 0x14, 0x1f, 0x12, 0x03, 0x07,
        0x16, 0x01, 0x02, 0x11, 0x22, 0x27, 0x0f, 0x1e, 0x2f, 0x23, 0x09, 0x1f, 0x07, 0x72, 0x03, 0x0c,
        0x00, 0x10, 0x00, 0xb8, 0x2d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x83, 0x01, 0x00, 0x00, 0x01, 0x1d, 0x00, 0x72, 0x51, 0xd0, 0x1e, 0x20, 0x6e, 0x28, 0x55, 0x00,
        0xc4, 0x8e, 0x21, 0x00, 0x00, 0x1f, 0x8c, 0x0a, 0xd0, 0x8a, 0x20, 0xe0, 0x2d, 0x10, 0x10, 0x3e,
        0x96, 0x00, 0xc4, 0x8e, 0x21, 0x00, 0x00, 0x19, 0xd6, 0x09, 0x80, 0xa0, 0x20, 0xe0, 0x2d, 0x10,
        0x10, 0x60, 0xa2, 0x00, 0x12, 0x8e, 0x21, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfb};
#endif /* DUMMY_ASUS_EDID */
uint32_t flashBuff132[VGA_CRC_BYTE];
uint8_t ret;
uint16_t i;
uint8_t flagI2CRxCplt = 0;
uint16_t iBufferCounter = 0;
uint8_t flagSlaveRxCplt = 0;
uint8_t flagTxCplt = 0;
uint8_t flagRxCplt = 0;

const uint8_t edidHeader[8] = {0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00};
uint8_t c[1];
uint32_t smVgaDetectTimeOut = 0;

// fsm Control state
uint8_t fsmStateVGARead = VGA_READ_START;
uint8_t smVgaDetectState = 0;
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
//void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c)
//{
//  flagTxCplt = 1;
//}
//
//void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
//{
//  if (hi2c->Instance == I2C2)
//  {
//    flagRxCplt = 1;
//  }
//}
void merge_i2c_EDID(uint8_t* buffer_a,uint8_t* buffer_b)
{
	buffer_a+=16;
	buffer_b+=16;
	*buffer_b=*buffer_b & *buffer_a;
}
void copy_buffer(uint8_t *a,uint8_t *b)
{
	int count=128;
	while(count-->0)
		*a++=*b++;
}
void    I2C_switch_VGA_1()
{
	I2C2_pin_switch=1;
	HAL_I2C_DeInit(&hi2c2);
	HAL_I2C_Init(&hi2c2);
}
void    I2C_switch_VGA_2()
{
	I2C2_pin_switch=2;
	HAL_I2C_DeInit(&hi2c2);
	HAL_I2C_Init(&hi2c2);
}
uint8_t is_VGA_detect_1()
{
	I2C_switch_VGA_1();
  if (iBufferCounter1 < 8)
  {
    ret = HAL_I2C_Master_Receive(&hi2c2, VGA_I2C_ADDRESS << 1, c, 1, 10 /*HAL_MAX_DELAY*/); // TODO: change to ReceiveDMA
    if (ret == HAL_OK)
    {
      if (edidHeader[iBufferCounter1] == c[0])
      {
        i2c1ValueBuff128_VGA1[iBufferCounter1] = c[0];
        iBufferCounter1++;
      }
      else
      {
        iBufferCounter1 = 0;
      }
    }
    else
    {
//      HAL_I2C_DeInit(&hi2c2);
//      HAL_I2C_Init(&hi2c2);
    }
  }
  else if (iBufferCounter1 < VGA_BYTE)
  {
    ret = HAL_I2C_Master_Receive(&hi2c2, VGA_I2C_ADDRESS << 1, i2c1ValueBuff128_VGA1 + iBufferCounter1, (VGA_BYTE - 8), 100); // TODO: change to ReceiveDMA
    if (ret == HAL_OK)
    {
      iBufferCounter1 = 0;
      return 1;
    }
    else
    {
//      HAL_I2C_DeInit(&hi2c2);
//      HAL_I2C_Init(&hi2c2);
    }

  }
  else
  {
    iBufferCounter1 = 0;
    return 0;
  }
  return 0;
}
uint8_t is_VGA_detect_2()
{
	I2C_switch_VGA_2();
//	HAL_I2C_DeInit(&hi2c2);
  if (iBufferCounter2 < 8)
  {
    ret = HAL_I2C_Master_Receive(&hi2c2, VGA_I2C_ADDRESS << 1, c, 1, 10 /*HAL_MAX_DELAY*/); // TODO: change to ReceiveDMA
    if (ret == HAL_OK)
    {
      if (edidHeader[iBufferCounter2] == c[0])
      {
        i2c1ValueBuff128_VGA2[iBufferCounter2] = c[0];
        iBufferCounter2++;
      }
      else
      {
        iBufferCounter2 = 0;
      }
    }
    else
    {
//      HAL_I2C_DeInit(&hi2c2);
//      HAL_I2C_Init(&hi2c2);
    }
  }
  else if (iBufferCounter2 < VGA_BYTE)
  {
    ret = HAL_I2C_Master_Receive(&hi2c2, VGA_I2C_ADDRESS << 1, i2c1ValueBuff128_VGA2 + iBufferCounter2, (VGA_BYTE - 8), 100); // TODO: change to ReceiveDMA
    if (ret == HAL_OK)
    {
      iBufferCounter2 = 0;
      return 1;
    }
    else
    {
//      HAL_I2C_DeInit(&hi2c2);
//      HAL_I2C_Init(&hi2c2);
    }

  }
  else
  {
    iBufferCounter2 = 0;
    return 0;
  }
  return 0;
}
uint8_t is_VGA_detect()
{
	if (HAL_GetTick() - lastTimedetect < 750)
	{
//	    lastTimeTaskEDID = HAL_GetTick();
	    ret2=is_VGA_detect_2();
	    I2C_switch_VGA_1();
	}
	else if(HAL_GetTick() - lastTimedetect < 1500)
	{
		ret1=is_VGA_detect_1();
//		I2C_switch_VGA_1();
	}
	else lastTimedetect = HAL_GetTick();
	switch (ret1+ret2)
	{
	case 0:
		return 0;
		break;
	case 1:
		if (ret1==1){
		counter_VGA_1++;
		copy_buffer(i2c1ValueBuff128, i2c1ValueBuff128_VGA1);
		return 1;
		break ;
		}
		else {copy_buffer(i2c1ValueBuff128, i2c1ValueBuff128_VGA2);
		counter_VGA_2++;
		return 1;
		break;
	    }
	case 2:
		copy_buffer(i2c1ValueBuff128, i2c1ValueBuff128_VGA1);
		return 1;
		break ;
	}
	return 0;
}
//uint8_t is_VGA_detect2()
//{
//  switch (smVgaDetectState)
//  {
//  case 0:
//    if (HAL_I2C_GetState(&hi2c2) == HAL_I2C_STATE_READY)
//    {                                                                       // receive 1 byte
//      ret = HAL_I2C_Master_Receive_DMA(&hi2c2, VGA_I2C_ADDRESS << 1, c, 1); // TODO: change to ReceiveDMA
//      if (ret == HAL_OK)
//      {
//        smVgaDetectState = 1;
//        smVgaDetectTimeOut = HAL_GetTick();
//      }
//    }
//    break;
//  case 1: // wait respone
//    if (flagRxCplt > 0)
//    {
//      flagRxCplt = 0;
//      if (edidHeader[iBufferCounter] == c[0])
//      {
//        i2c1ValueBuff128[iBufferCounter] = c[0];
//        iBufferCounter++;
//        if (iBufferCounter >= 8)
//        {
//          smVgaDetectState = 2;
//        }
//        else
//          smVgaDetectState = 0;
//      }
//      else
//      {
//        smVgaDetectState = 0;
//        iBufferCounter = 0;
//        printf("sadsd");
//      }
//    }
//    else if (HAL_GetTick() - smVgaDetectTimeOut > 10000)
//    {
//      smVgaDetectState = 0;
//      iBufferCounter = 0;
//    }
//    break;
//  case 2:
//    ret = HAL_I2C_Master_Receive_DMA(&hi2c2, VGA_I2C_ADDRESS << 1,
//                                     i2c1ValueBuff128 + iBufferCounter, (VGA_BYTE - 8));
//    //	    ret = HAL_I2C_Master_Receive(&hi2c2, VGA_I2C_ADDRESS << 1, i2c1ValueBuff128 + iBufferCounter , (VGA_BYTE - 8) , HAL_MAX_DELAY); // TODO: change to ReceiveDMA
//    if (ret == HAL_OK)
//    {
//      smVgaDetectState = 3;
//      iBufferCounter = 0;
//    }
//    else
//    {
//      smVgaDetectState = 0;
//      iBufferCounter = 0;
//    }
//    break;
//  case 3:
//    if (flagRxCplt > 0)
//    {
//      flagRxCplt = 0;
//      smVgaDetectState = 0;
//      iBufferCounter = 0;
//      return 1;
//    }
//    break;
//  case 4:
//
//    break;
//  }
//  return 0;
//}

//uint8_t is_VGA_detect()
//{
//  //  uint8_t ret;
//  // uint8_t compareBuffer[8] = {0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00};
//  //  uint8_t c[1];
//  if (iBufferCounter < 8)
//  {
//    ret = HAL_I2C_Master_Receive(&hi2c2, VGA_I2C_ADDRESS << 1, c, 1, 10 /*HAL_MAX_DELAY*/); // TODO: change to ReceiveDMA
//    if (ret == HAL_OK)
//    {
//      if (edidHeader[iBufferCounter] == c[0])
//      {
//        i2c1ValueBuff128[iBufferCounter] = c[0];
//        iBufferCounter++;
//      }
//      else
//      {
//        iBufferCounter = 0;
//      }
//    }
//    else
//    {
//      HAL_I2C_DeInit(&hi2c2);
//      HAL_I2C_Init(&hi2c2);
//    }
//  }
//  else if (iBufferCounter < VGA_BYTE)
//  {
//    ret = HAL_I2C_Master_Receive(&hi2c2, VGA_I2C_ADDRESS << 1, i2c1ValueBuff128 + iBufferCounter, (VGA_BYTE - 8), 100); // TODO: change to ReceiveDMA
//    if (ret == HAL_OK)
//    {
//      iBufferCounter = 0;
//      return 1;
//    }
//    else
//    {
//      HAL_I2C_DeInit(&hi2c2);
//      HAL_I2C_Init(&hi2c2);
//    }
//    //    ret = HAL_I2C_Master_Receive(&hi2c2, VGA_I2C_ADDRESS << 1, &i2c1ValueBuff128[iBufferCounter], 1, HAL_MAX_DELAY); // TODO: change to ReceiveDMA
//    //    if (ret == HAL_OK)
//    //    {
//    //      iBufferCounter++;
//    //      if (iBufferCounter >= VGA_BYTE)
//    //      {
//    //        iBufferCounter = 0;
//    //        return 1;
//    //      }
//    //    }
//  }
//  else
//  { // collect 128 byte buffer done
//    iBufferCounter = 0;
//    return 0;
//  }
//  return 0;
//}

/*
//  function:       bufer_compare :
//  parameters:             2 buffer
//  returns:            0 if same, >1 if different
//  description:     check buffer
*/
uint8_t bufer_compare(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t bfLen)
{
  uint8_t *apBuffer1 = pBuffer1;
  uint8_t *apBuffer2 = pBuffer2;
  while (bfLen--)
  {
    if ((*apBuffer1) != (*apBuffer2))
    {
      return bfLen;
    }
    apBuffer1++;
    apBuffer2++;
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
#ifdef USING_CRC
  Flash_Read_Data(START_FLASH_ADDR, (uint32_t *)flashBuff132, VGA_CRC_BYTE);
  for (i = 0; i < VGA_CRC_BYTE; i++) // Read 132 bytes and distribute them to buffer 128 & buffer 4
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

#else
  Flash_Read_Data(START_FLASH_ADDR, (uint32_t *)flashBufferVGA128, VGA_BYTE);
#endif
}

/*
//  function: store_info_vga_flash :
//  parameters: buffer 128 bytes
//  returns: none - should return result
//  description:     store info to flash
*/
uint8_t store_info_vga_flash(uint32_t *bufferVGA)
{
#ifdef USING_CRC
  uint32_t buff132[VGA_CRC_BYTE];
  printf("store_info_vga_flash\r\n");
  crcValue.crcValue32 = HAL_CRC_Accumulate(&hcrc, (uint32_t *)bufferVGA, VGA_BYTE);
  for (uint8_t i = 0; i < VGA_CRC_BYTE; i++)
  {
    if (i < VGA_BYTE)
      buff132[i] = bufferVGA[i];
    else
      buff132[i] = crcValue.crcValue8[i - VGA_BYTE];
  }
  Flash_Write_Data(START_FLASH_ADDR, (uint32_t *)buff132, VGA_CRC_BYTE);
  //    Flash_Write_Data(START_FLASH_ADDR, (uint32_t *)bufferVGA, VGA_BYTE);

  // printf("Write data to flash \r\n");
  // if (!read_flash_checked())
  // { // if check flash fail
  //   printf("flash write check failed 1\n\r");
  //   Flash_Write_Data(START_FLASH_ADDR, (uint32_t *)buff132, VGA_CRC_BYTE);
  //   if (!read_flash_checked())
  //   { // re write flash fail
  //     printf("flash write check failed 2\n\r");
  //     // raise evetn write fail
  //     return 0;
  //   }
  // }
  return 1;
#else
  Flash_Write_Data(START_FLASH_ADDR, (uint32_t *)bufferVGA, VGA_BYTE);
#endif
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
//  I2C2_pin_switch=2;
//  HAL_I2C_DeInit(&hi2c2);
//  HAL_I2C_Init(&hi2c2);

  lastTimedetect=HAL_GetTick();
}

/*
//  function: vga_tasks :
//  parameters: none
//  returns: none
//  description:
*/

void vga_read_tasks()
{
  switch (fsmStateVGARead)
  {
  case VGA_READ_START:
    fsmStateVGARead = VGA_READ_IDLE;
    lastTimeTaskEDID = HAL_GetTick();
    break;
  case VGA_READ_IDLE:
    if (HAL_GetTick() - lastTimeTaskEDID > 1000)
    {
      fsmStateVGARead = VGA_READ_DETECT_DEVICE;
    }
    break;
  case VGA_READ_DETECT_DEVICE:
    if (HAL_I2C_IsDeviceReady(&hi2c2, VGA_I2C_ADDRESS << 1, 1, 1) == HAL_OK)
      break;
  case VGA_READ_REQUEST_DEVICE_FULL:

    break;
  case VGA_READ_REQUEST_DEVICE_SINGLE_HEADER:

    break;
  case VGA_READ_REQUEST_DEVICE_SINGLE_REMAIN:

    break;
  case VGA_READ_READ_FLASH:

    break;
  case VGA_READ_COMPARE_BUFFER:

    break;
  case VGA_READ_WRITE_FLASH:

    break;
  case VGA_READ_SNAP_TIME:

    break;
  default:
    break;
  }
}

void vga_tasks()
{
  uint8_t result;

  // vga_read_tasks();

  // if (HAL_GetTick() - lastTimeTaskEDID > 10)
  // {
  //   lastTimeTaskEDID = HAL_GetTick();
  //   if (is_VGA_detect()) // if have VGA
  //   {
  //     read_flash_checked();
  //     result = bufer_compare(flashBufferVGA128, i2c1ValueBuff128, 20 /*VGA_BYTE*/);
  //     if (result > 0 )
  //       //       if(1)
  //     {                                         // if VGA != localBuff
  //       store_info_vga_flash(i2c1ValueBuff128); // storage new VGA EDID
  //       HAL_Delay(10000);
  //     }
  //   }
  //   else
  //   {
  //     //      printf("fail read VGA device");
  //   }
  // }

  if (HAL_GetTick() - lastTimeTaskEDID > 10)
  {
    lastTimeTaskEDID = HAL_GetTick();
    //    if (HAL_I2C_IsDeviceReady(&hi2c2, VGA_I2C_ADDRESS << 1, 1, 1) == HAL_OK)
    //    {
//    store_info_vga_flash((uint32_t *)i2c1ValueBuff128);
    if (is_VGA_detect()) // if have VGA
    {
      read_flash_checked();
      copy_buffer(atest_edid,i2c1ValueBuff128);
      if (bufer_compare((uint8_t *)flashBufferVGA128, i2c1ValueBuff128, 20 /*VGA_BYTE*/))
      {                                                     // if VGA != localBuff
        store_info_vga_flash((uint32_t *)i2c1ValueBuff128); // storage new VGA EDID
//        copy_buffer(atest_edid,i2c1ValueBuff128);
      }
      //    }
    }
  }

//  if (HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_READY)
//  {
//    // HAL_I2C_Slave_Receive_DMA(&hi2c1, &dummyReceive, 1);
//    HAL_I2C_Slave_Transmit_DMA(&hi2c1, (uint8_t *)flashBufferVGA128, VGA_BYTE);
//  }
}

// void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
// {
// 	if(hi2c->Instance==I2C1)
// 	{
// 		if(dummyReceive == 0)
// 		{
// 			HAL_I2C_Slave_Transmit_DMA(&hi2c1, flashBufferVGA128, VGA_BYTE);
// 		}else
// 		{
// 			HAL_I2C_Slave_Transmit_DMA(&hi2c1, (uint8_t *)&flashBufferVGA128[dummyReceive], 1);
// 		}
// 	}else if(hi2c->Instance==I2C2)
// 	{

// 	}
// }

// void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c)
//{
//	if(hi2c->Instance==I2C1)
//	{
//
//	}else if(hi2c->Instance==I2C2)
//	{
//
//	}
// }
