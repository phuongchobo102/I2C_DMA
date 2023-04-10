/**
 ********************************************************************************
 * @file    ${file_name}
 * @author  ${user}
 * @date    ${date}
 * @brief   
 ********************************************************************************
 */

/************************************
 * INCLUDES
 ************************************/
#include "i2c1_smbus_impl.h"
#include "main.h"
/************************************
 * PRIVATE PREPROCESSOR DEFINES
 ************************************/
 /* Use following constants to set up speed of your interface (48MHz clock)*/
#define SMBUS_TIMING_10K                      ((uint32_t)0xB042C3C7) /* 10kHz speed */
#define SMBUS_TIMING_100K                     ((uint32_t)0xB0420F13) /* 100kHz speed */
#define SMBUS_TIMING_400K                     ((uint32_t)0x50330309) /* 400kHz speed */
#define SMBUS_TIMING_1M                       ((uint32_t)0x50100103) /* 1MHz speed */
#define SMBUS_TIMEOUT_DEFAULT                 ((uint32_t)0x80618061)


#define EDID_COMMANDS_TABBLE_SIZE         		((uint8_t)256)
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
/* ----------- definition of PMBUS commands ---------------- */
st_command_t const EDID_COMMANDS_TABLE[] =
{
	{ 0x00, READ_OR_WRITE, 0, 1 },											/* code 00 */
	{ 0x01, READ_OR_WRITE, 0, 1 },	                        				/* code 01 */
	{ 0x02, READ_OR_WRITE, 0, 1 },	                    					/* code 02 */
	{ 0x03, READ_OR_WRITE, 0, 1 },	                             			/* code 03 */
	{ 0x04, READ_OR_WRITE, 0, 1 },                              			/* code 04 */
	{ 0x05, READ_OR_WRITE, 0, 1 },                      					/* code 05 */
	{ 0x06, READ_OR_WRITE, 0, 1 },                      					/* code 06 */
	{ 0x07, READ_OR_WRITE, 0, 1 },                        					/* code 07 */
	{ 0x08, READ_OR_WRITE, 0, 1 },                        					/* code 08 */
	{ 0x09, READ_OR_WRITE, 0, 1 },                        					/* code 09 */
	{ 0x0A, READ_OR_WRITE, 0, 1 },                      					/* code 0A */
	{ 0x0B, READ_OR_WRITE, 0, 1 },                      					/* code 0B */
	{ 0x0C, READ_OR_WRITE, 0, 1 },                        					/* code 0C */
	{ 0x0D, READ_OR_WRITE, 0, 1 },                        					/* code 0D */
	{ 0x0E, READ_OR_WRITE, 0, 1 },                        					/* code 0E */
	{ 0x0F, READ_OR_WRITE, 0, 1 },											/* code 0F */
	{ 0x10, READ_OR_WRITE, 0, 1 },											/* code 10 */
	{ 0x11, READ_OR_WRITE, 0, 1 },                          				/* code 11 */
	{ 0x12, READ_OR_WRITE, 0, 1 },                          				/* code 12 */
	{ 0x13, READ_OR_WRITE, 0, 1 },                          				/* code 13 */
	{ 0x14, READ_OR_WRITE, 0, 1 },                       					/* code 14 */
	{ 0x15, READ_OR_WRITE, 0, 1 },											/* code 15 */
	{ 0x16, READ_OR_WRITE, 0, 1 },                           				/* code 16 */
	{ 0x17, READ_OR_WRITE, 0, 1 },											/* code 17 */
	{ 0x18, READ_OR_WRITE, 0, 1 },                          				/* code 18 */
	{ 0x19, READ_OR_WRITE, 0, 1 },                                  		/* code 19 */
	{ 0x1A, READ_OR_WRITE, 0, 1 },                               			/* code 1A */
	{ 0x1B, READ_OR_WRITE, 0, 1 },                      					/* code 1B */
	{ 0x1C, READ_OR_WRITE, 0, 1 },                               			/* code 1C */
	{ 0x1D, READ_OR_WRITE, 0, 1 },                      					/* code 1D */
	{ 0x1E, READ_OR_WRITE, 0, 1 },                               			/* code 1E */
	{ 0x1F, READ_OR_WRITE, 0, 1 },                      					/* code 1F */
	{ 0x20, READ_OR_WRITE, 0, 1 },											/* code 20 */
	{ 0x21, READ_OR_WRITE, 0, 1 },											/* code 21 */
	{ 0x22, READ_OR_WRITE, 0, 1 },											/* code 22 */
	{ 0x23, READ_OR_WRITE, 0, 1 },											/* code 23 */
	{ 0x24, READ_OR_WRITE, 0, 1 },											/* code 24 */
	{ 0x25, READ_OR_WRITE, 0, 1 },											/* code 25 */
	{ 0x26, READ_OR_WRITE, 0, 1 },											/* code 26 */
	{ 0x27, READ_OR_WRITE, 0, 1 },											/* code 27 */
	{ 0x28, READ_OR_WRITE, 0, 1 },											/* code 28 */
	{ 0x29, READ_OR_WRITE, 0, 1 },											/* code 29 */
	{ 0x2A, READ_OR_WRITE, 0, 1 },											/* code 2A */
	{ 0x2B, READ_OR_WRITE, 0, 1 },											/* code 2B */
	{ 0x2C, READ_OR_WRITE, 0, 1 },											/* code 2C */
	{ 0x2D, READ_OR_WRITE, 0, 1 },											/* code 2D */
	{ 0x2E, READ_OR_WRITE, 0, 1 },											/* code 2E */
	{ 0x2F, READ_OR_WRITE, 0, 1 },											/* code 2F */
	{ 0x30, READ_OR_WRITE, 0, 1 },											/* code 30 */
	{ 0x31, READ_OR_WRITE, 0, 1 },											/* code 31 */
	{ 0x32, READ_OR_WRITE, 0, 1 },											/* code 32 */
	{ 0x33, READ_OR_WRITE, 0, 1 },											/* code 33 */
	{ 0x34, READ_OR_WRITE, 0, 1 },											/* code 34 */
	{ 0x35, READ_OR_WRITE, 0, 1 },											/* code 35 */
	{ 0x36, READ_OR_WRITE, 0, 1 },											/* code 36 */
	{ 0x37, READ_OR_WRITE, 0, 1 },											/* code 37 */
	{ 0x38, READ_OR_WRITE, 0, 1 },											/* code 38 */
	{ 0x39, READ_OR_WRITE, 0, 1 },											/* code 39 */
	{ 0x3A, READ_OR_WRITE, 0, 1 },											/* code 3A */
	{ 0x3B, READ_OR_WRITE, 0, 1 },											/* code 3B */
	{ 0x3C, READ_OR_WRITE, 0, 1 },											/* code 3C */
	{ 0x3D, READ_OR_WRITE, 0, 1 },											/* code 3D */
	{ 0x3E, READ_OR_WRITE, 0, 1 },											/* code 3E */
	{ 0x3F, READ_OR_WRITE, 0, 1 },											/* code 3F */
	{ 0x40, READ_OR_WRITE, 0, 1 },											/* code 40 */
	{ 0x41, READ_OR_WRITE, 0, 1 },											/* code 41 */
	{ 0x42, READ_OR_WRITE, 0, 1 },											/* code 42 */
	{ 0x43, READ_OR_WRITE, 0, 1 },											/* code 43 */
	{ 0x44, READ_OR_WRITE, 0, 1 },											/* code 44 */
	{ 0x45, READ_OR_WRITE, 0, 1 },											/* code 45 */
	{ 0x46, READ_OR_WRITE, 0, 1 },											/* code 46 */
	{ 0x47, READ_OR_WRITE, 0, 1 },											/* code 47 */
	{ 0x48, READ_OR_WRITE, 0, 1 },											/* code 48 */
	{ 0x49, READ_OR_WRITE, 0, 1 },											/* code 49 */
	{ 0x4A, READ_OR_WRITE, 0, 1 },											/* code 4A */
	{ 0x4B, READ_OR_WRITE, 0, 1 },											/* code 4B */
	{ 0x4C, READ_OR_WRITE, 0, 1 },											/* code 4C */
	{ 0x4D, READ_OR_WRITE, 0, 1 },											/* code 4D */
	{ 0x4E, READ_OR_WRITE, 0, 1 },											/* code 4E */
	{ 0x4F, READ_OR_WRITE, 0, 1 },											/* code 4F */
	{ 0x50, READ_OR_WRITE, 0, 1 },											/* code 50 */
	{ 0x51, READ_OR_WRITE, 0, 1 },											/* code 51 */
	{ 0x52, READ_OR_WRITE, 0, 1 },											/* code 52 */
	{ 0x53, READ_OR_WRITE, 0, 1 },											/* code 53 */
	{ 0x54, READ_OR_WRITE, 0, 1 },											/* code 54 */
	{ 0x55, READ_OR_WRITE, 0, 1 },											/* code 55 */
	{ 0x56, READ_OR_WRITE, 0, 1 },											/* code 56 */
	{ 0x57, READ_OR_WRITE, 0, 1 },											/* code 57 */
	{ 0x58, READ_OR_WRITE, 0, 1 },											/* code 58 */
	{ 0x59, READ_OR_WRITE, 0, 1 },											/* code 59 */
	{ 0x5A, READ_OR_WRITE, 0, 1 },											/* code 5A */
	{ 0x5B, READ_OR_WRITE, 0, 1 },											/* code 5B */
	{ 0x5C, READ_OR_WRITE, 0, 1 },											/* code 5C */
	{ 0x5D, READ_OR_WRITE, 0, 1 },											/* code 5D */
	{ 0x5E, READ_OR_WRITE, 0, 1 },											/* code 5E */
	{ 0x5F, READ_OR_WRITE, 0, 1 },											/* code 5F */
	{ 0x60, READ_OR_WRITE, 0, 1 },											/* code 60 */
	{ 0x61, READ_OR_WRITE, 0, 1 },											/* code 61 */
	{ 0x62, READ_OR_WRITE, 0, 1 },											/* code 62 */
	{ 0x63, READ_OR_WRITE, 0, 1 },											/* code 63 */
	{ 0x64, READ_OR_WRITE, 0, 1 },											/* code 64 */
	{ 0x65, READ_OR_WRITE, 0, 1 },											/* code 65 */
	{ 0x66, READ_OR_WRITE, 0, 1 },											/* code 66 */
	{ 0x67, READ_OR_WRITE, 0, 1 },											/* code 67 */
	{ 0x68, READ_OR_WRITE, 0, 1 },											/* code 68 */
	{ 0x69, READ_OR_WRITE, 0, 1 },											/* code 69 */
	{ 0x6A, READ_OR_WRITE, 0, 1 },											/* code 6A */
	{ 0x6B, READ_OR_WRITE, 0, 1 },											/* code 6B */
	{ 0x6C, READ_OR_WRITE, 0, 1 },											/* code 6C */
	{ 0x6D, READ_OR_WRITE, 0, 1 },											/* code 6D */
	{ 0x6E, READ_OR_WRITE, 0, 1 },											/* code 6E */
	{ 0x6F, READ_OR_WRITE, 0, 1 },											/* code 6F */
	{ 0x70, READ_OR_WRITE, 0, 1 },											/* code 70 */
	{ 0x71, READ_OR_WRITE, 0, 1 },											/* code 71 */
	{ 0x72, READ_OR_WRITE, 0, 1 },											/* code 72 */
	{ 0x73, READ_OR_WRITE, 0, 1 },											/* code 73 */
	{ 0x74, READ_OR_WRITE, 0, 1 },											/* code 74 */
	{ 0x75, READ_OR_WRITE, 0, 1 },											/* code 75 */
	{ 0x76, READ_OR_WRITE, 0, 1 },											/* code 76 */
	{ 0x77, READ_OR_WRITE, 0, 1 },											/* code 77 */
	{ 0x78, READ_OR_WRITE, 0, 1 },											/* code 78 */
	{ 0x79, READ_OR_WRITE, 0, 1 },											/* code 79 */
	{ 0x7A, READ_OR_WRITE, 0, 1 },											/* code 7A */
	{ 0x7B, READ_OR_WRITE, 0, 1 },											/* code 7B */
	{ 0x7C, READ_OR_WRITE, 0, 1 },											/* code 7C */
	{ 0x7D, READ_OR_WRITE, 0, 1 },											/* code 7D */
	{ 0x7E, READ_OR_WRITE, 0, 1 },											/* code 7E */
	{ 0x7F, READ_OR_WRITE, 0, 1 },											/* code 7F */
	{ 0x80, READ_OR_WRITE, 0, 1 },											/* code 80 */
	{ 0x81, READ_OR_WRITE, 0, 1 },											/* code 81 */
	{ 0x82, READ_OR_WRITE, 0, 1 },											/* code 82 */
	{ 0x83, READ_OR_WRITE, 0, 1 },											/* code 83 */
	{ 0x84, READ_OR_WRITE, 0, 1 },											/* code 84 */
	{ 0x85, READ_OR_WRITE, 0, 1 },											/* code 85 */
	{ 0x86, READ_OR_WRITE, 0, 1 },											/* code 86 */
	{ 0x87, READ_OR_WRITE, 0, 1 },											/* code 87 */
	{ 0x88, READ_OR_WRITE, 0, 1 },											/* code 88 */
	{ 0x89, READ_OR_WRITE, 0, 1 },											/* code 89 */
	{ 0x8A, READ_OR_WRITE, 0, 1 },											/* code 8A */
	{ 0x8B, READ_OR_WRITE, 0, 1 },											/* code 8B */
	{ 0x8C, READ_OR_WRITE, 0, 1 },											/* code 8C */
	{ 0x8D, READ_OR_WRITE, 0, 1 },											/* code 8D */
	{ 0x8E, READ_OR_WRITE, 0, 1 },											/* code 8E */
	{ 0x8F, READ_OR_WRITE, 0, 1 },											/* code 8F */
	{ 0x90, READ_OR_WRITE, 0, 1 },											/* code 90 */
	{ 0x91, READ_OR_WRITE, 0, 1 },											/* code 91 */
	{ 0x92, READ_OR_WRITE, 0, 1 },											/* code 92 */
	{ 0x93, READ_OR_WRITE, 0, 1 },											/* code 93 */
	{ 0x94, READ_OR_WRITE, 0, 1 },											/* code 94 */
	{ 0x95, READ_OR_WRITE, 0, 1 },											/* code 95 */
	{ 0x96, READ_OR_WRITE, 0, 1 },											/* code 96 */
	{ 0x97, READ_OR_WRITE, 0, 1 },											/* code 97 */
	{ 0x98, READ_OR_WRITE, 0, 1 },											/* code 98 */
	{ 0x99, READ_OR_WRITE, 0, 1 },											/* code 99 */
	{ 0x9A, READ_OR_WRITE, 0, 1 },											/* code 9A */
	{ 0x9B, READ_OR_WRITE, 0, 1 },											/* code 9B */
	{ 0x9C, READ_OR_WRITE, 0, 1 },											/* code 9C */
	{ 0x9D, READ_OR_WRITE, 0, 1 },											/* code 9D */
	{ 0x9E, READ_OR_WRITE, 0, 1 },											/* code 9E */
	{ 0x9F, READ_OR_WRITE, 0, 1 },											/* code 9F */
	{ 0xA0, READ_OR_WRITE, 0, 1 },											/* code A0 */
	{ 0xA1, READ_OR_WRITE, 0, 1 },											/* code A1 */
	{ 0xA2, READ_OR_WRITE, 0, 1 },											/* code A2 */
	{ 0xA3, READ_OR_WRITE, 0, 1 },											/* code A3 */
	{ 0xA4, READ_OR_WRITE, 0, 1 },											/* code A4 */
	{ 0xA5, READ_OR_WRITE, 0, 1 },											/* code A5 */
	{ 0xA6, READ_OR_WRITE, 0, 1 },											/* code A6 */
	{ 0xA7, READ_OR_WRITE, 0, 1 },											/* code A7 */
	{ 0xA8, READ_OR_WRITE, 0, 1 },											/* code A8 */
	{ 0xA9, READ_OR_WRITE, 0, 1 },											/* code A9 */
	{ 0xAA, READ_OR_WRITE, 0, 1 },											/* code AA */
	{ 0xAB, READ_OR_WRITE, 0, 1 },											/* code AB */
	{ 0xAC, READ_OR_WRITE, 0, 1 },											/* code AC */
	{ 0xAD, READ_OR_WRITE, 0, 1 },											/* code AD */
	{ 0xAE, READ_OR_WRITE, 0, 1 },											/* code AE */
	{ 0xAF, READ_OR_WRITE, 0, 1 },											/* code AF */
	{ 0xB0, READ_OR_WRITE, 0, 1 },											/* code B0 */
	{ 0xB1, READ_OR_WRITE, 0, 1 },											/* code B1 */
	{ 0xB2, READ_OR_WRITE, 0, 1 },											/* code B2 */
	{ 0xB3, READ_OR_WRITE, 0, 1 },											/* code B3 */
	{ 0xB4, READ_OR_WRITE, 0, 1 },											/* code B4 */
	{ 0xB5, READ_OR_WRITE, 0, 1 },											/* code B5 */
	{ 0xB6, READ_OR_WRITE, 0, 1 },											/* code B6 */
	{ 0xB7, READ_OR_WRITE, 0, 1 },											/* code B7 */
	{ 0xB8, READ_OR_WRITE, 0, 1 },											/* code B8 */
	{ 0xB9, READ_OR_WRITE, 0, 1 },											/* code B9 */
	{ 0xBA, READ_OR_WRITE, 0, 1 },											/* code BA */
	{ 0xBB, READ_OR_WRITE, 0, 1 },											/* code BB */
	{ 0xBC, READ_OR_WRITE, 0, 1 },											/* code BC */
	{ 0xBD, READ_OR_WRITE, 0, 1 },											/* code BD */
	{ 0xBE, READ_OR_WRITE, 0, 1 },											/* code BE */
	{ 0xBF, READ_OR_WRITE, 0, 1 },											/* code BF */
	{ 0xC0, READ_OR_WRITE, 0, 1 },											/* code C0 */
	{ 0xC1, READ_OR_WRITE, 0, 1 },											/* code C1 */
	{ 0xC2, READ_OR_WRITE, 0, 1 },											/* code C2 */
	{ 0xC3, READ_OR_WRITE, 0, 1 },											/* code C3 */
	{ 0xC4, READ_OR_WRITE, 0, 1 },											/* code C4 */
	{ 0xC5, READ_OR_WRITE, 0, 1 },											/* code C5 */
	{ 0xC6, READ_OR_WRITE, 0, 1 },											/* code C6 */
	{ 0xC7, READ_OR_WRITE, 0, 1 },											/* code C7 */
	{ 0xC8, READ_OR_WRITE, 0, 1 },											/* code C8 */
	{ 0xC9, READ_OR_WRITE, 0, 1 },											/* code C9 */
	{ 0xCA, READ_OR_WRITE, 0, 1 },											/* code CA */
	{ 0xCB, READ_OR_WRITE, 0, 1 },											/* code CB */
	{ 0xCC, READ_OR_WRITE, 0, 1 },											/* code CC */
	{ 0xCD, READ_OR_WRITE, 0, 1 },											/* code CD */
	{ 0xCE, READ_OR_WRITE, 0, 1 },											/* code CE */
	{ 0xCF, READ_OR_WRITE, 0, 1 },											/* code CF */
	{ 0xD0, READ_OR_WRITE, 0, 1 },											/* code D0 */
	{ 0xD1, READ_OR_WRITE, 0, 1 },											/* code D1 */
	{ 0xD2, READ_OR_WRITE, 0, 1 },											/* code D2 */
	{ 0xD3, READ_OR_WRITE, 0, 1 },											/* code D3 */
	{ 0xD4, READ_OR_WRITE, 0, 1 },											/* code D4 */
	{ 0xD5, READ_OR_WRITE, 0, 1 },											/* code D5 */
	{ 0xD6, READ_OR_WRITE, 0, 1 },											/* code D6 */
	{ 0xD7, READ_OR_WRITE, 0, 1 },											/* code D7 */
	{ 0xD8, READ_OR_WRITE, 0, 1 },											/* code D8 */
	{ 0xD9, READ_OR_WRITE, 0, 1 },											/* code D9 */
	{ 0xDA, READ_OR_WRITE, 0, 1 },											/* code DA */
	{ 0xDB, READ_OR_WRITE, 0, 1 },											/* code DB */
	{ 0xDC, READ_OR_WRITE, 0, 1 },											/* code DC */
	{ 0xDD, READ_OR_WRITE, 0, 1 },											/* code DD */
	{ 0xDE, READ_OR_WRITE, 0, 1 },											/* code DE */
	{ 0xDF, READ_OR_WRITE, 0, 1 },											/* code DF */
	{ 0xE0, READ_OR_WRITE, 0, 1 },											/* code E0 */
	{ 0xE1, READ_OR_WRITE, 0, 1 },											/* code E1 */
	{ 0xE2, READ_OR_WRITE, 0, 1 },											/* code E2 */
	{ 0xE3, READ_OR_WRITE, 0, 1 },											/* code E3 */
	{ 0xE4, READ_OR_WRITE, 0, 1 },											/* code E4 */
	{ 0xE5, READ_OR_WRITE, 0, 1 },											/* code E5 */
	{ 0xE6, READ_OR_WRITE, 0, 1 },											/* code E6 */
	{ 0xE7, READ_OR_WRITE, 0, 1 },											/* code E7 */
	{ 0xE8, READ_OR_WRITE, 0, 1 },											/* code E8 */
	{ 0xE9, READ_OR_WRITE, 0, 1 },											/* code E9 */
	{ 0xEA, READ_OR_WRITE, 0, 1 },											/* code EA */
	{ 0xEB, READ_OR_WRITE, 0, 1 },											/* code EB */
	{ 0xEC, READ_OR_WRITE, 0, 1 },											/* code EC */
	{ 0xED, READ_OR_WRITE, 0, 1 },											/* code ED */
	{ 0xEE, READ_OR_WRITE, 0, 1 },											/* code EE */
	{ 0xEF, READ_OR_WRITE, 0, 1 },											/* code EF */
	{ 0xF0, READ_OR_WRITE, 0, 1 },											/* code F0 */
	{ 0xF1, READ_OR_WRITE, 0, 1 },											/* code F1 */
	{ 0xF2, READ_OR_WRITE, 0, 1 },											/* code F2 */
	{ 0xF3, READ_OR_WRITE, 0, 1 },											/* code F3 */
	{ 0xF4, READ_OR_WRITE, 0, 1 },											/* code F4 */
	{ 0xF5, READ_OR_WRITE, 0, 1 },											/* code F5 */
	{ 0xF6, READ_OR_WRITE, 0, 1 },											/* code F6 */
	{ 0xF7, READ_OR_WRITE, 0, 1 },											/* code F7 */
	{ 0xF8, READ_OR_WRITE, 0, 1 },											/* code F8 */
	{ 0xF9, READ_OR_WRITE, 0, 1 },											/* code F9 */
	{ 0xFA, READ_OR_WRITE, 0, 1 },											/* code FA */
	{ 0xFB, READ_OR_WRITE, 0, 1 },											/* code FB */
	{ 0xFC, READ_OR_WRITE, 0, 1 },											/* code FC */
	{ 0xFD, READ_OR_WRITE, 0, 1 },											/* code FD */
	{ 0xFE, READ_OR_WRITE, 0, 1 },											/* code FE */
	{ 0xFF, READ_OR_WRITE, 0, 1 },											/* code FF */
};


