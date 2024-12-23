/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file         stm32f4xx_hal_msp.c
  * @brief        This file provides code for the MSP Initialization
  *               and de-Initialization codes.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "main.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */


/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Define */

/* USER CODE END Define */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Macro */

/* USER CODE END Macro */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External functions --------------------------------------------------------*/
/* USER CODE BEGIN ExternalFunctions */

/* USER CODE END ExternalFunctions */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  /* System interrupt init*/

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

/* USER CODE BEGIN 1 */


///**
//* @brief SPI MSP Initialization
//* This function configures the hardware resources used in this example
//* @param hspi: SPI handle pointer
//* @retval None
//*/
//void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
//{
//  GPIO_InitTypeDef GPIO_InitStruct = {0};
//  if(hspi->Instance==SPI2)
//  {
//  /* USER CODE BEGIN SPI2_MspInit 0 */

//  /* USER CODE END SPI2_MspInit 0 */
//    /* Peripheral clock enable */
//    __HAL_RCC_SPI2_CLK_ENABLE();

//    __HAL_RCC_GPIOB_CLK_ENABLE();
//    /**SPI2 GPIO Configuration
//    PB10     ------> SPI2_SCK
//    PB14     ------> SPI2_MISO
//    PB15     ------> SPI2_MOSI
//    PB9     ------> SPI2_NSS
//    */
//    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_9;
//    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//    GPIO_InitStruct.Pull = GPIO_NOPULL;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//    GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
//    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

//    /* SPI2 DMA Init */
//    /* SPI2_RX Init */
////    hdma_spi2_rx.Instance = DMA1_Stream3;
////    hdma_spi2_rx.Init.Channel = DMA_CHANNEL_0;
////    hdma_spi2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
////    hdma_spi2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
////    hdma_spi2_rx.Init.MemInc = DMA_MINC_ENABLE;
////    hdma_spi2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
////    hdma_spi2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
////    hdma_spi2_rx.Init.Mode = DMA_NORMAL;
////    hdma_spi2_rx.Init.Priority = DMA_PRIORITY_LOW;
////    hdma_spi2_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
////    if (HAL_DMA_Init(&hdma_spi2_rx) != HAL_OK)
////    {
////      Error_Handler();
////    }

////    __HAL_LINKDMA(hspi,hdmarx,hdma_spi2_rx);

//    /* SPI2_TX Init */
////    hdma_spi2_tx.Instance = DMA1_Stream4;
////    hdma_spi2_tx.Init.Channel = DMA_CHANNEL_0;
////    hdma_spi2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
////    hdma_spi2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
////    hdma_spi2_tx.Init.MemInc = DMA_MINC_ENABLE;
////    hdma_spi2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
////    hdma_spi2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
////    hdma_spi2_tx.Init.Mode = DMA_NORMAL;
////    hdma_spi2_tx.Init.Priority = DMA_PRIORITY_LOW;
////    hdma_spi2_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
////    if (HAL_DMA_Init(&hdma_spi2_tx) != HAL_OK)
////    {
////      Error_Handler();
////    }

////    __HAL_LINKDMA(hspi,hdmatx,hdma_spi2_tx);

//  /* USER CODE BEGIN SPI2_MspInit 1 */

//  /* USER CODE END SPI2_MspInit 1 */
//  }

//}

///**
//* @brief SPI MSP De-Initialization
//* This function freeze the hardware resources used in this example
//* @param hspi: SPI handle pointer
//* @retval None
//*/
//void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
//{
//  if(hspi->Instance==SPI2)
//  {
//  /* USER CODE BEGIN SPI2_MspDeInit 0 */

//  /* USER CODE END SPI2_MspDeInit 0 */
//    /* Peripheral clock disable */
//    __HAL_RCC_SPI2_CLK_DISABLE();

//    /**SPI2 GPIO Configuration
//    PB10     ------> SPI2_SCK
//    PB14     ------> SPI2_MISO
//    PB15     ------> SPI2_MOSI
//    PB9     ------> SPI2_NSS
//    */
//    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10|GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_9);

