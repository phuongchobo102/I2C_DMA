#ifndef __SYSTEM_SWITCH_H__
#define __SYSTEM_SWITCH_H__
#include <stdint.h>

void system_switch(uint8_t channel);
void system_enable();
void system_disable();
void system_switch_init();
void system_switch_tasks();
#endif