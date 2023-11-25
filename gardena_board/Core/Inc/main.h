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
#include "stm32g0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//#include <stdbool.h>
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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define CHG_nCE_Pin GPIO_PIN_14
#define CHG_nCE_GPIO_Port GPIOC
#define CHG_nINT_Pin GPIO_PIN_15
#define CHG_nINT_GPIO_Port GPIOC
#define CHG_SDA_Pin GPIO_PIN_0
#define CHG_SDA_GPIO_Port GPIOA
#define CHG_SCL_Pin GPIO_PIN_1
#define CHG_SCL_GPIO_Port GPIOA
#define VBAT_Pin GPIO_PIN_2
#define VBAT_GPIO_Port GPIOA
#define CHG_STAT_Pin GPIO_PIN_3
#define CHG_STAT_GPIO_Port GPIOA
#define moafet_q5_Pin GPIO_PIN_4
#define moafet_q5_GPIO_Port GPIOA
#define MOTOR_CURRENT_Pin GPIO_PIN_5
#define MOTOR_CURRENT_GPIO_Port GPIOA
#define NTC_Pin GPIO_PIN_6
#define NTC_GPIO_Port GPIOA
#define mosfetu4small_Pin GPIO_PIN_7
#define mosfetu4small_GPIO_Port GPIOA
#define mosfetu5_big_Pin GPIO_PIN_0
#define mosfetu5_big_GPIO_Port GPIOB
#define H1_Pin GPIO_PIN_1
#define H1_GPIO_Port GPIOB
#define H2_Pin GPIO_PIN_2
#define H2_GPIO_Port GPIOB
#define mosfetu4_big_Pin GPIO_PIN_8
#define mosfetu4_big_GPIO_Port GPIOA
#define mosfetu5_small_Pin GPIO_PIN_9
#define mosfetu5_small_GPIO_Port GPIOA
#define r10k_and1k_to_3_3v_Pin GPIO_PIN_11
#define r10k_and1k_to_3_3v_GPIO_Port GPIOA
#define is_charger_present_Pin GPIO_PIN_12
#define is_charger_present_GPIO_Port GPIOA
#define Keep_Me_On_Senpai_Pin_Pin GPIO_PIN_15
#define Keep_Me_On_Senpai_Pin_GPIO_Port GPIOA
#define q11_g_Charge_EN_Pin GPIO_PIN_3
#define q11_g_Charge_EN_GPIO_Port GPIOB
#define led1_Pin GPIO_PIN_4
#define led1_GPIO_Port GPIOB
#define led2_Pin GPIO_PIN_5
#define led2_GPIO_Port GPIOB
#define u10_dcp__Pin GPIO_PIN_6
#define u10_dcp__GPIO_Port GPIOB
#define led3_Pin GPIO_PIN_7
#define led3_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
