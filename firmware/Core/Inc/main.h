/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
void onSysTickUpdate();

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define IR_SEND_Pin GPIO_PIN_3
#define IR_SEND_GPIO_Port GPIOA
#define BAT_LEVEL_Pin GPIO_PIN_5
#define BAT_LEVEL_GPIO_Port GPIOA
#define BTN_MENU_Pin GPIO_PIN_0
#define BTN_MENU_GPIO_Port GPIOB
#define BTN_MENU_EXTI_IRQn EXTI0_IRQn
#define BTN1_Pin GPIO_PIN_1
#define BTN1_GPIO_Port GPIOB
#define BTN1_EXTI_IRQn EXTI1_IRQn
#define BTN2_Pin GPIO_PIN_2
#define BTN2_GPIO_Port GPIOB
#define BTN2_EXTI_IRQn EXTI2_IRQn
#define IR_RECV_38_Pin GPIO_PIN_12
#define IR_RECV_38_GPIO_Port GPIOB
#define IR_RECV_36_Pin GPIO_PIN_13
#define IR_RECV_36_GPIO_Port GPIOB
#define IR_RECV_40_Pin GPIO_PIN_14
#define IR_RECV_40_GPIO_Port GPIOB
#define BTN_SYS_Pin GPIO_PIN_15
#define BTN_SYS_GPIO_Port GPIOB
#define BTN_SYS_EXTI_IRQn EXTI15_10_IRQn
#define IR_RECV_EN_Pin GPIO_PIN_8
#define IR_RECV_EN_GPIO_Port GPIOA
#define BTN3_Pin GPIO_PIN_3
#define BTN3_GPIO_Port GPIOB
#define BTN3_EXTI_IRQn EXTI3_IRQn
#define BTN4_Pin GPIO_PIN_4
#define BTN4_GPIO_Port GPIOB
#define BTN4_EXTI_IRQn EXTI4_IRQn
#define BTN5_Pin GPIO_PIN_5
#define BTN5_GPIO_Port GPIOB
#define BTN5_EXTI_IRQn EXTI9_5_IRQn
#define BTN6_Pin GPIO_PIN_6
#define BTN6_GPIO_Port GPIOB
#define BTN6_EXTI_IRQn EXTI9_5_IRQn
#define BTN7_Pin GPIO_PIN_7
#define BTN7_GPIO_Port GPIOB
#define BTN7_EXTI_IRQn EXTI9_5_IRQn
#define BTN8_Pin GPIO_PIN_8
#define BTN8_GPIO_Port GPIOB
#define BTN8_EXTI_IRQn EXTI9_5_IRQn
#define BTN9_Pin GPIO_PIN_9
#define BTN9_GPIO_Port GPIOB
#define BTN9_EXTI_IRQn EXTI9_5_IRQn

/* USER CODE BEGIN Private defines */
#define IRMP_Receive_GPIO_Port IR_RECV_38_GPIO_Port
#define IRMP_Receive_Pin IR_RECV_38_Pin
#define IRSND_Transmit_GPIO_Port IR_SEND_GPIO_Port
#define IRSND_Transmit_Pin IR_SEND_Pin

#define _Error_Handler(file, line) Error_Handler()


/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
