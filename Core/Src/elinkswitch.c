/**
********************************************************************************
* @file    elinkswitch.c
* @author  Phu Tran
* @date    20th Feb 2023
* @brief
********************************************************************************
*/

/************************************
 * INCLUDES
 ************************************/
#include "elinkswitch.h"
#include "elinkswitch_gpio.h"
#include "main.h"
#include "usbd_def.h"
#include "usbd_customhid.h"
#include "usb_sw_selector.h"
// #include "elinkswitch.h"

#include <stdio.h>
/************************************
 * PRIVATE PREPROCESSOR DEFINES
 ************************************/
#ifndef ELINKSWITCH_STATE_CHANGE_EVENT_MAX_LIST
#define ELINKSWITCH_STATE_CHANGE_EVENT_MAX_LIST 5
#endif /*ELINKSWITCH_STATE_CHANGE_EVENT_MAX_LIST*/
/************************************
 * PRIVATE MACROS
 ************************************/

/************************************
 * PRIVATE TYPEDEFS
 ************************************/
typedef struct elinkswitch_state_change_event_listener_struct
{
  uint8_t max_size;
  uint8_t current_size;
  elinkswitch_state_change_event *list;
} elinkswitch_state_change_event_listener_t;
/************************************
 * EXPORTED VARIABLES DEFINITION
 ************************************/
//static elinkswitch_usb_trigger_t elinkswitch_usb_trigger;
// static uint8_t elinkswitch_usb_out_buff[128];
// static uint16_t elinkswitch_usb_out_length;
extern USBD_HandleTypeDef hUsbDeviceFS;
/************************************
 * PRIVATE & STATIC CONSTANTS
 ************************************/
elinkswitch_state_change_event elinkswitch_state_change_event_array[ELINKSWITCH_STATE_CHANGE_EVENT_MAX_LIST];
/************************************
 * PRIVATE & STATIC VARIABLES
 ************************************/
elinkswitch_state_e elinkswitch_current_state = ELINKSWITCH_STATE_NOT_INIT;
elinkswitch_state_change_event_listener_t elinkswitch_state_change_event_listener;
elinkswitch_usb_trigger_t elinkswitch_usb_trigger;
bool is_elinkswitch_usb_trigger_set;
// elinkswitch_button_event elinkswitch_receive_button_event;

volatile bool elinkswitch_state_changed = false;
/************************************
 * GLOBAL VARIABLES
 ************************************/

/************************************
 * PRIVATE&STATIC FUNCTION PROTOTYPES
 ************************************/
static void lc_elinkswitch_state_change_notify(void);
static bool lc_elinkswitch_state_switch_to_authorized(void);
static bool lc_elinkswitch_state_switch_back_to_inited(void);
static bool lc_elinkswitch_state_switch_to_receive_usb_command(elinkswitch_received_usb_command_event_e event, const uint8_t *in, uint16_t in_length, uint8_t *out, uint16_t *out_length);
static bool lc_elinkswitch_receive_btn_event(elinkswitch_button_event_e btn_event);
/************************************
 * PRIVATE & STATIC FUNCTIONS
 ************************************/
/*!
 * @brief Notify to listeners that elinkswitch has changed state
 *
 * @params none
 *
 *
 * @return none
 *
 */
static void lc_elinkswitch_state_change_notify(void)
{
  for (uint8_t i = 0; i < elinkswitch_state_change_event_listener.current_size; i++)
  {
    elinkswitch_state_change_event_listener.list[i](elinkswitch_current_state);
  }
}

/*!
 * @brief Notify to listeners that elinkswitch has changed state
 *
 * @params none
 *
 *
 * @return none
 *
 */
static bool lc_elinkswitch_state_switch_to_authorized(void)
{
  elinkswitch_current_state = ELINKSWITCH_STATE_AUTHORIZED;
  elinkswitch_state_changed = true;
  return true;
}

/*!
 * @brief Notify to listeners that elinkswitch has changed state
 *
 * @params none
 *
 *
 * @return none
 *
 */
static bool lc_elinkswitch_state_switch_back_to_inited(void)
{
  if (elinkswitch_current_state != ELINKSWITCH_STATE_INITED)
  {
    elinkswitch_current_state = ELINKSWITCH_STATE_INITED;
    elinkswitch_state_changed = true;
    return true;
  }
  return false;
}

/*!
 * @brief Notify to listeners that elinkswitch has changed state
 *
 * @params none
 *
 *
 * @return none
 *
 */
