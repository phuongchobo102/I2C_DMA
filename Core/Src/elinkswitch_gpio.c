/**
********************************************************************************
* @file    elinkswitch_gpio.c
* @author  Phu Tran, Minh La, An Le, Vinh Pham
* @date    Jan 2023
* @brief   
********************************************************************************
*/

/************************************
* INCLUDES
************************************/
#include "elinkswitch_gpio.h"
#include "stm32f0xx_ll_bus.h"
#include "stm32f0xx_ll_gpio.h"
#include "main.h"
#include "lwbtn.h"

#include "elinkswitch.h"

#include <stdio.h>

/************************************
* PRIVATE MACROS AND DEFINES
************************************/

/************************************
* PRIVATE TYPEDEFS
************************************/

/************************************
* INCLUDED EXTERN VARIABLES DEFINITION
************************************/

/************************************
* STATIC VARIABLES
************************************/
static uint32_t stick = 0;
static uint32_t time_last;

struct lwbtn elsgpio_lwbtn;
/* User defined settings */
const uint8_t keys[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8'/*, '9'*/,'a','b','c','d'};
uint32_t last_time_keys[sizeof(keys) / sizeof(keys[0])] = {0};

uint16_t adcBuffer[4];
/* List of buttons to process with assigned custom arguments for callback functions */
static lwbtn_btn_t btns[] = {
  {.arg = (void*)&keys[0]}, {.arg = (void*)&keys[1]}, {.arg = (void*)&keys[2]}, {.arg = (void*)&keys[3]},
  {.arg = (void*)&keys[4]}, {.arg = (void*)&keys[5]}, {.arg = (void*)&keys[6]}, {.arg = (void*)&keys[7]} ,
  {.arg = (void*)&keys[8]}/*, {.arg = (void*)&keys[9] } */,
  {.arg = (void*)&keys[9]}, {.arg = (void*)&keys[10]}, {.arg = (void*)&keys[11]}, {.arg = (void*)&keys[12]}
};

elinkswitch_button_event elinkswitch_receive_button_event;
/************************************
* GLOBAL VARIABLES
************************************/

ADC_ChannelConfTypeDef sConfig = {0};
/************************************
* STATIC FUNCTION PROTOTYPES
************************************/

uint32_t get_tick(void);
uint8_t prv_btn_get_state(struct lwbtn* lw, struct lwbtn_btn* btn);
void prv_btn_event(struct lwbtn* lw, struct lwbtn_btn* btn, lwbtn_evt_t evt);

void lc_elsgpio_elinkswitch_state_change_event(elinkswitch_state_e new_state);
/************************************
* STATIC FUNCTIONS
************************************/
/**
* \brief           Get input state callback 
* \param           lw: LwBTN instance
* \param           btn: Button instance
*/
uint32_t get_tick(void)
{
  return stick;
}


//uint16_t read_adc_usb(uint8_t channel){
//  uint16_t adcValue = 0;
//  if (channel == 1) sConfig.Channel = ADC_CHANNEL_1;
//  else if( channel ==3 ) sConfig.Channel = ADC_CHANNEL_3;  
//  else if( channel ==4 ) sConfig.Channel = ADC_CHANNEL_4;
//  else if( channel ==6 ) sConfig.Channel = ADC_CHANNEL_6;
//  else return 0;
////  sConfig.Rank = channel;
//  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
//  {
//    Error_Handler(); 
//  }
//  
//  HAL_ADC_Start(&hadc);
//  HAL_ADC_PollForConversion(&hadc, HAL_MAX_DELAY);
//  adcValue = HAL_ADC_GetValue(&hadc);
//  HAL_ADC_Stop(&hadc);
////  sConfig.Rank = 0;
////  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
////  {
////    Error_Handler();
////  }
//  return adcValue;
//}
/**
* \brief           Get input state callback 
* \param           lw: LwBTN instance
* \param           btn: Button instance
* \return          `1` if button active, `0` otherwise
*/
uint8_t prv_btn_get_state(struct lwbtn* lw, struct lwbtn_btn* btn) {
  uint8_t result = 0;
  uint16_t rawADC;
  (void)lw;
  
  /*
  * Function will return negative number if button is pressed,
  * or zero if button is releases
  */
  if(btn)
  {
    uint8_t u8_temp = *((uint8_t*)(btn->arg));
    if(u8_temp == '0')
    {
      if(!LL_GPIO_IsInputPinSet(ELS_BUTTON_EDID_GPIO_PORT, ELS_BUTTON_EDID_PIN))
      {
        result = 1;
      }else
      {
        result = 0;
      }
    }else if(u8_temp == '1')
    {
      if(!LL_GPIO_IsInputPinSet(ELS_BUTTON1_GPIO_PORT, ELS_BUTTON1_PIN))
      {
        result = 1;
      }else
      {
        result = 0;
      }
    }else if(u8_temp == '2')
    {
      if(!LL_GPIO_IsInputPinSet(ELS_BUTTON2_GPIO_PORT, ELS_BUTTON2_PIN))
      {
        result = 1;
      }else
      {
        result = 0;
      }
    }else if(u8_temp == '3')
    {
      if(!LL_GPIO_IsInputPinSet(ELS_BUTTON3_GPIO_PORT, ELS_BUTTON3_PIN))
      {
        result = 1;
      }else
      {
        result = 0;
      }
    }else if(u8_temp == '4')
    {
      if(!LL_GPIO_IsInputPinSet(ELS_BUTTON4_GPIO_PORT, ELS_BUTTON4_PIN))
      {
        result = 1;
      }else
      {
        result = 0;
      }
    }else if(u8_temp == '5')
    {
      if(!LL_GPIO_IsInputPinSet(VGA1_PLUG_DT_GPIO_Port, VGA1_PLUG_DT_Pin))
      {
        result = 1;
      }else
      {
        result = 0;
      }
    }else if(u8_temp == '6')
    {
      if(!LL_GPIO_IsInputPinSet(VGA2_PLUG_DT_GPIO_Port, VGA2_PLUG_DT_Pin))
      {
        result = 1;
      }else
      {
        result = 0;
      }
    }else if(u8_temp == '7')
    {
      if(!LL_GPIO_IsInputPinSet(VGA3_PLUG_DT_GPIO_Port, VGA3_PLUG_DT_Pin))
      {
        result = 1;
      }else
      {
        result = 0;
      }
    }else if(u8_temp == '8')
    {
      if(!LL_GPIO_IsInputPinSet(VGA4_PLUG_DT_GPIO_Port, VGA4_PLUG_DT_Pin))
      {
        result = 1;
      }else
      {
        result = 0;
      }
    }
    else if(u8_temp == 'a')
    {    
      if( adcValue[0] > 500)
      {
        result = 1;
      }else
      {
        result = 0;
      }
    }
    else if(u8_temp == 'b')
    {
      if( adcValue[1] >500)
      {
        result = 1;
      }else
      {
        result = 0;
      }
    }else if(u8_temp == 'c')
    {
      if( adcValue[2] > 500)
      {
        result = 1;
      }else
      {
        result = 0;
      }
    }else if(u8_temp == 'd')
    {
      if( adcValue[3] > 500)
      {
        result = 1;
      }else
      {
        result = 0;
      }
    }
  }
  return result;
}

/**
* \brief           Button event
* 
* \param           lw: LwBTN instance
* \param           btn: Button instance
* \param           evt: Button event
*/
void prv_btn_event(struct lwbtn* lw, struct lwbtn_btn* btn, lwbtn_evt_t evt) {
  /*const*/ char* s = NULL;
  uint32_t keepalive_cnt = 0;
  uint32_t* diff_time_ptr = &last_time_keys[(*(uint8_t*)btn->arg) - '0'];
  uint32_t diff_time = get_tick() - *diff_time_ptr;
  char currentButton; 
  
  /* This is for purpose of test and timing validation */
  if (diff_time > 2000) {
    diff_time = 0;
  }
  *diff_time_ptr = get_tick(); /* Set current date as last one */
  
  /* Get event string */
  if (0) {
#if LWBTN_CFG_USE_KEEPALIVE
  } else if (evt == LWBTN_EVT_KEEPALIVE) {
    //        s = "KEEPALIVE";
#endif /* LWBTN_CFG_USE_KEEPALIVE */
  } else if (evt == LWBTN_EVT_ONPRESS) {
    s = "  ONPRESS";
  } else if (evt == LWBTN_EVT_ONRELEASE) {
    s = "ONRELEASE";
  } else if (evt == LWBTN_EVT_ONCLICK) {
    s = "  ONCLICK";
  } else {
    s = "  UNKNOWN";
  }
#if LWBTN_CFG_USE_KEEPALIVE
  keepalive_cnt = btn->keepalive.cnt;
#endif
  //SetConsoleTextAttribute(hConsole, color);
  if(s == "ONRELEASE" )
  {
    currentButton = *(uint8_t*)btn->arg;
    switch(currentButton){
    case '1'...'4': //button
      printf("Button %c Pressed \r\n", currentButton);
      currentButton -= '1' ;  //convert to 0...3
      channelSelect =  currentButton;
      if(  elinkswitch_receive_button_event) {
        elinkswitch_receive_button_event(ELINKSWITCH_BUTTON1_ON_RELEASE + currentButton);
      }
      break;
    case '5'...'8': //vga detect
      currentButton -= '5' ;
      printf("VGA detect OUT %d\r\n", currentButton);   
      vgaStatus[currentButton] = 0;
      break;
    case 'a'...'d': //usb detect
      currentButton -= 'a';
      printf("USB detect OUT %d\r\n", currentButton); 
      usbStatus[currentButton] = 0;
      break;
    case '0': //button EDID
      edidStatus = (edidStatus + 1) % 2;
      printf("Button EDID Pressed %d\r\n", edidStatus);
      if(  elinkswitch_receive_button_event) {
        elinkswitch_receive_button_event(ELINKSWITCH_BUTTON5_ON_RELEASE );
      }
      //TODO: EDID bahavior
      break;
    }
  }
  if(s == "  ONPRESS" ){
    currentButton = *(uint8_t*)btn->arg;
    switch(currentButton){
    case 'a'...'d': //usb detect
      currentButton -= 'a';
      printf("USB detect plug IN %d\r\n", currentButton); 
      usbStatus[currentButton] = 1;
      break;
      case '5'...'8': //vga detect
      currentButton -= '5' ;
      printf("VGA detect IN %d\r\n", currentButton);    
      vgaStatus[currentButton] = 1;
      break;
    default: 
      break;
//    case '0': //button EDID
//      printf("Button EDID Press\r\n");
//      //TODO: EDID bahavior
//      break;
    }
  }
  (void)lw;
}


void lc_elsgpio_elinkswitch_state_change_event(elinkswitch_state_e new_state)
{
	//ToDo: Handle state change here
	printf("\r\n Change to state=%d \r\n",new_state);
}
/************************************
* GLOBAL FUNCTIONS
************************************/
/*!
* @brief initialize gpio for elinkswitch
*
* @param[in] none
*
* @return true if success, otherwise else
*/
bool elsgpio_init(void)
{
  //LL_GPIO_InitTypeDef gpio_initstruct;
  
//  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
//  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
//  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
//  
//  LL_GPIO_SetPinMode(ELS_USB_SENSE1_GPIO_PORT, ELS_USB_SENSE1_PIN, LL_GPIO_MODE_INPUT);
//  LL_GPIO_SetPinPull(ELS_USB_SENSE1_GPIO_PORT, ELS_USB_SENSE1_PIN, LL_GPIO_PULL_NO);
//  LL_GPIO_SetPinSpeed(ELS_USB_SENSE1_GPIO_PORT, ELS_USB_SENSE1_PIN, LL_GPIO_SPEED_FREQ_HIGH);
//  LL_GPIO_SetPinMode(ELS_USB_SENSE2_GPIO_PORT, ELS_USB_SENSE2_PIN, LL_GPIO_MODE_INPUT);
//  LL_GPIO_SetPinPull(ELS_USB_SENSE2_GPIO_PORT, ELS_USB_SENSE2_PIN, LL_GPIO_PULL_NO);
//  LL_GPIO_SetPinSpeed(ELS_USB_SENSE2_GPIO_PORT, ELS_USB_SENSE2_PIN, LL_GPIO_SPEED_FREQ_HIGH);
//  LL_GPIO_SetPinMode(ELS_USB_SENSE3_GPIO_PORT, ELS_USB_SENSE3_PIN, LL_GPIO_MODE_INPUT);
//  LL_GPIO_SetPinPull(ELS_USB_SENSE3_GPIO_PORT, ELS_USB_SENSE3_PIN, LL_GPIO_PULL_NO);
//  LL_GPIO_SetPinSpeed(ELS_USB_SENSE3_GPIO_PORT, ELS_USB_SENSE3_PIN, LL_GPIO_SPEED_FREQ_HIGH);
//  LL_GPIO_SetPinMode(ELS_USB_SENSE4_GPIO_PORT, ELS_USB_SENSE4_PIN, LL_GPIO_MODE_INPUT);
//  LL_GPIO_SetPinPull(ELS_USB_SENSE4_GPIO_PORT, ELS_USB_SENSE4_PIN, LL_GPIO_PULL_NO);
//  LL_GPIO_SetPinSpeed(ELS_USB_SENSE4_GPIO_PORT, ELS_USB_SENSE4_PIN, LL_GPIO_SPEED_FREQ_HIGH);
//  
//  LL_GPIO_SetPinMode(ELS_BUTTON1_GPIO_PORT, ELS_BUTTON1_PIN, LL_GPIO_MODE_INPUT);
//  LL_GPIO_SetPinPull(ELS_BUTTON1_GPIO_PORT, ELS_BUTTON1_PIN, LL_GPIO_PULL_NO);
//  LL_GPIO_SetPinSpeed(ELS_BUTTON1_GPIO_PORT, ELS_BUTTON1_PIN, LL_GPIO_SPEED_FREQ_HIGH);
//  LL_GPIO_SetPinMode(ELS_BUTTON2_GPIO_PORT, ELS_BUTTON2_PIN, LL_GPIO_MODE_INPUT);
//  LL_GPIO_SetPinPull(ELS_BUTTON2_GPIO_PORT, ELS_BUTTON2_PIN, LL_GPIO_PULL_NO);
//  LL_GPIO_SetPinSpeed(ELS_BUTTON2_GPIO_PORT, ELS_BUTTON2_PIN, LL_GPIO_SPEED_FREQ_HIGH);
//  LL_GPIO_SetPinMode(ELS_BUTTON3_GPIO_PORT, ELS_BUTTON3_PIN, LL_GPIO_MODE_INPUT);
//  LL_GPIO_SetPinPull(ELS_BUTTON3_GPIO_PORT, ELS_BUTTON3_PIN, LL_GPIO_PULL_NO);
//  LL_GPIO_SetPinSpeed(ELS_BUTTON3_GPIO_PORT, ELS_BUTTON3_PIN, LL_GPIO_SPEED_FREQ_HIGH);
//  LL_GPIO_SetPinMode(ELS_BUTTON4_GPIO_PORT, ELS_BUTTON4_PIN, LL_GPIO_MODE_INPUT);
//  LL_GPIO_SetPinPull(ELS_BUTTON4_GPIO_PORT, ELS_BUTTON4_PIN, LL_GPIO_PULL_NO);
//  LL_GPIO_SetPinSpeed(ELS_BUTTON4_GPIO_PORT, ELS_BUTTON4_PIN, LL_GPIO_SPEED_FREQ_HIGH);
  

	/* */
	elinkswitch_state_change_event_listener_register(lc_elsgpio_elinkswitch_state_change_event);
  /* lwbt*/
  
  elsgpio_lwbtn.btns = btns;
  elsgpio_lwbtn.btns_cnt = sizeof(keys) / sizeof(keys[0]);
  elsgpio_lwbtn.evt_fn = prv_btn_event;
#if LWBTN_CFG_GET_STATE_MODE != LWBTN_GET_STATE_MODE_MANUAL
  elsgpio_lwbtn.get_state_fn = prv_btn_get_state;
#endif /* LWBTN_CFG_GET_STATE_MODE != LWBTN_GET_STATE_MODE_MANUAL */
  
  /* Define buttons */
  lwbtn_init_ex(&elsgpio_lwbtn, btns, sizeof(btns) / sizeof(btns[0]), prv_btn_get_state, prv_btn_event);
  time_last = get_tick();
  return true;
}

bool elsgpio_register_button_event_listener(elinkswitch_button_event listener)
{
	elinkswitch_receive_button_event = listener;
	return true;
}

/*!
* @brief regular task which runs all stuff relate to gpio
*
* @param[in] none
*
* @return The value of the larger number.
*/
void elsgpio_task(void)
{
  /* Process forever */
  lwbtn_process_ex(/*NULL*/&elsgpio_lwbtn, get_tick());
  
  /* Manually read button state */
#if LWBTN_CFG_GET_STATE_MODE == LWBTN_GET_STATE_MODE_MANUAL
  for (size_t i = 0; i < sizeof(btns) / sizeof(btns[0]); ++i) {
    lwbtn_set_btn_state(&btns[i], prv_btn_get_state(/*NULL*/&elsgpio_lwbtn, &btns[i]));
  }
#endif /* LWBTN_CFG_GET_STATE_MODE == LWBTN_GET_STATE_MODE_MANUAL */
  
  /* Check if specific button is active and do some action */
  //  for(size_t i = 0; i < sizeof(btns) / sizeof(btns[0]); ++i) 
  //  { 
  //    if (lwbtn_is_btn_active(&btns[i])) {
  //        if ((get_tick() - time_last) > 200) {
  //            time_last = get_tick();
  //            printf("Button[%d] is active\r\n",i);
  //        }
  //    }
  //  }
}

/*!
* @brief call when 1ms system elapsed
*
* @param[in] none
*
* @return none
*/
void elsgpio_1ms_elapsed(void)
{
  static uint16_t counter = 0;
  static uint8_t seconds = 0;
  static uint8_t minutes = 0;
  counter++;
  stick++;
  if(counter > 999)
  {
    counter = 0;
    seconds++;
    if(seconds > 59)
    {
      seconds = 0;
      minutes++;
      if(minutes > 59)
      {
        minutes = 0;
      }
    }
    //    printf("\n\r minutes--seconds: %d--%d \n\r ", minutes, seconds);
  }
}