/************************************
 * PRIVATE & STATIC VARIABLES
 ************************************/
SMBUS_HandleTypeDef    handle1;

uint8_t       *piobuf;
SMBUS_HandleTypeDef *phandle1;
SMBUS_StackHandleTypeDef *pcontext1;
SMBUS_StackHandleTypeDef context1;
uint32_t      index;




 uint32_t i2c1_smbus_lastTime;
/************************************
 * GLOBAL VARIABLES
 ************************************/

/************************************
 * PRIVATE&STATIC FUNCTION PROTOTYPES
 ************************************/

/************************************
 * PRIVATE & STATIC FUNCTIONS
 ************************************/
/**
  * @brief  Callback function notifying about extended command incoming, implementation
    is supporting extended command defined by PMBus
  * @param  pStackContext : Pointer to a SMBUS_StackHandleTypeDef structure that contains
  *                the configuration information for the specified SMBUS.
  * @retval HAL_StatusTypeDef response code. Equal STACK_OK if success, any other value means problem
  */
//HAL_StatusTypeDef STACK_SMBUS_ExtendCommand( SMBUS_StackHandleTypeDef *pStackContext )
//{
//  uint16_t       size = 0U;
//  uint8_t       *piobuf = NULL;
//
//  /* accessing the IO buffer */
//  piobuf = STACK_SMBUS_GetBuffer( pStackContext );
//
//  /*
//    Extended command must be identified by the value of the actual command code
//   */
//  switch ( piobuf[0] )
//  {
//    case 0:
//      pStackContext->CurrentCommand = (st_command_t *) & EXTENDED_READ_BYTE;
//      break;
//    case 1:
//      pStackContext->CurrentCommand = (st_command_t *) & EXTENDED_READ_WORD;
//      break;
//    case 2:
//      pStackContext->CurrentCommand = (st_command_t *) & EXTENDED_WRITE_BYTE;
//      /*
//        size of the bytes yet to be received
//       */
//      size = 1U;
//      break;
//    case 3:
//      pStackContext->CurrentCommand = (st_command_t *) & EXTENDED_WRITE_WORD;
//      size = 2U;
//      break;
//    default:
//      pStackContext->StateMachine |= SMBUS_SMS_ERROR;
//      return STACK_ERROR;
//  }
//
//  /*
//    reading the remaining data (stack won't do that for us this time
//  */
//  if ((pStackContext->CurrentCommand->cmnd_query & WRITE) == WRITE )
//  {
//    /*
//      To make sure the executecommand is called again once the remaining data
//      is in the buffer
//    */
//    STACK_PMBUS_ExtendExecution(pStackContext);
//
//    /*
//    PEC byte y/n?
//    */
//    if ((pStackContext->StateMachine & SMBUS_SMS_PEC_ACTIVE ) == SMBUS_SMS_PEC_ACTIVE )
//    {
//      size += PEC_SIZE;
//    }
//
//    /*
//      asking the HAL for more bytes to receive
//    */
//    pStackContext->Byte_count += size;
//    HAL_SMBUS_Slave_Receive_IT( pStackContext->Device, &(pStackContext->Buffer[2]), size, SMBUS_NEXT_FRAME );
//  }
//
//  return STACK_OK;
//}

