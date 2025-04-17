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

void buzz(uint16_t time) {
	for (int i = 0; i < time; i +=1 ) {
		toggle_PA11();
		HAL_Delay(1);
	}
}

void buzz_low(uint16_t time) {
	for (int i = 0; i < time; i +=1 ) {
		toggle_PA11();
		HAL_Delay(5);
	}
}
void caneck_caneck(void) {
	// triples
	for (int i = 0; i < 3; i += 1) {
		buzz(100);
		HAL_Delay(100);
	}

	// quarter note
	buzz(100);
	HAL_Delay(300);

	// eighth note
	buzz(100);
	HAL_Delay(125);

	// quarter note
	buzz(100);
	HAL_Delay(300);

	// eighth note
	buzz(100);
	HAL_Delay(125);

	// quarter note
	buzz(100);
	HAL_Delay(300);



}
