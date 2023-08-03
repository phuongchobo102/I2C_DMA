#ifndef _VGA_H_
#define _VGA_H_

#include "main.h"
//#define USING_CRC           1
#define START_FLASH_ADDR    0x0801f800 //0x0801fc00 //0x00012000  //0x0801f000
#define VGA_I2C_ADDRESS     0x50
#define VGA_BYTE            128 //256
#define CRC_BYTE            4
#define VGA_CRC_BYTE        (VGA_BYTE + CRC_BYTE) //260 //132 //260

enum VGA_READ_STATE {
    VGA_READ_START = 0,
  VGA_READ_IDLE ,
  VGA_READ_DETECT_DEVICE,
  VGA_READ_REQUEST_DEVICE_FULL,
  VGA_READ_REQUEST_DEVICE_SINGLE_HEADER,
  VGA_READ_REQUEST_DEVICE_SINGLE_REMAIN,
  VGA_READ_READ_FLASH,
  VGA_READ_COMPARE_BUFFER,
  VGA_READ_WRITE_FLASH,
  VGA_READ_SNAP_TIME
};
// da type of vga detected//
typedef enum {
	NO_VGA,
	VGA_1_DETECTED,
	VGA_2_DETECTED,
	VGA_12_DETECTED
}vga_detect_status_t;

extern CRC_HandleTypeDef hcrc;
extern I2C_HandleTypeDef hi2c2;
extern I2C_HandleTypeDef hi2c1;
extern uint8_t I2C2_pin_switch;
extern uint8_t atest_edid[128];
//extern uint8_t flashBufferVGA128[];

void vga_init();
void vga_tasks();

#endif
