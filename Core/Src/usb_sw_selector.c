#include "main.h"
#include "usbd_def.h"
#include "usb_sw_selector.h"
#include "usbd_customhid.h"
#include "crc16_ibm.h"

extern uint8_t flag_rx;
extern uint8_t report_buffer[64];
extern uint8_t tx_buffer[64];
extern USBD_HandleTypeDef hUsbDeviceFS;
extern uint32_t UID[4];
extern uint8_t key_aes[16];

/////For elinkswitch
static elinkswitch_usb_trigger_t elinkswitch_usb_trigger;
static uint8_t elinkswitch_usb_in_buff[128];
static uint8_t elinkswitch_usb_out_buff[128];
static uint16_t elinkswitch_usb_in_length;
static uint16_t elinkswitch_usb_out_length;
////\

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
                
void authenKVM_init(void)
 {
    if(elinkswitch_get_usb_triggers(&elinkswitch_usb_trigger))
          printf("Successful \r\n");
                else
                printf("Failed \r\n");
                
 }


void authenKVM(void)
{  
    uint8_t i, tmp_1[16], tmp_2[16], *p_UID;
    uint32_t *tmp32, swap[4];
    usb_msg = (usb_msg_format_t *)tx_buffer;
    usb_msg_format_t response_host;
    static uint8_t switch_status = USB_NO_READY;  
    p_UID = (uint8_t *)UID;
    
    if(hUsbDeviceFS.dev_state == USBD_STATE_SUSPENDED)
    {
      switch_status = USB_NO_READY;
      //Notify
      elinkswitch_usb_trigger.back_to_inited();
    }
        
    if (flag_rx == 1)
    {
      if(usb_msg->header == USB_HEADER)
      {
        switch(usb_msg->opcode)
        {
          case USB_ACK:
            response_host.header = USB_HEADER;
            response_host.opcode = USB_ACK;
            response_host.opcode_status = 0x01;
            response_host.len = 5;
            printf("Response USB_ACK \r\n");
            USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&response_host, 0x40); 
            
            break;            
          case USB_AUTHEN_ING:
            response_host.header = USB_HEADER;
            response_host.opcode = USB_AUTHEN_ING;
            response_host.opcode_status = 0x01;
            printf("Receive USB_AUTHEN_ING \r\n");
            
            break;            
          case USB_AUTHEN_FINISH:
            response_host.header = USB_HEADER;
            response_host.opcode = USB_AUTHEN_FINISH;
            response_host.opcode_status = 0x01;
            printf("Receive USB_AUTHEN_FINISH \r\n");
            
            break;             
          default:
            printf("OPCODE is wrong! 0x%02x \r\n", usb_msg->opcode);
            break;            
        }
        // check state machine
        switch(switch_status)
        {
          case USB_NO_READY:
            if(usb_msg->opcode == USB_ACK)
            {
              switch_status = USB_INIT;
              printf("Go to USB_INIT \r\n");
            }
            else
              printf("USB_NO_READY \r\n");
            
            break;
          case USB_INIT:
            if(usb_msg->opcode == USB_AUTHEN_ING)
            {
              AES_ECB_decrypt(usb_msg->data, (const uint8_t*)key_aes, tmp_1, 16);
              //swap tmp_1
              for(i=0; i<16; i++)
                printf("0x%02x, ", tmp_1[i]);
              tmp32 = (uint32_t *)tmp_1;
              swap[0] = tmp32[2];
              swap[1] = tmp32[3];
              swap[2] = tmp32[0];
              swap[3] = tmp32[1];
              for(i=0; i<4; i++)
                printf("0x%08x, ", swap[i]);
              AES_ECB_encrypt((const uint8_t*)swap, (const uint8_t*)key_aes, response_host.data, 16);
              AES_ECB_encrypt((const uint8_t*)UID, (const uint8_t*)key_aes, &response_host.data[16], 16);
              response_host.len = 5 + 32;
              USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&response_host, 0x40);
              printf("Go to USB_AUTHEN_ING \r\n");
              switch_status = USB_AUTHEN_ING;              
            }
            else
              printf("USB_INIT \r\n");
            
            break;            
          case USB_AUTHEN_ING:
            if(usb_msg->opcode == USB_AUTHEN_FINISH)
            {
                AES_ECB_decrypt(usb_msg->data, (const uint8_t*)key_aes, tmp_1, 16);
                
                USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&response_host, 0x40);
                switch_status = USB_AUTHEN_FINISH;
                //Notify
 //               elinkswitch_usb_trigger.authorized();
                // compare UID                
//                if(strncmp((const char *)p_UID, (const char *)tmp_1, 4)==0)
//                {
//                  switch_status = USB_AUTHEN_FINISH;
//                  response_host.len = 5;
//                  USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&response_host, 0x40);
//                  //Notify
//                  elinkswitch_usb_trigger.authorized();
//                }
//                else
//                   printf("Wrong UID \r\n");
            }
            else
              printf("USB_AUTHEN_ING \r\n");
            
            break;              
          case USB_AUTHEN_FINISH:
            printf("Receive USB_AUTHEN_FINISH \r\n");
            // proccess request
            process_usb_msg(usb_msg);
            break;            
          default:
            
            break;             
        }
        
      } // end if(usb_msg->header == USB_HEADER)
      
      flag_rx = 0;
    }    // end if(flag_rx = 1)
}

