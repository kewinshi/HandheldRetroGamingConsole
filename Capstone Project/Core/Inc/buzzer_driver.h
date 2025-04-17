#ifndef BUZZER_DRIVER_H
#define BUZZER_DRIVER_H

#include "stm32f1xx_hal.h"  // Include HAL for STM32 functions

void toggle_PA11(void);
void set_PA11(uint8_t state);
void buzz(uint16_t time);
void buzz_low(uint16_t time);
void caneck_caneck(void);

#endif