static bool lc_elinkswitch_state_switch_to_receive_usb_command(elinkswitch_received_usb_command_event_e event, const uint8_t *in, uint16_t in_length, uint8_t *out, uint16_t *out_length)
{
  if (elinkswitch_current_state == ELINKSWITCH_STATE_AUTHORIZED)
  {
    /**
     * uint8_t channelSelect = 0;
uint8_t edidStatus;
uint8_t vgaStatus[4];
uint8_t usbStatus[4];
     */
    printf("go to lc_elinkswitch_state_switch_to_receive_usb_command \r\n");
    if (event == ELINKSWITCH_RECEIVED_USB_COMMAND_GET_USB_PORTS_STATUS)
    {
      if (out)
      {
        out[0] = get_current_usb(0); // usbStatus[0];
        out[1] = get_current_usb(1); // usbStatus[1];
        out[2] = get_current_usb(2); // usbStatus[2];
        out[3] = get_current_usb(3); // usbStatus[3];
      }
      if (out_length)
      {
        *out_length = 4;
      }
    }
    else if (event == ELINKSWITCH_RECEIVED_USB_COMMAND_GET_VGA_PORTS_STATUS)
    {
      if (out)
      {
        out[0] = get_current_vga(0); // vgaStatus[0];
        out[1] = get_current_vga(1); // vgaStatus[1];
        out[2] = get_current_vga(2); // vgaStatus[2];
        out[3] = get_current_vga(3); // vgaStatus[3];
      }
      if (out_length)
      {
        *out_length = 4;
      }
    }
    else if (event == ELINKSWITCH_RECEIVED_USB_COMMAND_GET_CHANNEL_STATUS)
    {
      if (out)
      {
        out[0] = get_current_channel(); //
      }
      if (out_length)
      {
        *out_length = 1;
      }
    }
    else if (event == ELINKSWITCH_RECEIVED_USB_COMMAND_GET_STORED_VGA_ID)
    {
    }
    else if ((event == ELINKSWITCH_RECEIVED_USB_COMMAND_SET_USB_PORTS) || (event == ELINKSWITCH_RECEIVED_USB_COMMAND_SET_VGA_PORTS))
    {
      if (in)
      {
        //        channelSelect = in[0];
        set_current_channel(in[0]);
        // set_current_channel(usb_msg->data[0]);
      }
    }
    else if (event == ELINKSWITCH_RECEIVED_USB_COMMAND_SET_STORED_VGA_ID)
    {
    }
    elinkswitch_current_state = ELINKSWITCH_STATE_RECEIVED_USB_COMMAND;

    elinkswitch_state_changed = true;
    return true;
  }
  else
  {
    printf("\r\n elinkswitch_current_state is not elinkswitch_current_state=%d,instead of %d(ELINKSWITCH_STATE_AUTHORIZED) \r\n", elinkswitch_current_state, ELINKSWITCH_STATE_AUTHORIZED);
  }
  return false;
}

/*!
 * @brief Notify to listeners that elinkswitch has changed state
 *
 * @params none
 *
 *
 * @return none
 *
 */
static bool lc_elinkswitch_receive_btn_event(elinkswitch_button_event_e btn_event)
{
  printf("receive event button %d \r\n", btn_event);
  if ((elinkswitch_current_state == ELINKSWITCH_STATE_AUTHORIZED) || (elinkswitch_current_state == ELINKSWITCH_STATE_RECEIVED_USB_COMMAND))
  {
    // if (btn_event != ELINKSWITCH_BUTTON5_ON_RELEASE)
    switch (btn_event)
    {
    case ELINKSWITCH_BUTTON1_ON_RELEASE ... ELINKSWITCH_BUTTON4_ON_RELEASE:
      elinkswitch_current_state = ELINKSWITCH_STATE_RECEIVED_BTN_EVENT;

      elinkswitch_state_changed = true;
      if (hUsbDeviceFS.dev_state != USBD_STATE_SUSPENDED)
      {
        usb_msg_format_t response_host;
        response_host.header = USB_HEADER;
        response_host.opcode = USB_GET_CHANNEL;
        response_host.opcode_status = 0x01;
        response_host.data[0] = get_current_channel();
        printf("Response USB_GET_CHANNEL_STATUS %d \r\n", response_host.data[0]);
        USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&response_host, 0x40);
      }
      break;
    case ELINKSWITCH_USB_ON_CHANGE:
      elinkswitch_current_state = ELINKSWITCH_STATE_RECEIVED_BTN_EVENT;
      elinkswitch_state_changed = true;
      if (hUsbDeviceFS.dev_state != USBD_STATE_SUSPENDED)
      {
        usb_msg_format_t response_host;
        response_host.header = USB_HEADER;
        response_host.opcode = USB_GET_MOUSE_PORT_STATUS;
        response_host.opcode_status = 0x01;
        response_host.data[0] = get_current_usb(0);
        response_host.data[1] = get_current_usb(1);
        response_host.data[2] = get_current_usb(2);
        response_host.data[3] = get_current_usb(3);
        printf("Response USB_GET_MOUSE_PORT_STATUS %d \r\n", response_host.data[0]);
        USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&response_host, 0x40);
      }
      break;
    case ELINKSWITCH_VGA_ON_CHANGE:
      elinkswitch_current_state = ELINKSWITCH_STATE_RECEIVED_BTN_EVENT;
      elinkswitch_state_changed = true;
      if (hUsbDeviceFS.dev_state != USBD_STATE_SUSPENDED)
      {
        usb_msg_format_t response_host;
        response_host.header = USB_HEADER;
        response_host.opcode = USB_GET_VGA_PORT_STATUS;
        response_host.opcode_status = 0x01;
        response_host.data[0] = get_current_vga(0);
        response_host.data[1] = get_current_vga(1);
        response_host.data[2] = get_current_vga(2);
        response_host.data[3] = get_current_vga(3);
        printf("Response USB_GET_MOUSE_PORT_STATUS %d \r\n", response_host.data[0]);
        USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&response_host, 0x40);
      }
      break;
    case ELINKSWITCH_BUTTONEDID_ON_RELEASE:
      elinkswitch_current_state = ELINKSWITCH_STATE_RECEIVED_BTN_EVENT;
      elinkswitch_state_changed = true;
      if (hUsbDeviceFS.dev_state != USBD_STATE_SUSPENDED)
      {
        usb_msg_format_t response_host;
        response_host.header = USB_HEADER;
        response_host.opcode = USB_GET_CHANNEL;
        response_host.opcode_status = 0x01;
        response_host.data[0] = get_current_channel();
        response_host.data[1] = get_current_edid();
        printf("Response USB_GET_MOUSE_PORT_STATUS %d \r\n", response_host.data[0]);
        USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t *)&response_host, 0x40);
      }
      break;
    default:
      break;
    }
    return true;
  }
  return false;
}
/************************************
 * GLOBAL FUNCTIONS
 ************************************/

