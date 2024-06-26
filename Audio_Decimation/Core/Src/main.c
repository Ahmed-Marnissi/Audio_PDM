/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "decimation._task.h"
#include "fft_task.h"
#include "FreeRTOS.h"
#include "task.h"

#include "debug.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef enum
{
	AUDIO_IDLE = 0U ,
	AUDIO_PLAYBACK  ,
	AUDIO_EQUALIZER ,

}DevicceMode_en ;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */



/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/


/* USER CODE BEGIN PV */
uint8_t u8USBrecieveBuffer[64];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t  uBuffercmp (uint8_t *buffer1 , uint8_t *buffer2, uint16_t  length )
{
    for (size_t i = 0; i < length; i++)
    {
        if (buffer1[i] != buffer2[i])
        {
            return 1U;
        }
    }
    return 0U;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
   HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  DEBUG_UTILS_INIT();
  /* USER CODE END SysInit */

  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
  /* Wait for CMD */

  uint8_t u8ModeSelected = 0U ;
  DevicceMode_en Mode = AUDIO_IDLE ;
  do
  {
	  if ( 0U == uBuffercmp(u8USBrecieveBuffer , "PB_MODE"  , 7U ) )
	  {
		  	  u8ModeSelected = 1U ;
		  	  Mode =AUDIO_PLAYBACK ;
	  }
	  else if (0U == uBuffercmp(u8USBrecieveBuffer , "EQ_MODE"  , 7U ) )
	  {
		      u8ModeSelected = 1U ;
		      Mode =AUDIO_EQUALIZER ;
	  }else
	  {
		  /*Nothing TO do */
	  }

  }while ( u8ModeSelected == 0U );

  if ( u8ModeSelected == 1U  )
  {
  BaseType_t    xReturned  =pdFALSE;

		         xReturned = xTaskCreate(
		  	  	  vDecimationTaskRoutine ,
                  "DECIMATION_TASK" ,
                  300U,
				  (void * )Mode ,
                  tskIDLE_PRIORITY + 2U ,
                  NULL
				  );


  if (  xReturned != pdPASS )
  {
	  /*Reset */
  }

  xReturned = xTaskCreate(
		  vFFT_TaskRoutine  ,
		  "FFT_TASK" ,
		  300U,
		  (void * )Mode ,
		  tskIDLE_PRIORITY + 1U ,
		  NULL
		  );


    if (  xReturned != pdPASS )
    {
  	  /*Reset */
    }


  /* start system */
  vTaskStartScheduler();
  }
  else
  {
	  /*Nothing to  do */
  }

  /* USER CODE END 2 */


  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}


void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
