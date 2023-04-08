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
/**
  * @brief  Callback function notifying about extended command incoming, implementation
    is supporting extended command defined by PMBus
  * @param  pStackContext : Pointer to a SMBUS_StackHandleTypeDef structure that contains
  *                the configuration information for the specified SMBUS.
  * @retval HAL_StatusTypeDef response code. Equal STACK_OK if success, any other value means problem
  */
HAL_StatusTypeDef STACK_SMBUS_ExtendCommand( SMBUS_StackHandleTypeDef *pStackContext )
{
  uint16_t       size = 0U;
  uint8_t       *piobuf = NULL;

  /* accessing the IO buffer */
  piobuf = STACK_SMBUS_GetBuffer( pStackContext );

  /*
    Extended command must be identified by the value of the actual command code
   */
  switch ( piobuf[0] )
  {
    case 0:
      pStackContext->CurrentCommand = (st_command_t *) & EXTENDED_READ_BYTE;
      break;
    case 1:
      pStackContext->CurrentCommand = (st_command_t *) & EXTENDED_READ_WORD;
      break;
    case 2:
      pStackContext->CurrentCommand = (st_command_t *) & EXTENDED_WRITE_BYTE;
      /*
        size of the bytes yet to be received
       */
      size = 1U;
      break;
    case 3:
      pStackContext->CurrentCommand = (st_command_t *) & EXTENDED_WRITE_WORD;
      size = 2U;
      break;
    default:
      pStackContext->StateMachine |= SMBUS_SMS_ERROR;
      return STACK_ERROR;
  }

  /*
    reading the remaining data (stack won't do that for us this time
  */
  if ((pStackContext->CurrentCommand->cmnd_query & WRITE) == WRITE )
  {
    /*
      To make sure the executecommand is called again once the remaining data
      is in the buffer
    */
    STACK_PMBUS_ExtendExecution(pStackContext);

    /*
    PEC byte y/n?
    */
    if ((pStackContext->StateMachine & SMBUS_SMS_PEC_ACTIVE ) == SMBUS_SMS_PEC_ACTIVE )
    {
      size += PEC_SIZE;
    }

    /*
      asking the HAL for more bytes to receive
    */
    pStackContext->Byte_count += size;
    HAL_SMBUS_Slave_Receive_IT( pStackContext->Device, &(pStackContext->Buffer[2]), size, SMBUS_NEXT_FRAME );
  }

  return STACK_OK;
}

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
    if ( pStackContext->CurrentCommand->cmnd_code == PMBC_PMBUS_COMMAND_EXT )
    {
//      BSP_LED_On(LED_GREEN);
    }
    else /* regular command case */
    {
//      BSP_LED_On(LED_GREEN);
      if ((pStackContext->CurrentCommand->cmnd_query & BLOCK ) == BLOCK )
      {
        *piobuf = (pStackContext->CurrentCommand->cmnd_master_Rx_size) - 1U;
        /* byte size of reply for block read command */
        /* One byte for size, rest are [size] of data */
      }
    }
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
 * @brief Identify the larger of two 8-bit integers.
 *
 * @param[in] num1  The first number to be compared.
 * @param[in] num2  The second number to be compared.
 *
 * @return The value of the larger number.
 *
 * This function must be run before crc_fast() or the table stored in ROM.
 */
void i2c1_smbus_init(void)
{

}

/*** end of file ***/