/**
  * @brief  a version of the default SMBUS implementation with support for
  *       PMBus extended command
  * @param  pStackContext : Pointer to a SMBUS_StackHandleTypeDef structure that contains
  *                the configuration information for the specified SMBUS.
  * @retval None
  */
//void STACK_SMBUS_LocateCommand( SMBUS_StackHandleTypeDef *pStackContext )
//{
//  uint8_t       commandCode = pStackContext->Buffer[0];
//#ifdef DENSE_CMD_TBL
//  uint32_t      current, low, top;
//#endif
//
//#ifdef ARP
//  if ( pStackContext->StateMachine & SMBUS_SMS_ARP_AM )
//  {
//    STACK_SMBUS_LocateCommandARP( pStackContext, commandCode );
//  }
//  else
//#endif /* ARP treatment */
//
//    if (commandCode == PMBC_PMBUS_COMMAND_EXT)
//    {
//      /*
//        May not be exactly read byte, but the stack cannot know for sure
//       */
//      pStackContext->CurrentCommand = (st_command_t*)&EXTENDED_READ_BYTE;
//    }
//    else
//    {
//      /*
//        Code searching for command based on command code
//        */
//#ifdef DENSE_CMD_TBL
//
//      /*
//        initializing the command code search - the table must have all commands sorted, but there may be gaps
//       */
//      low = 0U;
//      top = pStackContext->CMD_tableSize - 1U;
//      pStackContext->CurrentCommand = NULL;
//
//      while ( top >= low )
//      {
//        /*
//          Pick interval half
//         */
//        current = ( low + top ) >> 1U;
//        if (pStackContext->CMD_table[current].cmnd_code == commandCode)
//        {
//          /*
//            we have found our command
//           */
//          pStackContext->CurrentCommand = &(pStackContext->CMD_table[current]);
//          return;
//        }
//        else if (pStackContext->CMD_table[current].cmnd_code < commandCode)
//        {
//          /*
//            Look at upper half
//          */
//          low = current + 1U;
//        }
//        else
//        {
//          top = current - 1U;
//        }
//      }
//#else
//      /*
//      Simple command table - command code equals the table index
//      */
//      pStackContext->CurrentCommand = &(pStackContext->CMD_table[commandCode]);
//
//      /* a sanity check */
//      if ( pStackContext->CurrentCommand->cmnd_code != commandCode )
//      {
//        pStackContext->CurrentCommand = NULL;
//      }
//#endif /* DENSE_CMD_TBL */
//    }
//}

