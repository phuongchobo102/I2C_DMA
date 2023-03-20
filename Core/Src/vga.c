#include "vga.h"
#include "stdio.h"
#include "string.h"

uint32_t lastTimeTaskEDID;

uint8_t i2c1ValueBuff128[128];
uint8_t flashBufferVGA128[128];
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
    ret = HAL_I2C_Master_Receive(&hi2c2, VGA_I2C_ADDRESS << 1, i2c1ValueBuff128, VGA_EDID_BYTE, 100); // TODO: change to ReceiveDMA
    if (ret == HAL_OK)
    {
        for (uint8_t i = 0; i < VGA_EDID_BYTE; i++) // print
        {
            printf("0x%02x ", i2c1ValueBuff128[i]);
            if (i % 8 == 7)
                printf("\r\n");
        }
        ret = strncmp(i2c1ValueBuff128, compareBuffer, 5);
        if (ret == 0) // thid is VGA device
            return 1;
    }
    return 0;
}

/*
//  function:       is_VGA_diff_flash :
//  parameters:             2 buffer
//  returns:            0 if same, 1 if different
//  description:     check buffer
*/
uint8_t is_VGA_diff_flash(uint8_t *localBuff, uint8_t *deviceBuff)
{
    for (uint8_t i = 0; i < sizeof(deviceBuff); i++)
    {
        if (localBuff[i] != deviceBuff[i]) // this is new device
            return 1;
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
    uint8_t flashBuff132[132];
    Flash_Read_Data(START_FLASH_ADDR, (uint32_t *)flashBuff132, sizeof(flashBuff132));
    for (uint8_t i = 0; i < sizeof(flashBuff132); i++) // Read 132 bytes and distribute them to buffer 128 & buffer 4
    {
        if (i < 128)
            flashBufferVGA128[i] = flashBuff132[i];
        else
            crcValue.crcValue8[i - 128] = flashBuff132[i];
    }
    //  check crc
    uint32_t tmpCRC = HAL_CRC_Accumulate(&hcrc, (uint32_t *)flashBufferVGA128, sizeof(flashBufferVGA128));
    if (tmpCRC != crcValue.crcValue32)
    {
        printf("fail check crc at read flash \r\n");
        return 0;
    }
    printf("correct crcValue");
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
    uint8_t buff132[132];
    uint8_t ret;
    crcValue.crcValue32 = HAL_CRC_Accumulate(&hcrc, (uint32_t *)bufferVGA, sizeof(bufferVGA));
    for (uint8_t i = 0; i < sizeof(buff132); i++)
    {
        if (i < VGA_EDID_BYTE)
            buff132[i] = bufferVGA[i];
        else
            buff132[i] = crcValue.crcValue8[i - 128];
    }
    Flash_Write_Data(START_FLASH_ADDR, (uint32_t *)buff132, sizeof(buff132));
    if (!read_flash_checked())
    { // if check flash fail
        Flash_Write_Data(START_FLASH_ADDR, (uint32_t *)buff132, sizeof(buff132));
        if (!read_flash_checked())
        { // re write flash fail
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
    uint8_t ret = HAL_I2C_IsDeviceReady(&hi2c2, (uint16_t)(80 << 1), 3, 5);
    if (ret == HAL_OK)
    {
        is_VGA_detect();
        read_flash_checked();
        if (is_VGA_diff_flash(flashBufferVGA128, i2c1ValueBuff128))
        {
            printf("Diff found \r\n");
            store_info_vga_flash(i2c1ValueBuff128);
        }
        else
        {
            printf("Diff not found \r\n");
        }
    }
}

/*
//  function: vga_tasks :
//  parameters: none
//  returns: none
//  description:
*/
void vga_tasks()
{
    if (HAL_GetTick() - lastTimeTaskEDID > 1049)
    {
        lastTimeTaskEDID = HAL_GetTick();
        // if vga device at output
        if (is_VGA_detect()) // if have VGA
        {
            read_flash_checked(); // read local flash
            if (is_VGA_diff_flash(flashBufferVGA128, i2c1ValueBuff128))
            {                                           // if VGA != localBuff
                store_info_vga_flash(i2c1ValueBuff128); // storage new VGA EDID
            }
        }

        // if vga device request from vga input
        // check command is correct
        // return buffer i2c1ValueBuff128

        if (HAL_I2C_Slave_Receive_DMA(&hi2c1, (uint8_t *)i2c1ValueBuff128, 128) != HAL_OK)
        {
            Error_Handler();
        }

        if (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
            return;
        if (HAL_I2C_Slave_Transmit_DMA(&hi2c1, (uint8_t *)i2c1ValueBuff128, 128) != HAL_OK)
        {
            /* Transfer error in transmission process */
            Error_Handler();
        }
    }
}