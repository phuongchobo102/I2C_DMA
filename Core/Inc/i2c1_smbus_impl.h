/**
 ********************************************************************************
 * @file    ${file_name}
 * @author  ${user}
 * @date    ${date}
 * @brief
 * COPYRIGHT NOTICE: (c) 2022 Firmware Bros Group.  All rights reserved. 
 ********************************************************************************
 */

#ifndef _I2C1_SMBUS_IMPL_H_
#define _I2C1_SMBUS_IMPL_H_

#ifdef __cplusplus
extern "C" {
#endif

/************************************
 * INCLUDES
 ************************************/
#include "stm32_SMBUS_stack.h"
/************************************
 * EXPORTED PREPROCESSOR DEFINES
 ************************************/
 
 /************************************
 * EXPORTED MACROS
 ************************************/

/************************************
 * EXPORTED TYPEDEFS
 ************************************/
 
 /************************************
 * EXPORTED CONSTANTS
 ************************************/
extern st_command_t const EDID_COMMANDS_TABLE[];
/************************************
 * EXPORTED VARIABLES
 ************************************/
extern uint8_t       *piobuf;
extern SMBUS_HandleTypeDef *phandle1;
extern SMBUS_StackHandleTypeDef *pcontext1;
extern SMBUS_StackHandleTypeDef context1;
extern uint32_t i2c1_smbus_lastTime;




/************************************
 * EXPORTED  FUNCTION PROTOTYPES
 ************************************/
extern void i2c1_smbus_init(SMBUS_HandleTypeDef* hsmbus);
extern void i2c1_smbus_task(void);
#ifdef __cplusplus
}
#endif



#endif /* _I2C1_SMBUS_IMPL_H_ */

/*** end of file ***/