/**
  * @brief  Callback function notifying slave about command incoming, implementation
    is supporting extended command defined by PMBus
  * @param  pStackContext : Pointer to a SMBUS_StackHandleTypeDef structure that contains
  *                the configuration information for the specified SMBUS.
  * @retval HAL_StatusTypeDef response code. Equal STACK_OK if success, any other value means problem
  */
HAL_StatusTypeDef STACK_SMBUS_ExecuteCommand( SMBUS_StackHandleTypeDef *pStackContext )
{
  uint8_t       *piobuf = NULL;

  /* accessing the IO buffer */
  piobuf = STACK_SMBUS_GetBuffer( pStackContext );

  if ( piobuf == NULL )
  {
    pStackContext->StateMachine |= SMBUS_SMS_ERROR;
  }
  else if ( pStackContext->CurrentCommand == (st_command_t *)&HOST_NOTIFY_PROTOCOL )
  {
    /* host notify command */
    if ( pStackContext->Buffer[0] == SMBUS_ADDR_DEFAULT )
    {
      /* Usually the Host notify is used for ARP, but may not be limited to it */
#ifdef ARP
#ifdef HOST1
      /* case of ARP notify */
      ARP_process = 1U;
#endif /* HOST1 */
#endif /* ARP */
    }
  }
  else    /* Normal device command execution */
  {
    /* Zone config command example implementation */
#ifdef PMBUS13
    if ( pStackContext->CurrentCommand->cmnd_code == PMBC_ZONE_CONFIG )
    {
      pStackContext->TheZone.writeZone = pStackContext->Buffer[1];
      pStackContext->TheZone.readZone = pStackContext->Buffer[2];
    }
    else if ( pStackContext->CurrentCommand->cmnd_code == PMBC_ZONE_ACTIVE )
    {
      pStackContext->TheZone.activeWriteZone = pStackContext->Buffer[1];
      pStackContext->TheZone.activeReadZone = pStackContext->Buffer[2];
    }

#endif /* PMBUS13 */
    /*
      first step is to see if we have a case of extended command
    */
//    if ( pStackContext->CurrentCommand->cmnd_code == PMBC_PMBUS_COMMAND_EXT )
//    {
////      BSP_LED_On(LED_GREEN);
//    }
//    else /* regular command case */
//    {
////      BSP_LED_On(LED_GREEN);
//      if ((pStackContext->CurrentCommand->cmnd_query & BLOCK ) == BLOCK )
//      {
//        *piobuf = (pStackContext->CurrentCommand->cmnd_master_Rx_size) - 1U;
//        /* byte size of reply for block read command */
//        /* One byte for size, rest are [size] of data */
//      }
//    }
  }
  return STACK_OK;
}

