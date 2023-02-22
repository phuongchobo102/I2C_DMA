#include "usb_sw_selector.h"



void usb_sw_enable(uint8_t isTrue){
	//if(isTrue) 	HAL_GPIO_WritePin(USB_SW_ENABLE)
	//else
}

void usb_sw_select(uint8_t portIndex) {
    switch (portIndex) {
	case 0:
		HAL_GPIO_WritePin(USB_SW_SEL0_GPIO_Port, USB_SW_SEL0_Pin, 0);
		HAL_GPIO_WritePin(USB_SW_SEL1_GPIO_Port, USB_SW_SEL1_Pin, 0);
		break;
	case 1:
		HAL_GPIO_WritePin(USB_SW_SEL0_GPIO_Port, USB_SW_SEL0_Pin, 1);
		HAL_GPIO_WritePin(USB_SW_SEL1_GPIO_Port, USB_SW_SEL1_Pin, 0);
		break;
	case 2:
		HAL_GPIO_WritePin(USB_SW_SEL0_GPIO_Port, USB_SW_SEL0_Pin, 0);
		HAL_GPIO_WritePin(USB_SW_SEL1_GPIO_Port, USB_SW_SEL1_Pin, 1);
		break;
	case 3:
		HAL_GPIO_WritePin(USB_SW_SEL0_GPIO_Port, USB_SW_SEL0_Pin, 1);
		HAL_GPIO_WritePin(USB_SW_SEL1_GPIO_Port, USB_SW_SEL1_Pin, 1);
		break;
	default:
		break;
	}
}

