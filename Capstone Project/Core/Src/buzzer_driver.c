#include "buzzer_driver.h"
#include "main.h"


void toggle_PA11(void) {
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_11);  // Toggle PA11
}

void set_PA11(uint8_t state) {
    if (state) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);  // Set PA11 high
    } else {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET); // Set PA11 low
    }
}