void process_usb_msg(usb_msg_format_t *usb_msg)
{  
  uint8_t i;
  usb_msg_format_t response_host;  
  
  printf("process_usb_msg :0x%02x \r\n", usb_msg->opcode);
  
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
        break;
        
      case USB_ACK:
        response_host.header = USB_HEADER;
        response_host.opcode = USB_ACK;
        response_host.opcode_status = 0x01;
        printf("Response USB_ACK \r\n");
        USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&response_host, 0x40);
        break;
        
      case USB_WRT_EDID:
        response_host.header = USB_HEADER;
        response_host.opcode = USB_WRT_EDID;
        response_host.opcode_status = 0x01;
        printf("Response USB_WRT_EDID \r\n");
        USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&response_host, 0x40);
        break;
        
      case USB_REPLY_EDID:
        response_host.header = USB_HEADER;
        response_host.opcode = USB_REPLY_EDID;
        response_host.opcode_status = 0x01;
        printf("Response USB_REPLY_EDID \r\n");
        USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&response_host, 0x40);
        break;
        
     case USB_GET_MOUSE_PORT_STATUS:
        response_host.header = USB_HEADER;
        response_host.opcode = USB_GET_MOUSE_PORT_STATUS;
        response_host.opcode_status = 0x01;
        //Notify and get result
        elinkswitch_usb_trigger.receive_usb_command(ELINKSWITCH_RECEIVED_USB_COMMAND_GET_USB_PORTS_STATUS,NULL, 0,elinkswitch_usb_out_buff, &elinkswitch_usb_out_length);
        //ToDo: Retrieve data from elinkswitch_usb_out_buff and send to USB host
        response_host.data[0] = elinkswitch_usb_out_buff[0];
        response_host.data[1] = elinkswitch_usb_out_buff[1];
        response_host.data[2] = elinkswitch_usb_out_buff[2];
        response_host.data[3] = elinkswitch_usb_out_buff[3];
        printf("Response USB_GET_MOUSE_PORT_STATUS \r\n");
        USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&response_host, 0x40);
        break;
        
      case USB_GET_VGA_PORT_STATUS:
        response_host.header = USB_HEADER;
        response_host.opcode = USB_GET_VGA_PORT_STATUS;
        response_host.opcode_status = 0x01;
        //Notify and get result
        elinkswitch_usb_trigger.receive_usb_command(ELINKSWITCH_RECEIVED_USB_COMMAND_GET_VGA_PORTS_STATUS,NULL, 0,elinkswitch_usb_out_buff, &elinkswitch_usb_out_length);
        //ToDo: Retrieve data from elinkswitch_usb_out_buff and send to USB host
        response_host.data[0] = elinkswitch_usb_out_buff[0];
        response_host.data[1] = elinkswitch_usb_out_buff[1];
        response_host.data[2] = elinkswitch_usb_out_buff[2];
        response_host.data[3] = elinkswitch_usb_out_buff[3];
        printf("Response USB_GET_VGA_PORT_STATUS \r\n");
        USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&response_host, 0x40);
        break;
        
      case USB_SET_USB_PORT:
        response_host.header = USB_HEADER;
        response_host.opcode = USB_SET_USB_PORT;
        response_host.opcode_status = 0x01;
        //Notify and get result
        elinkswitch_usb_in_buff[0] = usb_msg->data[0];
        elinkswitch_usb_trigger.receive_usb_command(ELINKSWITCH_RECEIVED_USB_COMMAND_SET_USB_PORTS,elinkswitch_usb_in_buff, 1,NULL, 0);
        printf("Response USB_SET_USB_PORT \r\n");
        USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&response_host, 0x40);
        break;
        
      case USB_SET_VGA_PORT:
        response_host.header = USB_HEADER;
        response_host.opcode = USB_SET_VGA_PORT;
        response_host.opcode_status = 0x01;
        //Notify and get result
        elinkswitch_usb_in_buff[0] = usb_msg->data[0];
        elinkswitch_usb_trigger.receive_usb_command(ELINKSWITCH_RECEIVED_USB_COMMAND_SET_VGA_PORTS,elinkswitch_usb_in_buff, 1,NULL, 0);
        printf("Response USB_SET_VGA_PORT \r\n");
        USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&response_host, 0x40);
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
       
}


