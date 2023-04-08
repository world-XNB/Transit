/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Led_Pin GPIO_PIN_13
#define Led_GPIO_Port GPIOC
#define Output1_Pin GPIO_PIN_2
#define Output1_GPIO_Port GPIOA
#define Output2_Pin GPIO_PIN_3
#define Output2_GPIO_Port GPIOA
#define Output3_Pin GPIO_PIN_4
#define Output3_GPIO_Port GPIOA
#define Output4_Pin GPIO_PIN_5
#define Output4_GPIO_Port GPIOA
#define Input1_Pin GPIO_PIN_3
#define Input1_GPIO_Port GPIOB
#define Input2_Pin GPIO_PIN_4
#define Input2_GPIO_Port GPIOB
#define Input3_Pin GPIO_PIN_5
#define Input3_GPIO_Port GPIOB
#define Input4_Pin GPIO_PIN_6
#define Input4_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
//#define SINGLE_END_MARK      //单结束位时打开


/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
