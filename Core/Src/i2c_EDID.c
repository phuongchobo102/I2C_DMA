#include "stdio.h"
#include "main.h"
#include "i2c_EDID.h"

extern I2C_HandleTypeDef hi2c1;
uint8_t aRxBuffer[RXBUFFERSIZE];
uint8_t aTxBuffer[RXBUFFERSIZE];

void reponse_50_msg(void)
{
  /* The board receives the message and sends it back */

  /*##-2- Put I2C peripheral in reception process ###########################*/  
  if(HAL_I2C_Slave_Receive_IT(&hi2c1, (uint8_t *)aRxBuffer, RXBUFFERSIZE) != HAL_OK)
  {
    /* Transfer error in reception process */
    printf("HAL_I2C_Slave_Receive_IT is error \r\n");
    Error_Handler();
  }
  
  /*##-3- Wait for the end of the transfer ###################################*/  
  /*  Before starting a new communication transfer, you need to check the current   
      state of the peripheral; if it’s busy you need to wait for the end of current
      transfer before starting a new one.
      For simplicity reasons, this example is just waiting till the end of the
      transfer, but application may perform other tasks while transfer operation
      is ongoing. */
  while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
  {
    
  }
  
  /*##-4- Start the transmission process #####################################*/  
  /* While the I2C in reception process, user can transmit data through 
     "aTxBuffer" buffer */
  if(HAL_I2C_Slave_Transmit_IT(&hi2c1, (uint8_t*)aTxBuffer, TXBUFFERSIZE)!= HAL_OK)
  {
    /* Transfer error in transmission process */
    printf("HAL_I2C_Slave_Transmit_IT is error \r\n");
    Error_Handler();    
  }
  
  /*##-5- Wait for the end of the transfer ###################################*/  
  /*  Before starting a new communication transfer, you need to check the current   
      state of the peripheral; if it’s busy you need to wait for the end of current
      transfer before starting a new one.
      For simplicity reasons, this example is just waiting till the end of the
      transfer, but application may perform other tasks while transfer operation
      is ongoing. */
  while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
  {
    
  } 
  
}
