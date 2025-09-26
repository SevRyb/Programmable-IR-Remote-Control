/*
 * button.c
 *
 *  Created on:
 *      Author: Severyn
 */

#include "button.h"

/* Callbacks */
extern void onBtnMenu(ButtonEvent event, uint8_t id);
extern void onBtnSys(ButtonEvent event, uint8_t id);
extern void onBtnGeneralPurpose(ButtonEvent event, uint8_t id);

static Button_t g_buttons[N_BUTTONS] = {
		{.port = BTN_MENU_GPIO_Port, .pin = BTN_MENU_Pin, .callback = onBtnMenu},
		{.port = BTN1_GPIO_Port, .pin = BTN1_Pin, .callback = onBtnGeneralPurpose},
		{.port = BTN2_GPIO_Port, .pin = BTN2_Pin, .callback = onBtnGeneralPurpose},
		{.port = BTN3_GPIO_Port, .pin = BTN3_Pin, .callback = onBtnGeneralPurpose},
		{.port = BTN4_GPIO_Port, .pin = BTN4_Pin, .callback = onBtnGeneralPurpose},
		{.port = BTN5_GPIO_Port, .pin = BTN5_Pin, .callback = onBtnGeneralPurpose},
		{.port = BTN6_GPIO_Port, .pin = BTN6_Pin, .callback = onBtnGeneralPurpose},
		{.port = BTN7_GPIO_Port, .pin = BTN7_Pin, .callback = onBtnGeneralPurpose},
		{.port = BTN8_GPIO_Port, .pin = BTN8_Pin, .callback = onBtnGeneralPurpose},
		{.port = BTN9_GPIO_Port, .pin = BTN9_Pin, .callback = onBtnGeneralPurpose},
		{.port = BTN_SYS_GPIO_Port, .pin = BTN_SYS_Pin, .callback = onBtnSys},
};

void Button_Init(void)
{
    for (uint8_t i = 0; i < N_BUTTONS; ++i)
    {
    	g_buttons[i].state = BUTTON_STATE_RELEASED;
    	g_buttons[i].activeState = GPIO_PIN_RESET;
    	g_buttons[i].debounceTime = 0;
    	g_buttons[i].holdTime = 0;
    	g_buttons[i].isHoldEventFired = 0;
    }
}

void Button_EXTI_Handler(uint16_t GPIO_Pin)
{
    for (uint8_t i = 0; i < N_BUTTONS; ++i)
    {
    	if (g_buttons[i].pin == GPIO_Pin)
    	{
			if (g_buttons[i].debounceTime == 0
			 && ((g_buttons[i].state == BUTTON_STATE_RELEASED && HAL_GPIO_ReadPin(g_buttons[i].port, g_buttons[i].pin) == g_buttons[i].activeState)
			 || (g_buttons[i].state == BUTTON_STATE_PRESSED && HAL_GPIO_ReadPin(g_buttons[i].port, g_buttons[i].pin) == !g_buttons[i].activeState)))
			{
				g_buttons[i].debounceTime = DEBOUNCE_TIME_MS;
			}
			break;
    	}
    }
}

void Button_TimerTick(void)
{
    for (uint8_t i = 0; i < N_BUTTONS; ++i)
    {
        if (g_buttons[i].debounceTime > 0)
        {
        	g_buttons[i].debounceTime--;
            if (g_buttons[i].debounceTime == 0)
            {
                if (HAL_GPIO_ReadPin(g_buttons[i].port, g_buttons[i].pin) == g_buttons[i].activeState)
                {
                	if (g_buttons[i].state == BUTTON_STATE_RELEASED)
                	{
                		g_buttons[i].state = BUTTON_STATE_PRESSED;
                		if (g_buttons[i].callback)
                			g_buttons[i].callback(BUTTON_EVENT_PRESS, i);
                	}
                }
                else
                {
                	if (g_buttons[i].state == BUTTON_STATE_PRESSED)
                	{
                		g_buttons[i].state = BUTTON_STATE_RELEASED;
                		g_buttons[i].holdTime = 0;
                		if (g_buttons[i].isHoldEventFired)
                		{
                			g_buttons[i].isHoldEventFired = 0;
                			if (g_buttons[i].callback)
                				g_buttons[i].callback(BUTTON_EVENT_LONG_RELEASE, i);
                		}
                		else
                		{
                			if (g_buttons[i].callback)
                				g_buttons[i].callback(BUTTON_EVENT_RELEASE, i);
                		}
                	}
                }
            }
        }
        if (g_buttons[i].state == BUTTON_STATE_PRESSED)
        {
        	if (g_buttons[i].holdTime < LONG_PRESS_TIME_MS)
        		g_buttons[i].holdTime++;
        	else
        	{
        		g_buttons[i].holdTime = 0;
        		if (!g_buttons[i].isHoldEventFired)
        		{
        			g_buttons[i].callback(BUTTON_EVENT_HOLD, i);
        			g_buttons[i].isHoldEventFired = 1;
        		}
        	}
        }
    }
}

ButtonState Button_GetState(uint8_t id)
{
	return g_buttons[id].state;
}