//void process_usb_msg(void)
//{  
//  uint8_t i;
//  usb_msg = (usb_msg_format_t *)tx_buffer;
//  usb_msg_format_t response_host;
//  
//  while (1)
//  {
//      if (flag_rx == 1)
//      {
//        for(i=0; i<64;i++)
//        {
//          printf("0x%02x, ", tx_buffer[i]);
//          if(i%8==7)
//            printf("\r\n");
//        }
//        if(usb_msg->header == USB_HEADER)
//        {
//          switch(usb_msg->opcode)
//          {
//            case USB_STATUS:
//              response_host.header = USB_HEADER;
//              response_host.opcode = USB_STATUS;
//              response_host.opcode_status = 0x01;
//              printf("Response USB_STATUS \r\n");
//              USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&response_host, 0x40);
//              // your function
//              break;
//              
//            case USB_ACK:
//              response_host.header = USB_HEADER;
//              response_host.opcode = USB_ACK;
//              response_host.opcode_status = 0x01;
//              printf("Response USB_ACK \r\n");
//              USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&response_host, 0x40);
//              // your function
//              break;
//              
//            case USB_WRT_EDID:
//              response_host.header = USB_HEADER;
//              response_host.opcode = USB_WRT_EDID;
//              response_host.opcode_status = 0x01;
//              printf("Response USB_WRT_EDID \r\n");
//              USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&response_host, 0x40);
//              // your function
//              break;
//              
//            case USB_REPLY_EDID:
//              response_host.header = USB_HEADER;
//              response_host.opcode = USB_REPLY_EDID;
//              response_host.opcode_status = 0x01;
//              printf("Response USB_REPLY_EDID \r\n");
//              USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&response_host, 0x40);
//              // your function
//              break;
//              
//           case USB_GET_MOUSE_PORT_STATUS:
//              response_host.header = USB_HEADER;
//              response_host.opcode = USB_GET_MOUSE_PORT_STATUS;
//              response_host.opcode_status = 0x01;
//              printf("Response USB_GET_MOUSE_PORT_STATUS \r\n");
//              USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&response_host, 0x40);
//              // your function
//              break;
//              
//            case USB_GET_VGA_PORT_STATUS:
//              response_host.header = USB_HEADER;
//              response_host.opcode = USB_GET_VGA_PORT_STATUS;
//              response_host.opcode_status = 0x01;
//              printf("Response USB_GET_VGA_PORT_STATUS \r\n");
//              USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&response_host, 0x40);
//              // your function
//              break;
//              
//            default:
//              printf("OPCODE is wrong! 0x%02x \r\n", usb_msg->opcode);
//              break;
//          }
//        }
//        else
//        {
//          printf("Header is wrong 0x%02x \r\n", usb_msg->header);
//          printf("But tx_buffer[0] is 0x%02x \r\n", tx_buffer[0]);
//        }
//        flag_rx = 0;
//      }       
//  } // end while
//}

//void authenKVM(void)
//{  
//    uint8_t i;
//    usb_msg = (usb_msg_format_t *)tx_buffer;
//    usb_msg_format_t response_host;
//    static uint8_t switch_status = USB_NO_READY;
//  
//  
//    if (flag_rx == 1)
//    {
//      if(usb_msg->header == USB_HEADER)
//      {
//        switch(usb_msg->opcode)
//        {
//          case USB_ACK:
//            response_host.header = USB_HEADER;
//            response_host.opcode = USB_ACK;
//            response_host.opcode_status = 0x01;
//            printf("Response USB_ACK \r\n");
//            USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&response_host, 0x40);            
//            
//            break;
//            
//          case USB_AUTHEN_ING:
//            response_host.header = USB_HEADER;
//            response_host.opcode = USB_AUTHEN_ING;
//            response_host.opcode_status = 0x01;
//            printf("Receive USB_AUTHEN_ING \r\n");
//            
//            break;
//            
//          case USB_AUTHEN_FINISH:
//            response_host.header = USB_HEADER;
//            response_host.opcode = USB_AUTHEN_FINISH;
//            response_host.opcode_status = 0x01;
//            printf("Receive USB_AUTHEN_FINISH \r\n");
//            break;
//             
//          default:
//            printf("OPCODE is wrong! 0x%02x \r\n", usb_msg->opcode);
//            break;            
//        }
//        
//        switch(switch_status)
//        {
//          case USB_NO_READY:
//            if(usb_msg->opcode == USB_ACK)
//            {
//              switch_status = USB_INIT;
//              printf("Go to USB_INIT \r\n");
//            }
//            else
//              printf("USB_NO_READY \r\n");
//            break;
//            
//          case USB_INIT:
//            if(usb_msg->opcode == USB_AUTHEN_ING)
//            {
//              DeCryptAes128((uint8_t *)UID , usb_msg->data, 48, key_aes);
//              //      AES_ECB_encrypt((const uint8_t*)&input_str[i], (const uint8_t*)key_aes, &output_str[i], 16);
//              USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&response_host, 0x40);
//              printf("Go to USB_AUTHEN_ING \r\n");
//              switch_status = USB_AUTHEN_ING;
//            }
//            else
//              printf("USB_INIT \r\n");
//            break;
//            
//          case USB_AUTHEN_ING:
//            if(usb_msg->opcode == USB_AUTHEN_ING)
//            {
//               switch_status = USB_AUTHEN_FINISH;              
//            }
//            else
//              printf("USB_AUTHEN_ING \r\n");
//            break;
//              
//          case USB_AUTHEN_FINISH:
//            printf("Receive USB_AUTHEN_FINISH \r\n");
//            USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&response_host, 0x40);
//
//            break;
//            
//          default:
//            
//            break;       
//              
//        }
//        
//      }
//      flag_rx = 0;
//    }
//    
//}

