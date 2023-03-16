#include "system_switch.h"
#include "main.h"
#include "usbd_def.h"
#include "usbd_customhid.h"
#include "usb_sw_selector.h"
#include "elinkswitch.h"

uint32_t lastTimeSystemSwitchTask = 0;
uint8_t lastChannelSelectSW = 10;
//static elinkswitch_usb_trigger_t elinkswitch_usb_trigger;
//static uint8_t elinkswitch_usb_out_buff[128];
//static uint16_t elinkswitch_usb_out_length;
//extern USBD_HandleTypeDef hUsbDeviceFS;

/**
 * @brief switch channel
 * @param
 * @retval None
 */
void system_switch(uint8_t channel)
{
//   if(HAL_GetTick() > 1000){
//     usb_msg_format_t response_host;
//     response_host.header = USB_HEADER;
//     response_host.opcode = USB_GET_CHANNEL;
//     response_host.opcode_status = 0x01;
//     // Notify and get result
//     // elinkswitch_usb_trigger.receive_usb_command(ELINKSWITCH_RECEIVED_USB_COMMAND_GET_CHANNEL_STATUS, NULL, 0, elinkswitch_usb_out_buff, &elinkswitch_usb_out_length);
//     // ToDo: Retrieve data from elinkswitch_usb_out_buff and send to USB host
//     response_host.data[0] = channel;
//     printf("Response USB_GET_CHANNEL_STATUS %d \r\n", response_host.data[0]);
//     USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&response_host, 0x40);
//   }
    if (channel == 0)
    {
        system_disable();
    }
    else if (channel > 0 && channel < 5)
    {
        channel--;
        HAL_GPIO_WritePin(USB_SW_SEL0_GPIO_Port, USB_SW_SEL0_Pin, (channel % 2));
        HAL_GPIO_WritePin(USB_SW_SEL1_GPIO_Port, USB_SW_SEL1_Pin, ((channel / 2) % 2));
        system_enable();
    }
}

void system_enable()
{
    HAL_GPIO_WritePin(KVMSW_EN_GPIO_Port, KVMSW_EN_Pin, 1);
}
void system_disable()
{
    HAL_GPIO_WritePin(KVMSW_EN_GPIO_Port, KVMSW_EN_Pin, 0);
}

void system_switch_init()
{
    // power enable
    // switch disaple
    HAL_GPIO_WritePin(KVMSW_EN_GPIO_Port, KVMSW_EN_Pin, 0);
}

void system_switch_tasks()
{
    if (HAL_GetTick() - lastTimeSystemSwitchTask > 373)
    {
        lastTimeSystemSwitchTask = HAL_GetTick();
        uint8_t cc = get_current_channel();
        if (lastChannelSelectSW != cc)
        {
            lastChannelSelectSW = cc;
            system_switch(cc);
        }
    }
}