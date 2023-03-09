// #include "led.h"
// #include "main.h"

// #define LED_STATE_OFF 	0
// #define LED_STATE_ON 		1
// #define LED_STATE_BLINK	2

// #define LED_COLOR_RED	0
// #define LED_COLOR_GREEN	1
// #define LED_COLOR_BLUE	2

// uint8_t ledValue[4];
// uint8_t Value[4] = { 0, 0, 0, 0 };
// union ledColor {
// 	uint16_t value;
// 	struct {
// 		//		uint8_t led1:3;
// 		//		uint8_t led2:3;
// 		//		uint8_t led3:3;
// 		//		uint8_t led4:3;
// 		//		uint8_t led5:3;
// 		//		uint8_t ledReserve:1;
// 		uint8_t ledReserve :1;
// 		uint8_t led5 :3;
// 		uint8_t led4 :3;
// 		uint8_t led3 :3;
// 		uint8_t led2 :3;
// 		uint8_t led1 :3;
// 	} v;
// } ledColor;
// union ledColor ledBuffer;

// void led_init() {
// 	//all blank
// }

// void led_set_state(uint8_t ledIndex, uint8_t ledState, uint8_t ledColor) {

// }

// void led_fsm() {

// }

// void led_on() {
// 	HAL_GPIO_WritePin(LED_SHDN_GPIO_Port, LED_SHDN_Pin, 1);
// }

// void led_off() {
// 	HAL_GPIO_WritePin(LED_SHDN_GPIO_Port, LED_SHDN_Pin, 0);
// }

// void  led_blink(uint8_t index) {
// 	switch (index) {
// 	case 0:
// 		ledBuffer.v.led1 += 1;
// 		break;

// 	case 1:
// 		ledBuffer.v.led2 += 1;
// 		break;

// 	case 2:
// 		ledBuffer.v.led3 += 1;
// 		break;

// 	case 3:
// 		ledBuffer.v.led4 += 1;
// 		break;

// 	}
// 	led_display();
// }

// void led_display_i2c(){
// //HAL_I2C
// }

// //void led_display_spi() {
// //	//	ledBuffer.value = 0b0000000000001001;//0b0000111100010001;
// //	ledBuffer.v.led1 = 1;
// //	ledBuffer.v.led2 = 1;
// //	ledBuffer.v.led3 = 1;
// //	ledBuffer.v.led4 = 1;
// //	ledBuffer.v.led5 = 1;
// //	HAL_GPIO_WritePin(LED_TLC_BLANK_GPIO_Port, LED_TLC_BLANK_Pin, 0);
// //	HAL_Delay(100);
// ////	HAL_GPIO_WritePin(LED_TLC_LATCH_GPIO_Port, LED_TLC_LATCH_Pin, 1);
// ////	HAL_Delay(100);
// //	HAL_GPIO_WritePin(LED_TLC_LATCH_GPIO_Port, LED_TLC_LATCH_Pin, 0);
// //	HAL_Delay(100);
// //	HAL_SPI_Transmit(&hspi1, /*ledBuffer.value*/0b1111111111111010, 2, 1000);
// //	HAL_Delay(100);
// //	HAL_GPIO_WritePin(LED_TLC_LATCH_GPIO_Port, LED_TLC_LATCH_Pin, 1);
// //}


// void led_display(){
// 	led_display_i2c();
// }
