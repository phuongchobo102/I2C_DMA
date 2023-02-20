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
/************************************
 * PRIVATE PREPROCESSOR DEFINES
 ************************************/
 
 /************************************
 * PRIVATE MACROS
 ************************************/

/************************************
 * PRIVATE TYPEDEFS
 ************************************/
 
 /************************************
 * EXPORTED VARIABLES DEFINITION
 ************************************/

/************************************
 * PRIVATE & STATIC CONSTANTS
 ************************************/

/************************************
 * PRIVATE & STATIC VARIABLES
 ************************************/
elinkswitch_state_e elinkswitch_current_state = ELINKSWITCH_STATE_NOT_INIT;
/************************************
 * GLOBAL VARIABLES
 ************************************/

/************************************
 * PRIVATE&STATIC FUNCTION PROTOTYPES
 ************************************/

/************************************
 * PRIVATE & STATIC FUNCTIONS
 ************************************/

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
	elinkswitch_current_state = ELINKSWITCH_STATE_INITED;
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
