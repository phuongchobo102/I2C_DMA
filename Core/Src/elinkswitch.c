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
#include <stdint.h>
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
typedef struct elinkswitch_state_change_event_listener_struct {
  uint8_t max_size;
  uint8_t current_size;
  elinkswitch_state_change_event *list;
}elinkswitch_state_change_event_listener_t;
/************************************
* EXPORTED VARIABLES DEFINITION
************************************/

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
//elinkswitch_button_event elinkswitch_receive_button_event;

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
static bool lc_elinkswitch_state_switch_to_receive_usb_command(void);
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
  for(uint8_t i = 0; i < elinkswitch_state_change_event_listener.current_size;i++)
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
  elinkswitch_current_state = ELINKSWITCH_STATE_INITED;
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
static bool lc_elinkswitch_state_switch_to_receive_usb_command(void)
{
  if(elinkswitch_current_state == ELINKSWITCH_STATE_AUTHORIZED)
  {
    elinkswitch_current_state = ELINKSWITCH_STATE_RECEIVED_USB_COMMAND;
    
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
static bool lc_elinkswitch_receive_btn_event(elinkswitch_button_event_e btn_event)
{
  printf("receivr event button %d \r\n", btn_event);
  if((elinkswitch_current_state == ELINKSWITCH_STATE_AUTHORIZED) || (elinkswitch_current_state == ELINKSWITCH_STATE_RECEIVED_USB_COMMAND))
  {
    if(btn_event != ELINKSWITCH_BUTTON5_ON_RELEASE)
    {
      elinkswitch_current_state = ELINKSWITCH_STATE_RECEIVED_BTN_EVENT;
      
      elinkswitch_state_changed = true;
      
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
  //ToDo: init stuff here
  /* Initialize listeners*/
  elinkswitch_state_change_event_listener.current_size = 0;
  elinkswitch_state_change_event_listener.max_size = ELINKSWITCH_STATE_CHANGE_EVENT_MAX_LIST;
  elinkswitch_state_change_event_listener.list = elinkswitch_state_change_event_array;
  /* Init usb trigger */
  elinkswitch_usb_trigger.authorized = lc_elinkswitch_state_switch_to_authorized;
  elinkswitch_usb_trigger.back_to_inited = lc_elinkswitch_state_switch_back_to_inited;
  elinkswitch_usb_trigger.receive_usb_command = lc_elinkswitch_state_switch_to_receive_usb_command;
  /**/
  //	elinkswitch_receive_button_event = lc_elinkswitch_receive_btn_event;
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
bool elinkswitch_state_change_event_listener_register(elinkswitch_state_change_event listener)
{
  if(elinkswitch_state_change_event_listener.current_size < elinkswitch_state_change_event_listener.max_size)
  {
    elinkswitch_state_change_event_listener.list[elinkswitch_state_change_event_listener.current_size] = listener;
    elinkswitch_state_change_event_listener.current_size++;
    return true;
  }
  return false;
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
  if(elinkswitch_state_changed){
    elinkswitch_state_changed = false;
    lc_elinkswitch_state_change_notify();
  }
  switch(elinkswitch_current_state)
  {
  case ELINKSWITCH_STATE_NOT_INIT:
    break;
  case ELINKSWITCH_STATE_INITED:
    break;
  case ELINKSWITCH_STATE_AUTHORIZED:
    break;
  case ELINKSWITCH_STATE_RECEIVED_USB_COMMAND:
    break;
  case ELINKSWITCH_STATE_RECEIVED_BTN_EVENT:
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
