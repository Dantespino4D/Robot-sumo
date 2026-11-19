/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    comp.c
  * @brief   This file provides code for the configuration
  *          of the COMP instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "comp.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

COMP_HandleTypeDef hcomp1;
COMP_HandleTypeDef hcomp2;
COMP_HandleTypeDef hcomp7;

/* COMP1 init function */
void MX_COMP1_Init(void)
{

  /* USER CODE BEGIN COMP1_Init 0 */

  /* USER CODE END COMP1_Init 0 */

  /* USER CODE BEGIN COMP1_Init 1 */

  /* USER CODE END COMP1_Init 1 */
  hcomp1.Instance = COMP1;
  hcomp1.Init.InputPlus = COMP_INPUT_PLUS_IO2;
  hcomp1.Init.InputMinus = COMP_INPUT_MINUS_DAC1_CH1;
  hcomp1.Init.OutputPol = COMP_OUTPUTPOL_INVERTED;
  hcomp1.Init.Hysteresis = COMP_HYSTERESIS_MEDIUM;
  hcomp1.Init.BlankingSrce = COMP_BLANKINGSRC_NONE;
  hcomp1.Init.TriggerMode = COMP_TRIGGERMODE_NONE;
  if (HAL_COMP_Init(&hcomp1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN COMP1_Init 2 */

  /* USER CODE END COMP1_Init 2 */

}
/* COMP2 init function */
void MX_COMP2_Init(void)
{

  /* USER CODE BEGIN COMP2_Init 0 */

  /* USER CODE END COMP2_Init 0 */

  /* USER CODE BEGIN COMP2_Init 1 */

  /* USER CODE END COMP2_Init 1 */
  hcomp2.Instance = COMP2;
  hcomp2.Init.InputPlus = COMP_INPUT_PLUS_IO2;
  hcomp2.Init.InputMinus = COMP_INPUT_MINUS_DAC1_CH2;
  hcomp2.Init.OutputPol = COMP_OUTPUTPOL_INVERTED;
  hcomp2.Init.Hysteresis = COMP_HYSTERESIS_MEDIUM;
  hcomp2.Init.BlankingSrce = COMP_BLANKINGSRC_NONE;
  hcomp2.Init.TriggerMode = COMP_TRIGGERMODE_NONE;
  if (HAL_COMP_Init(&hcomp2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN COMP2_Init 2 */

  /* USER CODE END COMP2_Init 2 */

}
/* COMP7 init function */
void MX_COMP7_Init(void)
{

  /* USER CODE BEGIN COMP7_Init 0 */

  /* USER CODE END COMP7_Init 0 */

  /* USER CODE BEGIN COMP7_Init 1 */

  /* USER CODE END COMP7_Init 1 */
  hcomp7.Instance = COMP7;
  hcomp7.Init.InputPlus = COMP_INPUT_PLUS_IO1;
  hcomp7.Init.InputMinus = COMP_INPUT_MINUS_DAC2_CH1;
  hcomp7.Init.OutputPol = COMP_OUTPUTPOL_INVERTED;
  hcomp7.Init.Hysteresis = COMP_HYSTERESIS_MEDIUM;
  hcomp7.Init.BlankingSrce = COMP_BLANKINGSRC_NONE;
  hcomp7.Init.TriggerMode = COMP_TRIGGERMODE_NONE;
  if (HAL_COMP_Init(&hcomp7) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN COMP7_Init 2 */

  /* USER CODE END COMP7_Init 2 */

}

void HAL_COMP_MspInit(COMP_HandleTypeDef* compHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(compHandle->Instance==COMP1)
  {
  /* USER CODE BEGIN COMP1_MspInit 0 */

  /* USER CODE END COMP1_MspInit 0 */

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**COMP1 GPIO Configuration
    PB1     ------> COMP1_INP
    */
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN COMP1_MspInit 1 */

  /* USER CODE END COMP1_MspInit 1 */
  }
  else if(compHandle->Instance==COMP2)
  {
  /* USER CODE BEGIN COMP2_MspInit 0 */

  /* USER CODE END COMP2_MspInit 0 */

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**COMP2 GPIO Configuration
    PA3     ------> COMP2_INP
    */
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN COMP2_MspInit 1 */

  /* USER CODE END COMP2_MspInit 1 */
  }
  else if(compHandle->Instance==COMP7)
  {
  /* USER CODE BEGIN COMP7_MspInit 0 */

  /* USER CODE END COMP7_MspInit 0 */

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**COMP7 GPIO Configuration
    PB14     ------> COMP7_INP
    */
    GPIO_InitStruct.Pin = GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN COMP7_MspInit 1 */

  /* USER CODE END COMP7_MspInit 1 */
  }
}

void HAL_COMP_MspDeInit(COMP_HandleTypeDef* compHandle)
{

  if(compHandle->Instance==COMP1)
  {
  /* USER CODE BEGIN COMP1_MspDeInit 0 */

  /* USER CODE END COMP1_MspDeInit 0 */

    /**COMP1 GPIO Configuration
    PB1     ------> COMP1_INP
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_1);

  /* USER CODE BEGIN COMP1_MspDeInit 1 */

  /* USER CODE END COMP1_MspDeInit 1 */
  }
  else if(compHandle->Instance==COMP2)
  {
  /* USER CODE BEGIN COMP2_MspDeInit 0 */

  /* USER CODE END COMP2_MspDeInit 0 */

    /**COMP2 GPIO Configuration
    PA3     ------> COMP2_INP
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_3);

  /* USER CODE BEGIN COMP2_MspDeInit 1 */

  /* USER CODE END COMP2_MspDeInit 1 */
  }
  else if(compHandle->Instance==COMP7)
  {
  /* USER CODE BEGIN COMP7_MspDeInit 0 */

  /* USER CODE END COMP7_MspDeInit 0 */

    /**COMP7 GPIO Configuration
    PB14     ------> COMP7_INP
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_14);

  /* USER CODE BEGIN COMP7_MspDeInit 1 */

  /* USER CODE END COMP7_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

