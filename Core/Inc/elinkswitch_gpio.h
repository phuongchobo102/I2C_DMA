/**
 ********************************************************************************
 * @file    elinkswitch_gpio.h
 * @author  Phu Tran, Minh La, An Le, Vinh Pham
 * @date    Jan 2023
 * @brief   
 ********************************************************************************
 */

#ifndef _ELINKSWITCH_GPIO_H_
#define _ELINKSWITCH_GPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

/************************************
 * INCLUDES
 ************************************/
#include <stdbool.h>
/************************************
 * MACROS AND DEFINES
 ************************************/

/************************************
 * TYPEDEFS
 ************************************/

/************************************
 * EXPORTED VARIABLES
 ************************************/

/************************************
 * GLOBAL FUNCTION PROTOTYPES
 ************************************/
extern bool elsgpio_init(void);
extern void elsgpio_task(void);
extern void elsgpio_1ms_elapsed(void);

#ifdef __cplusplus
}
#endif 

#endif  /* _ELINKSWITCH_GPIO_H_ */

/*** end of file ***/