#include "vga.h"
#include "stdio.h"
#include "string.h"

#define START_FLASH_ADDR 0x08010000
#define VGA_I2C_ADDRESS 0x50
#define VGA_EDID_BYTE 128

extern CRC_HandleTypeDef hcrc;
extern I2C_HandleTypeDef hi2c2;

uint32_t lastTimeTaskEDID;

uint8_t i2c1Value[128];
uint8_t flashBuffer[132];
uint8_t flashBufferVGA[128];
union crcValue
{
    uint32_t crcValue32;
    uint8_t crcValue8[4];
} crcValue;
//__IO uint32_t uwCRCValue = 0;

uint32_t lastTimeReadI2C1;
uint8_t ret;
uint8_t i;

uint8_t isVGADetect()
{
    uint8_t compareBuffer[5] = {0x00, 0xff, 0xff, 0xff, 0xff};
    HAL_I2C_Master_Receive(&hi2c2, VGA_I2C_ADDRESS << 1, i2c1Value, VGA_EDID_BYTE, 100); // TODO: change to ReceiveDMA
     for (i = 0; i < VGA_EDID_BYTE; i++)    //print
        {
            printf("0x%02x ", i2c1Value[i]);
            if (i % 8 == 7)
                printf("\r\n");
        }
    ret = strncmp(i2c1Value, compareBuffer, 5);
    if (ret == 0) // thid is VGA device
        return 1;
    return 0;
}

uint8_t isNewVGADevice(uint8_t *localBuff, uint8_t *deviceBuff)
{
    for (uint8_t i = 0; i < sizeof(deviceBuff); i++)
    {
        if (localBuff[i] != deviceBuff[i]) // this is new device
            return 1;
    }

    return 0;
}

void storeVGAInfo(uint8_t *bufferVGA)
{
    uint8_t buffer[132];
    crcValue.crcValue32 = HAL_CRC_Accumulate(&hcrc, (uint32_t *)bufferVGA, sizeof(bufferVGA));
    for (i = 0; i < sizeof(buffer); i++)
    {
        if (i < VGA_EDID_BYTE)
            buffer[i] = bufferVGA[i];
        else
            buffer[i] = crcValue.crcValue8[i - 128];
    }
    Flash_Write_Data(START_FLASH_ADDR, (uint32_t *)buffer, sizeof(buffer));
}

void vga_init()
{
    ret = HAL_I2C_IsDeviceReady(&hi2c2, (uint16_t)(80 << 1), 3, 5);
    if (ret == HAL_OK)
    {
        isVGADetect();

        // Read data from Flash
        Flash_Read_Data(START_FLASH_ADDR, (uint32_t *)flashBuffer, sizeof(flashBuffer));
        for (i = 0; i < sizeof(flashBuffer); i++) // Read 132 bytes and distribute them to buffer 128 & buffer 4
        {
            if (i < 128)
                flashBufferVGA[i] = flashBuffer[i];
            else
                crcValue.crcValue8[i - 128] = flashBuffer[i];
        }

        //  //  check crc
        uint32_t tmpCRC = HAL_CRC_Accumulate(&hcrc, (uint32_t *)flashBufferVGA, sizeof(flashBufferVGA));
        if (tmpCRC != crcValue.crcValue32)
            printf("fail check crc at read flash \r\n");
        else
            printf("correct crcValue");

        // compare i2c buffer and flash buffer
        if (isNewVGADevice(i2c1Value, flashBufferVGA))
        {
            printf("Diff found \r\n");
            // then write flash buffer new vga info
            storeVGAInfo(flashBufferVGA);
            // write flash buffer
            // memcopy buffer
        }
        else
        {
            printf("Diff not found \r\n");
        }
    }
}

void vga_tasks()
{
    if (HAL_GetTick() - lastTimeTaskEDID > 1000)
    {
        lastTimeTaskEDID = HAL_GetTick();
        // if vga device
        if(isVGADetect()) {
            //  read flash and compare if device is new
        //  if device info ! flash storage
        //  write new flash value
        }
        
    }
}