void usb_kvm_switch_init(void)
{
    if(!elinkswitch_get_usb_triggers(&elinkswitch_usb_trigger))
    {
        printf("\r\n elinkswitch_get_usb_triggers error \r\n");
    }
}

//void usb_kvm_switch_1ms_elapsed(void)
//{
//	static uint16_t ms = 0;
//	static uint8_t seconds = 0;
//	ms++;
//	if(ms == 1000)
//	{
//		ms = 0;
//		seconds++;
//		if(seconds == 60)
//		{
//			seconds = 0;
//		}
//		if(seconds == 5)
//		{
//			elinkswitch_usb_trigger.authorized();
//		}else if(seconds == 10)
//		{
//			elinkswitch_usb_trigger.receive_usb_command(ELINKSWITCH_RECEIVED_USB_COMMAND_GET_USB_PORTS_STATUS,NULL, 0,elinkswitch_usb_out_buff, &elinkswitch_usb_out_length);
//		}else if(seconds == 15)
//		{
//			static uint8_t temp = 0;
//			elinkswitch_usb_in_buff[0] = temp++;
//			elinkswitch_usb_trigger.receive_usb_command(ELINKSWITCH_RECEIVED_USB_COMMAND_SET_USB_PORTS,elinkswitch_usb_in_buff, 1,NULL, 0);
//		}else if(seconds == 20)
//		{
//			elinkswitch_usb_trigger.receive_usb_command(ELINKSWITCH_RECEIVED_USB_COMMAND_GET_VGA_PORTS_STATUS,NULL, 0,elinkswitch_usb_out_buff, &elinkswitch_usb_out_length);
//		}else if(seconds == 25)
//		{
//			static uint8_t temp = 0;
//			elinkswitch_usb_in_buff[0] = temp++;
//			elinkswitch_usb_trigger.receive_usb_command(ELINKSWITCH_RECEIVED_USB_COMMAND_SET_VGA_PORTS,elinkswitch_usb_in_buff, 1,NULL, 0);
//		}else if(seconds == 30)
//		{
//			elinkswitch_usb_trigger.receive_usb_command(ELINKSWITCH_RECEIVED_USB_COMMAND_GET_USB_PORTS_STATUS,NULL, 0,elinkswitch_usb_out_buff, &elinkswitch_usb_out_length);
//		}else if(seconds == 35)
//		{
//			static uint8_t temp = 0;
//			elinkswitch_usb_in_buff[0] = temp++;
//			elinkswitch_usb_trigger.receive_usb_command(ELINKSWITCH_RECEIVED_USB_COMMAND_SET_USB_PORTS,elinkswitch_usb_in_buff, 1,NULL, 0);
//		}else if(seconds == 40)
//		{
//			elinkswitch_usb_trigger.receive_usb_command(ELINKSWITCH_RECEIVED_USB_COMMAND_GET_VGA_PORTS_STATUS,NULL, 0,elinkswitch_usb_out_buff, &elinkswitch_usb_out_length);
//		}else if(seconds == 45)
//		{
//			static uint8_t temp = 0;
//			elinkswitch_usb_in_buff[0] = temp++;
//			elinkswitch_usb_trigger.receive_usb_command(ELINKSWITCH_RECEIVED_USB_COMMAND_SET_VGA_PORTS,elinkswitch_usb_in_buff, 1,NULL, 0);
//		}else if(seconds == 50)
//		{
//			elinkswitch_usb_trigger.back_to_inited();
//		}
//	}
//}

