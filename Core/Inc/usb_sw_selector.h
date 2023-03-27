#include "main.h"


typedef struct usb_msg_format {
  uint8_t header;
  uint8_t opcode;
  uint8_t opcode_status;
  uint8_t len;
  uint8_t data[58];
  uint8_t crc;  
} usb_msg_format_t;


#define USB_HEADER 0x5A
#define LEN_ACK_MSG 6
#define LEN_AUTHEN_MSG 6

enum USB_OPCODE {
  USB_STATUS = 0,
  USB_ACK,
  USB_AUTHEN_ING,
  USB_AUTHEN_FINISH,
  USB_GET_MOUSE_PORT_STATUS,
  USB_GET_VGA_PORT_STATUS,  
  USB_SET_USB_PORT,
  USB_SET_VGA_PORT, 
  USB_GET_CHANNEL,
  USB_WRT_EDID,
  USB_REPLY_EDID
};
  
enum AUTHEN_STATUS {
  USB_NO_READY,
  USB_INIT
//  USB_AUTHEN_ING,
//  USB_AUTHEN_FINISH
};

//static uint8_t elinkswitch_usb_in_buff[128];
//static uint8_t elinkswitch_usb_out_buff[128];
//static uint16_t elinkswitch_usb_in_length;
//static uint16_t elinkswitch_usb_out_length;

void usb_sw_enable(uint8_t isTrue);
void usb_sw_select(uint8_t portIndex) ;
void process_usb_msg(usb_msg_format_t *usb_msg);
void authenKVM_init(void);
void authenKVM(void);

void usb_kvm_switch_init(void);
//void usb_kvm_switch_1ms_elapsed(void);


