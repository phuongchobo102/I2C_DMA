#include "main.h"
#include "usbd_def.h"
#include "usb_sw_selector.h"
#include "usbd_customhid.h"

extern uint8_t flag_rx;
extern uint8_t report_buffer[64];
extern uint8_t tx_buffer[64];
extern USBD_HandleTypeDef hUsbDeviceFS;

usb_msg_format_t *usb_msg;

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


void process_usb_msg(void)
{  
  uint8_t i;
  usb_msg = (usb_msg_format_t *)tx_buffer;
  usb_msg_format_t response_host;
  
  while (1)
  {
      if (flag_rx == 1)
      {
        for(i=0; i<64;i++)
        {
          printf("0x%02x, ", tx_buffer[i]);
          if(i%8==7)
            printf("\r\n");
        }
        if(usb_msg->header == USB_HEADER)
        {
          switch(usb_msg->opcode)
          {
            case USB_STATUS:
              response_host.header = USB_HEADER;
              response_host.opcode = USB_STATUS;
              response_host.opcode_status = 0x01;
              printf("Response USB_STATUS \r\n");
              USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&response_host, 0x40);
              // your function
              break;
              
            case USB_ACK:
              response_host.header = USB_HEADER;
              response_host.opcode = USB_ACK;
              response_host.opcode_status = 0x01;
              printf("Response USB_ACK \r\n");
              USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&response_host, 0x40);
              // your function
              break;
              
            case USB_WRT_EDID:
              response_host.header = USB_HEADER;
              response_host.opcode = USB_WRT_EDID;
              response_host.opcode_status = 0x01;
              printf("Response USB_WRT_EDID \r\n");
              USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&response_host, 0x40);
              // your function
              break;
              
            case USB_REPLY_EDID:
              response_host.header = USB_HEADER;
              response_host.opcode = USB_REPLY_EDID;
              response_host.opcode_status = 0x01;
              printf("Response USB_REPLY_EDID \r\n");
              USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&response_host, 0x40);
              // your function
              break;
              
           case USB_GET_MOUSE_PORT_STATUS:
              response_host.header = USB_HEADER;
              response_host.opcode = USB_GET_MOUSE_PORT_STATUS;
              response_host.opcode_status = 0x01;
              printf("Response USB_GET_MOUSE_PORT_STATUS \r\n");
              USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&response_host, 0x40);
              // your function
              break;
              
            case USB_GET_VGA_PORT_STATUS:
              response_host.header = USB_HEADER;
              response_host.opcode = USB_GET_VGA_PORT_STATUS;
              response_host.opcode_status = 0x01;
              printf("Response USB_GET_VGA_PORT_STATUS \r\n");
              USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&response_host, 0x40);
              // your function
              break;
              
            default:
              printf("OPCODE is wrong! 0x%02x \r\n", usb_msg->opcode);
              break;
          }
        }
        else
        {
          printf("Header is wrong 0x%02x \r\n", usb_msg->header);
          printf("But tx_buffer[0] is 0x%02x \r\n", tx_buffer[0]);
        }
        flag_rx = 0;
      }       
  } // end while
}