/**
  * @brief  Stub of an error treatment function - set to ignore most errors
  * @param  pStackContext : Pointer to a SMBUS_StackHandleTypeDef structure that contains
  *                the configuration information for the specified SMBUS.
  * @retval None
  */
static void Error_Check( SMBUS_StackHandleTypeDef *pStackContext)
{
  if ( ( STACK_SMBUS_IsBlockingError(pStackContext) ) || ( STACK_SMBUS_IsCmdError( pStackContext ) ) )
  {
    /* No action, error symptoms are ignored */
    pStackContext->StateMachine &= ~(SMBUS_ERROR_CRITICAL | SMBUS_COM_ERROR);
  }
  else if ((pStackContext->StateMachine & SMBUS_SMS_ERR_PECERR ) ==
           SMBUS_SMS_ERR_PECERR ) /* PEC error, we won't wait for any more action */
  {
    pStackContext->StateMachine |= SMBUS_SMS_READY;
    pStackContext->CurrentCommand = NULL;
    pStackContext->StateMachine &= ~(SMBUS_SMS_ACTIVE_MASK | SMBUS_SMS_ERR_PECERR);
  }
}


/************************************
 * GLOBAL FUNCTIONS
 ************************************/

/*!
 * @brief Initializer of i2c1_smbus.
 *
 * @param none
 *
 * @return none.
 *
 * This function must be call in main()
 */
