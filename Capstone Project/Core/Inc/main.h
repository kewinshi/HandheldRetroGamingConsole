/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
void print(char *str);
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define btn_back_Pin GPIO_PIN_0
#define btn_back_GPIO_Port GPIOC
#define btn_select_Pin GPIO_PIN_1
#define btn_select_GPIO_Port GPIOC
#define btn_up_Pin GPIO_PIN_2
#define btn_up_GPIO_Port GPIOC
#define btn_left_Pin GPIO_PIN_3
#define btn_left_GPIO_Port GPIOC
#define btn_right_Pin GPIO_PIN_4
#define btn_right_GPIO_Port GPIOC
#define btn_down_Pin GPIO_PIN_5
#define btn_down_GPIO_Port GPIOC
#define BL_Pin GPIO_PIN_7
#define BL_GPIO_Port GPIOC
#define DC_Pin GPIO_PIN_8
#define DC_GPIO_Port GPIOA
#define RST_Pin GPIO_PIN_9
#define RST_GPIO_Port GPIOA
#define buzzer_Pin GPIO_PIN_11
#define buzzer_GPIO_Port GPIOA
#define CS_Pin GPIO_PIN_6
#define CS_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
typedef struct {
    int x, y;
    int active;
} Point;

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