/*!
 * @brief Initialize the elinkswitch functionality.
 *
 * @params none
 *
 *
 * @return none
 *
 */
void elinkswitch_init(void)
{
  // ToDo: init stuff here
  /* Initialize listeners*/
  elinkswitch_state_change_event_listener.current_size = 0;
  elinkswitch_state_change_event_listener.max_size = ELINKSWITCH_STATE_CHANGE_EVENT_MAX_LIST;
  elinkswitch_state_change_event_listener.list = elinkswitch_state_change_event_array;
  /* Init usb trigger */
  elinkswitch_usb_trigger.authorized = lc_elinkswitch_state_switch_to_authorized;
  elinkswitch_usb_trigger.back_to_inited = lc_elinkswitch_state_switch_back_to_inited;
  elinkswitch_usb_trigger.receive_usb_command = lc_elinkswitch_state_switch_to_receive_usb_command;
  is_elinkswitch_usb_trigger_set = false;
  /**/
  // elinkswitch_receive_button_event = lc_elinkswitch_receive_btn_event;
  elsgpio_register_button_event_listener(lc_elinkswitch_receive_btn_event);
  /* Mark that elink switch is inited*/
  elinkswitch_current_state = ELINKSWITCH_STATE_INITED;
}

/*!
 * @brief Register listener to receive elink switch state change.
 *
 * @params none
 *
 *
 * @return true: if it is success, otherwise is fail
 *
 */
bool elinkswitch_register_state_change_event_listener(elinkswitch_state_change_event listener)
{
  if (elinkswitch_state_change_event_listener.current_size < elinkswitch_state_change_event_listener.max_size)
  {
    elinkswitch_state_change_event_listener.list[elinkswitch_state_change_event_listener.current_size] = listener;
    elinkswitch_state_change_event_listener.current_size++;
    return true;
  }
  return false;
}

/*!
 * @brief Get usb triggers.
 *
 * @param[out] triggers actual trigger functions if there is no error
 *
 *
 * @return true: if it is success, otherwise is fail
 * This function should only be called from usb portion.
 */
bool elinkswitch_get_usb_triggers(elinkswitch_usb_trigger_t *triggers)
{

  if (!is_elinkswitch_usb_trigger_set)
  {
    if (!triggers)
    {
      triggers = &elinkswitch_usb_trigger;
    }
    triggers->authorized = elinkswitch_usb_trigger.authorized;
    triggers->back_to_inited = elinkswitch_usb_trigger.back_to_inited;
    triggers->receive_usb_command = elinkswitch_usb_trigger.receive_usb_command;
    is_elinkswitch_usb_trigger_set = true;
    return true;
  }
}

/*!
 * @brief routine to run all stuffs of elinkswitch.
 *
 * @params none
 *
 *
 * @return none
 *
 */
void elinkswitch_task(void)
{
  if (elinkswitch_state_changed)
  {
    elinkswitch_state_changed = false;
    lc_elinkswitch_state_change_notify();
  }
  switch (elinkswitch_current_state)
  {
  case ELINKSWITCH_STATE_NOT_INIT:
    break;
  case ELINKSWITCH_STATE_INITED:
    break;
  case ELINKSWITCH_STATE_AUTHORIZED:
    break;
  case ELINKSWITCH_STATE_RECEIVED_USB_COMMAND:
    elinkswitch_current_state = ELINKSWITCH_STATE_AUTHORIZED;
    break;
  case ELINKSWITCH_STATE_RECEIVED_BTN_EVENT:
    elinkswitch_current_state = ELINKSWITCH_STATE_AUTHORIZED;
    break;
  }
}

/*!
 * @brief Handling 1ms event.
 *
 * @params none
 *
 *
 * @return none
 *
 */
void elinkswitch_1ms_elapsed(void)
{
}
/*** end of file ***/