void i2c1_smbus_init(void)
{
	/* SMBUS instance initialization - smbus 1. */
  handle1.Instance = SMBUS1;
  handle1.Init.Timing = SMBUS_TIMING_100K;
  handle1.Init.AnalogFilter = SMBUS_ANALOGFILTER_ENABLE;
  handle1.Init.AddressingMode = SMBUS_ADDRESSINGMODE_7BIT;
#ifdef PMBUS13
  handle1.Init.DualAddressMode = SMBUS_DUALADDRESS_ENABLE;
  handle1.Init.OwnAddress2 = 0x40U;
  handle1.Init.OwnAddress2Masks = SMBUS_OA2_MASK05;
#else /* PMBUS13 */
  handle1.Init.DualAddressMode = SMBUS_DUALADDRESS_DISABLE;
  handle1.Init.OwnAddress2 = 0U;
  handle1.Init.OwnAddress2Masks = SMBUS_OA2_NOMASK;
#endif /* PMBUS13 */
  handle1.Init.GeneralCallMode = SMBUS_GENERALCALL_DISABLE;
  handle1.Init.NoStretchMode = SMBUS_NOSTRETCH_DISABLE;
#ifdef ARP
#ifdef DEV_PSA
  handle1.Init.OwnAddress1 = SMBUS_ADDR_DEVICE;
#else /* DEV_PSA */
  handle1.Init.OwnAddress1 = 0U;
#endif /* DEV_PSA */
  handle1.Init.PeripheralMode = SMBUS_PERIPHERAL_MODE_SMBUS_SLAVE_ARP;
#else      /* ARP */
  handle1.Init.OwnAddress1 = SMBUS_ADDR_DEVICE;
  handle1.Init.PeripheralMode = SMBUS_PERIPHERAL_MODE_SMBUS_SLAVE;
#endif /* ARP */
#ifdef USE_PEC
  handle1.Init.PacketErrorCheckMode = SMBUS_PEC_ENABLE;
#else
  handle1.Init.PacketErrorCheckMode = SMBUS_PEC_DISABLE;
#endif /* USE_PEC */
#ifdef HOST1
  handle1.Init.PeripheralMode = SMBUS_PERIPHERAL_MODE_SMBUS_HOST;
  handle1.Init.OwnAddress1 = 0U;
#endif /* HOST1 */
  handle1.Init.SMBusTimeout = SMBUS_TIMEOUT_DEFAULT;
  handle1.pBuffPtr = context1.Buffer;

  phandle1 = &handle1;

  HAL_SMBUS_Init( phandle1 );

  context1.CMD_table = (st_command_t *) & EDID_COMMANDS_TABLE[0];
  context1.CMD_tableSize = EDID_COMMANDS_TABBLE_SIZE;
//#ifndef TEST4
//#ifndef TEST5
//  /* Most tests do not use actual PMBUS commands */
//  context1.CMD_table = (st_command_t *) & PMBUS_COMMANDS_TEST[0];
//  context1.CMD_tableSize = PMBUS_CMD_TBL_SIZE;
//#endif /* TEST4 */
//#endif /* TEST5 */
  context1.Device = phandle1;
  //ToDo: check the preset byte
  context1.SRByte = 0x55U;
  context1.CurrentCommand = NULL;
#ifdef ARP
  context1.StateMachine = SMBUS_SMS_NONE;
#ifdef DEV_PSA
  context1.OwnAddress = SMBUS_ADDR_DEVICE;
#else /* DEV_PSA */
  context1.OwnAddress = 0U;
#endif /* DEV_PSA */
#ifndef  HOST1
  context1.ARP_UDID = (uint8_t *) &UDID;
#endif /* HOST1 */
#else /* ARP */
  context1.StateMachine = SMBUS_SMS_ARP_AR;
  context1.OwnAddress = SMBUS_ADDR_DEVICE;
#endif /* ARP */
#ifdef USE_PEC
  context1.StateMachine |= SMBUS_SMS_PEC_ACTIVE;
#endif
  pcontext1 = &context1;

  STACK_SMBUS_Init( pcontext1 );

  /*Read buffer 1st time*/

  piobuf = STACK_SMBUS_GetBuffer( pcontext1 );
  if (piobuf != NULL )
  {
    for (index = 0U; index < STACK_NBYTE_SIZE; index++)
    {
      piobuf[index] = (uint8_t)index;
    }
  }




  i2c1_smbus_lastTime = 0;
}


