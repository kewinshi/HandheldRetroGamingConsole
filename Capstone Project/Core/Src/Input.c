#include "Input.h"
#include "stm32f1xx_hal.h"  // Includes the HAL library
#include "main.h"           // Includes project-specific definitions (like LD2_GPIO_Port)

int get_input(void) {
	if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0) == GPIO_PIN_RESET) // back
	{
		return 0;
	}
	else if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1) == GPIO_PIN_RESET) // select
	{
		return 1;
	}
	else if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_2) == GPIO_PIN_RESET) // up
	{
//		print("up");
		return 2;
	}
	else if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_3) == GPIO_PIN_RESET) // left
	{
		return 3;
//		print("left");
	}
	else if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4) == GPIO_PIN_RESET) // right
	{
		return 4;
//		print("right");
	}
	else if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5) == GPIO_PIN_RESET) // down
	{
		return 5;
//		print("down");
	}
	else {
		return -1;
	}
}
