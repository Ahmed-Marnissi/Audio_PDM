/**
 * @file microphone.c
 * @brief Microphone hardware abstraction implementation.
 *
 * Implements initialization, DMA start, stop, and callback registration for the microphone interface.
 *
 * @author ahmed marnissi
 * @date April 12, 2024
 */

/*
 * microphone.c
 *
 *  Created on: Apr 12, 2024
 *      Author: 21624
 */

#include "microphone.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_def.h"
static  DMA_HandleTypeDef hdma_spi_rx ;

static  I2S_HandleTypeDef hi2s ;

/**
 * @brief DMA1 Stream3 interrupt handler for microphone DMA transfers.
 */
void DMA1_Stream3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream3_IRQn 0 */

  /* USER CODE END DMA1_Stream3_IRQn 0 */
  HAL_DMA_IRQHandler(  &hdma_spi_rx );
  /* USER CODE BEGIN DMA1_Stream3_IRQn 1 */

  /* USER CODE END DMA1_Stream3_IRQn 1 */
}


/**
 * @brief Initialize microphone hardware and peripherals.
 * @retval 0U on success, error code otherwise
 */
uint8_t u8Microphone_Init ( void  )
{


	  GPIO_InitTypeDef GPIO_InitStruct = {0};
	  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

	  /** Initializes the peripherals clock
		  */
		    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S;
		    PeriphClkInitStruct.PLLI2S.PLLI2SN = 369;
		    PeriphClkInitStruct.PLLI2S.PLLI2SR = 6;
		    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
		    {
		      return 1U ;
		    }
	  __HAL_RCC_DMA1_CLK_ENABLE();

	  HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 5, 0);
	  HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);



	  /* GPIO Ports Clock Enable */
	  __HAL_RCC_GPIOH_CLK_ENABLE();
	  __HAL_RCC_GPIOC_CLK_ENABLE();
	  __HAL_RCC_GPIOB_CLK_ENABLE();
	  __HAL_RCC_GPIOA_CLK_ENABLE();
	    /* Peripheral clock enable */
	    __HAL_RCC_SPI2_CLK_ENABLE();

	    hi2s.Instance = SPI2;
	    hi2s.Init.Mode = I2S_MODE_MASTER_RX;
	    hi2s.Init.Standard = I2S_STANDARD_PHILIPS;
	    hi2s.Init.DataFormat = I2S_DATAFORMAT_16B;
	    hi2s.Init.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
	    hi2s.Init.AudioFreq = I2S_AUDIOFREQ_32K;
	    hi2s.Init.CPOL = I2S_CPOL_LOW;
	    hi2s.Init.ClockSource = I2S_CLOCK_PLL;
	    hi2s.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;
	    if (HAL_I2S_Init(&hi2s) != HAL_OK)
	    {
	     return 3U ;
	    }


	  if( hi2s.Instance  == SPI2  )
	  {




	    __HAL_RCC_GPIOC_CLK_ENABLE();
	    __HAL_RCC_GPIOB_CLK_ENABLE();
	    /**I2S2 GPIO Configuration
	    PC2     ------> I2S2_ext_SD
	    PC3     ------> I2S2_SD
	    PB10     ------> I2S2_CK
	    PB12     ------> I2S2_WS
	    */
	    GPIO_InitStruct.Pin = GPIO_PIN_2;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	    GPIO_InitStruct.Alternate = GPIO_AF6_I2S2ext;
	    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	    GPIO_InitStruct.Pin = GPIO_PIN_3;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	    GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
	    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_12;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	    GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
	    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	    /* I2S DMA Init */
	    /* SPI_RX Init */
	    hdma_spi_rx.Instance = DMA1_Stream3;
	    hdma_spi_rx.Init.Channel = DMA_CHANNEL_0;
	    hdma_spi_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
	    hdma_spi_rx.Init.PeriphInc = DMA_PINC_DISABLE;
	    hdma_spi_rx.Init.MemInc = DMA_MINC_ENABLE;
	    hdma_spi_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
	    hdma_spi_rx.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
	    hdma_spi_rx.Init.Mode = DMA_CIRCULAR;
	    hdma_spi_rx.Init.Priority = DMA_PRIORITY_LOW;
	    hdma_spi_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

	    if (HAL_DMA_Init(&hdma_spi_rx) != HAL_OK)
	    {
	      return  2U ;
	    }
	  }

	    __HAL_LINKDMA( &hi2s , hdmarx , hdma_spi_rx );



	    return 0U ;

}

/**
 * @brief Start microphone DMA acquisition.
 * @param u16buffer Pointer to buffer for PDM data
 * @param datalen Number of PDM samples
 * @retval 0U on success, error code otherwise
 */
uint8_t u8Microphone_Start ( uint16_t * u16buffer  , uint16_t  datalen  )
{
	if  ( HAL_OK !=  HAL_I2S_Receive_DMA(&hi2s , u16buffer  , datalen ) )
	{
		return 1U ;
	}
	return  0U ;
}

/**
 * @brief Register microphone callback (if supported).
 * @retval 0U on success, error code otherwise
 */
uint8_t u8Microphone_RegisterCallback ( void  )
{
	return 0U;
}

/**
 * @brief Stop microphone acquisition.
 * @retval 0U on success, error code otherwise
 */
uint8_t u8Microphone_Stop( void  )
{
	return 0U ;
}