/*!
 * @brief Periodic task of i2c1_smbus.
 *
 * @param none
 *
 * @return none.
 *
 * This function must be call in loop of main()
 */
void i2c1_smbus_task(void)
{
	if (HAL_GetTick() - i2c1_smbus_lastTime > 200)
	{
	  i2c1_smbus_lastTime = HAL_GetTick();



	  /* Periodically checking error state of the stack */
	  	Error_Check( &context1 );

	  	/* Periodically checking for Quick command */
	  	if ( context1.StateMachine & ( SMBUS_SMS_QUICK_CMD_W | SMBUS_SMS_QUICK_CMD_R ) )
	  	{
	  		/* Todo: Indicate status */
//	  	  	BSP_LED_On(LED_GREEN);
	  		context1.StateMachine &= ~( SMBUS_SMS_QUICK_CMD_W | SMBUS_SMS_QUICK_CMD_R );
	  	}

	  	/* Todo: Indicate status */
//	  	BSP_LED_Off(LED_GREEN);

	  	/* Cleaning arbitration loss flag */
	  	context1.StateMachine &= ~SMBUS_SMS_ERR_ARLO;

//	  	if (buttonpress == 1U)
//	  	{
//#ifdef ALERT
//	  	  STACK_SMBUS_SendAlert( pcontext1 );
//#else /* ALERT */
//	  	  STACK_SMBUS_NotifyHost( pcontext1 );
//#endif /* DEV_ALERT */
//	  	  buttonpress = 0U;
//	  	}
	}

}

/*** end of file ***/
