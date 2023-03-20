#ifndef _VGA_H_
#define _VGA_H_

#include "main.h"

#define START_FLASH_ADDR 0x08010000
#define VGA_I2C_ADDRESS 0x50
#define VGA_EDID_BYTE 128

extern CRC_HandleTypeDef hcrc;
extern I2C_HandleTypeDef hi2c2;
extern I2C_HandleTypeDef hi2c1;
void vga_init();
void vga_tasks();

#endif