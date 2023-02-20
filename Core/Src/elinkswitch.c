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
 * @brief Identify the larger of two 8-bit integers.
 *
 * @param[in] num1  The first number to be compared.
 * @param[in] num2  The second number to be compared.
 *
 * @return The value of the larger number.
 *
 * This function must be run before crc_fast() or the table stored in ROM.
 */
int8_t 
max8 (int8_t num1, int8_t num2)
{
    return ((num1 > num2) ? num1 : num2);
}

/*** end of file ***/
