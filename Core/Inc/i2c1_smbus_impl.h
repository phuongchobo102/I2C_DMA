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

/************************************
 * EXPORTED VARIABLES
 ************************************/

/************************************
 * EXPORTED  FUNCTION PROTOTYPES
 ************************************/
extern void Defer_HAL_SMBUS_MasterTxCpltCallback(SMBUS_HandleTypeDef *hsmbus);
extern void Defer_HAL_SMBUS_MasterRxCpltCallback(SMBUS_HandleTypeDef *hsmbus);
extern void Defer_HAL_SMBUS_SlaveTxCpltCallback(SMBUS_HandleTypeDef *hsmbus);
extern void Defer_HAL_SMBUS_SlaveRxCpltCallback(SMBUS_HandleTypeDef *hsmbus);
extern void Defer_HAL_SMBUS_AddrCallback(SMBUS_HandleTypeDef *hsmbus, uint8_t TransferDirection, uint16_t AddrMatchCode);
extern void Defer_HAL_SMBUS_ListenCpltCallback(SMBUS_HandleTypeDef *hsmbus);
#ifdef __cplusplus
}
#endif



#endif /* _I2C1_SMBUS_IMPL_H_ */

/*** end of file ***/
