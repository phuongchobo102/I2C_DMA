#include "system_switch.h"
#include "main.h"

uint32_t lastTimeSystemSwitchTask;
uint8_t lastChannelSelectSW=10;
/**
 * @brief switch channel
 * @param
 * @retval None
 */
void system_switch(uint8_t channel)
{
    if (channel == 0)
    {
        system_disable();
    }
    else if (channel > 0 && channel < 5)
    {
        channel--;
        HAL_GPIO_WritePin(USB_SW_SEL0_GPIO_Port, USB_SW_SEL0_Pin, (channelSelect % 2));
        HAL_GPIO_WritePin(USB_SW_SEL1_GPIO_Port, USB_SW_SEL1_Pin, ((channelSelect / 2) % 2));
        system_enable();
    }
}

void system_enable()
{
    HAL_GPIO_WritePin(KVMSW_EN_GPIO_Port, KVMSW_EN_Pin, 1);
}
void system_disable()
{
    HAL_GPIO_WritePin(KVMSW_EN_GPIO_Port, KVMSW_EN_Pin, 0);
}

void system_switch_init()
{
    // power enable
    // switch disaple
    HAL_GPIO_WritePin(KVMSW_EN_GPIO_Port, KVMSW_EN_Pin, 0);
}

void system_switch_tasks()
{
    if (HAL_GetTick() - lastTimeSystemSwitchTask > 373)
    {
        lastTimeSystemSwitchTask = HAL_GetTick();
        uint8_t cc = get_current_channel();
        if(lastChannelSelectSW != cc){
            lastChannelSelectSW = cc;
            system_switch(cc);
        }
    }
}