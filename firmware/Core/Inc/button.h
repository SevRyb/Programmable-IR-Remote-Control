/*
 * button.h
 *
 *  Created on:
 *      Author: Severyn
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include "main.h"

#define N_BUTTONS 11				// Number of buttons
#define DEBOUNCE_TIME_MS 10			// Debounce time in ms
#define LONG_PRESS_TIME_MS 1000		// Long press time in ms

typedef enum {
	BUTTON_STATE_RELEASED,
	BUTTON_STATE_PRESSED,
} ButtonState;

typedef enum {
	BUTTON_EVENT_PRESS,
	BUTTON_EVENT_RELEASE,
	BUTTON_EVENT_LONG_RELEASE,
	BUTTON_EVENT_HOLD
} ButtonEvent;

typedef struct {
    GPIO_TypeDef* port;
    uint16_t pin;

    ButtonState state;
    void (*callback)(ButtonEvent, uint8_t);

    uint8_t debounceTime;
    uint16_t holdTime;
    uint8_t isHoldEventFired;

    GPIO_PinState activeState;

} Button_t;

void Button_Init(void);
void Button_TimerTick(void); // Call every 1 ms
void Button_EXTI_Handler(uint16_t GPIO_Pin);
ButtonState Button_GetState(uint8_t id);

#endif /* INC_BUTTON_H_ */
