/**
 ********************************************************************************
 * @file    elinkswitch.h
 * @author  Phu Tran
 * @date    20th Feb 2023
 * @brief
 * COPYRIGHT NOTICE: (c) 2023 Firmware Bros Group.  All rights reserved.
 ********************************************************************************
 */

#ifndef _ELINKSWITCH_H_
#define _ELINKSWITCH_H_

#ifdef __cplusplus
extern "C"
{
#endif

/************************************
 * INCLUDES
 ************************************/
#include <stdbool.h>
#include <stdint.h>
	// #include "main.h"
	/************************************
	 * EXPORTED PREPROCESSOR DEFINES
	 ************************************/

	/************************************
	 * EXPORTED MACROS
	 ************************************/

	/************************************
	 * EXPORTED TYPEDEFS
	 ************************************/
	typedef enum elinkswitch_state_enum
	{
		ELINKSWITCH_STATE_NOT_INIT,
		ELINKSWITCH_STATE_INITED,
		ELINKSWITCH_STATE_AUTHORIZED,
		ELINKSWITCH_STATE_RECEIVED_USB_COMMAND,
		ELINKSWITCH_STATE_RECEIVED_BTN_EVENT
	} elinkswitch_state_e;

	typedef enum elinkswitch_button_event_enum
	{
		ELINKSWITCH_BUTTON1_ON_RELEASE,
		ELINKSWITCH_BUTTON2_ON_RELEASE,
		ELINKSWITCH_BUTTON3_ON_RELEASE,
		ELINKSWITCH_BUTTON4_ON_RELEASE,
		ELINKSWITCH_BUTTON5_ON_RELEASE,
		ELINKSWITCH_USB_ON_CHANGE,
		ELINKSWITCH_VGA_ON_CHANGE,
		ELINKSWITCH_BUTTONEDID_ON_RELEASE
	} elinkswitch_button_event_e;

	typedef enum elinkswitch_received_usb_command_event_enum
	{
		ELINKSWITCH_RECEIVED_USB_COMMAND_GET_USB_PORTS_STATUS,
		ELINKSWITCH_RECEIVED_USB_COMMAND_GET_VGA_PORTS_STATUS,
		ELINKSWITCH_RECEIVED_USB_COMMAND_GET_CHANNEL_STATUS,
		ELINKSWITCH_RECEIVED_USB_COMMAND_GET_STORED_VGA_ID,
		ELINKSWITCH_RECEIVED_USB_COMMAND_SET_USB_PORTS,
		ELINKSWITCH_RECEIVED_USB_COMMAND_SET_VGA_PORTS,
		ELINKSWITCH_RECEIVED_USB_COMMAND_SET_STORED_VGA_ID
	} elinkswitch_received_usb_command_event_e;

	typedef void (*elinkswitch_state_change_event)(elinkswitch_state_e new_state);
	typedef bool (*elinkswitch_trigger_change_state)(void);
	typedef bool (*elinkswitch_received_usb_command_event)(elinkswitch_received_usb_command_event_e event, const uint8_t *in, uint16_t in_length, uint8_t *out, uint16_t *out_length);
	typedef bool (*elinkswitch_button_event)(elinkswitch_button_event_e btn_event);

	typedef struct elinkswitch_usb_trigger_struct
	{
		elinkswitch_trigger_change_state authorized;
		elinkswitch_trigger_change_state back_to_inited;
		elinkswitch_received_usb_command_event receive_usb_command;
	} elinkswitch_usb_trigger_t;

	/************************************
	 * EXPORTED CONSTANTS
	 ************************************/

	/************************************
	 * EXPORTED VARIABLES
	 ************************************/

	/************************************
	 * EXPORTED  FUNCTION PROTOTYPES
	 ************************************/
	void elinkswitch_init(void);
	bool elinkswitch_register_state_change_event_listener(elinkswitch_state_change_event listener);
	bool elinkswitch_get_usb_triggers(elinkswitch_usb_trigger_t *triggers);
	void elinkswitch_task(void);
	void elinkswitch_1ms_elapsed(void);

#ifdef __cplusplus
}
#endif

#endif /* _ELINKSWITCH_H_ */

/*** end of file ***/
