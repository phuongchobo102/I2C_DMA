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
extern "C" {
#endif

/************************************
 * INCLUDES
 ************************************/

/************************************
 * EXPORTED PREPROCESSOR DEFINES
 ************************************/
 
 /************************************
 * EXPORTED MACROS
 ************************************/

/************************************
 * EXPORTED TYPEDEFS
 ************************************/
 typedef enum elinkswitch_state_enum {
	 ELINKSWITCH_STATE_NOT_INIT,
	 ELINKSWITCH_STATE_INITED,
	 ELINKSWITCH_STATE_AUTHORIZED,
	 ELINKSWITCH_STATE_RECEIVED_USB_COMMAND,
	 ELINKSWITCH_STATE_RECEIVED_BTN_EVENT
 }elinkswitch_state_e;
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
void elinkswitch_task(void);
void elinkswitch_1ms_elapsed(void);

#ifdef __cplusplus
}
#endif



#endif /* _ELINKSWITCH_H_ */

/*** end of file ***/