//    /* SPI2 DMA DeInit */
//    HAL_DMA_DeInit(hspi->hdmarx);
//    HAL_DMA_DeInit(hspi->hdmatx);
//  /* USER CODE BEGIN SPI2_MspDeInit 1 */

//  /* USER CODE END SPI2_MspDeInit 1 */
//  }

//}


/**
* @brief ADC MSP Initialization
* This function configures the hardware resources used in this example
* @param hadc: ADC handle pointer
* @retval None
*/
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(hadc->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspInit 0 */

  /* USER CODE END ADC1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_ADC1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**ADC1 GPIO Configuration
    PA4     ------> ADC1_IN4
    */
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN ADC1_MspInit 1 */
    HAL_NVIC_SetPriority(ADC_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(ADC_IRQn);
  /* USER CODE END ADC1_MspInit 1 */
  }

}


/**
* @brief ADC MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hadc: ADC handle pointer
* @retval None
*/
void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
  if(hadc->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspDeInit 0 */

  /* USER CODE END ADC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC1_CLK_DISABLE();

    /**ADC1 GPIO Configuration
    PA4     ------> ADC1_IN4
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4);

  /* USER CODE BEGIN ADC1_MspDeInit 1 */

  /* USER CODE END ADC1_MspDeInit 1 */
  }

}

/**
* @brief TIM_Base MSP Initialization
* This function configures the hardware resources used in this example
* @param htim_base: TIM_Base handle pointer
* @retval None
*/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(htim_base->Instance==TIM1)
  {
  /* USER CODE BEGIN TIM1_MspInit 0 */

  /* USER CODE END TIM1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_TIM1_CLK_ENABLE();

    __HAL_RCC_GPIOE_CLK_ENABLE();
    /**TIM1 GPIO Configuration
    PE9     ------> TIM1_CH1
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /* TIM1 interrupt Init */
    HAL_NVIC_SetPriority(TIM1_CC_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
  /* USER CODE BEGIN TIM1_MspInit 1 */

  /* USER CODE END TIM1_MspInit 1 */
  }
  if(htim_base->Instance==TIM6)
  {
  /* USER CODE BEGIN TIM6_MspInit 0 */

  /* USER CODE END TIM6_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_TIM6_CLK_ENABLE();
    /* TIM6 interrupt Init */
    HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 4, 0);
    HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
  /* USER CODE BEGIN TIM6_MspInit 1 */

  /* USER CODE END TIM6_MspInit 1 */
  }
	if(htim_base->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspInit 0 */

  /* USER CODE END TIM2_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_TIM2_CLK_ENABLE();
    /* TIM2 interrupt Init */
    HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
  /* USER CODE BEGIN TIM2_MspInit 1 */

  /* USER CODE END TIM2_MspInit 1 */
  }
}



/**
* @brief TIM_Base MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param htim_base: TIM_Base handle pointer
* @retval None
*/
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
{
  if(htim_base->Instance==TIM1)
  {
  /* USER CODE BEGIN TIM1_MspDeInit 0 */

  /* USER CODE END TIM1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM1_CLK_DISABLE();

    /**TIM1 GPIO Configuration
    PE9     ------> TIM1_CH1
    */
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_9);

    /* TIM1 interrupt DeInit */
    HAL_NVIC_DisableIRQ(TIM1_CC_IRQn);
  /* USER CODE BEGIN TIM1_MspDeInit 1 */

  /* USER CODE END TIM1_MspDeInit 1 */
  }
  if(htim_base->Instance==TIM6)
  {
  /* USER CODE BEGIN TIM6_MspDeInit 0 */

  /* USER CODE END TIM6_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM6_CLK_DISABLE();

    /* TIM6 interrupt DeInit */
    HAL_NVIC_DisableIRQ(TIM6_DAC_IRQn);
  /* USER CODE BEGIN TIM6_MspDeInit 1 */

  /* USER CODE END TIM6_MspDeInit 1 */
  }
}

/* USER CODE END 1 */
