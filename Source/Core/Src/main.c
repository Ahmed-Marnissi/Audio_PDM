/**
 * @file main.c
 * @brief Main program entry point for Audio Decimation/FFT Application.
 *
 * Initializes system, selects mode via USB, and starts decimation and FFT tasks using FreeRTOS.
 *
 * @author ahmed marnissi
 * @date 2024
 */



/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "FreeRTOS.h"
#include "task.h"
#include "debug.h"
#include "../../APP/Task_Decimation/Inc/decimation._task.h"
#include "../../APP/Task_fft/Inc/fft_task.h"

typedef enum 
{
    USB_MODE_WAIT = 0U,
    USB_MODE_CHECK,
    USB_MODE_SELECTED
} UsbModeState_t;

typedef enum
{
	AUDIO_IDLE = 0U ,
	AUDIO_PLAYBACK  ,
	AUDIO_EQUALIZER ,

}DeviceMode_t ;


/* USER CODE BEGIN PV */
uint8_t u8USBrecieveBuffer[64];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/**
 * @brief Compare two buffers for equality.
 * @param buffer1 First buffer
 * @param buffer2 Second buffer
 * @param length Length in bytes
 * @retval 0U if equal, 1U if not
 */
static uint8_t uBuffercmp(uint8_t *buffer1, uint8_t *buffer2, uint16_t length);

/**
 * @brief Start audio decimation application: create tasks and start scheduler.
 *
 * @param mode Audio processing mode (AUDIO_PLAYBACK, AUDIO_EQUALIZER, ...)
 */
static void start_audio_decimation_app(uint8_t mode);

/**
 * @brief Wait for user to select audio mode via USB.
 *
 * @param u8USBrecieveBuffer Pointer to USB receive buffer
 * @return DeviceMode_t Selected mode (AUDIO_PLAYBACK, AUDIO_EQUALIZER, ...)
 */
static DeviceMode_t wait_for_user_select_mode(uint8_t *u8USBrecieveBuffer);


/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{


  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
   HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize debug utils */
  DEBUG_UTILS_INIT();
  
  /* usb  init */
  MX_USB_DEVICE_Init();

  /* Mode selection via USB */
    DeviceMode_t Mode = wait_for_user_select_mode(u8USBrecieveBuffer);

  if ( Mode != AUDIO_IDLE  )
  {
	  start_audio_decimation_app(Mode);
  }

   /*will never go here freeRTOS scheduler will take care of tasks*/
  while (1)
  {
   
  }
}

/**
 * @brief Compare two buffers for equality.
 * @param buffer1 First buffer
 * @param buffer2 Second buffer
 * @param length Length in bytes
 * @retval 0U if equal, 1U if not
 */
static uint8_t uBuffercmp(uint8_t *buffer1, uint8_t *buffer2, uint16_t length)
{
    for (size_t i = 0; i < length; i++) {
        if (buffer1[i] != buffer2[i]) {
            return 1U;
        }
    }
    return 0U;
}

/**
 * @brief Start audio decimation application: create tasks and start scheduler.
 *
 * @param mode Audio processing mode (AUDIO_PLAYBACK, AUDIO_EQUALIZER, ...)
 */
static void start_audio_decimation_app(uint8_t mode)
{
    BaseType_t xReturned = pdFALSE;
    xReturned = xTaskCreate(
        vDecimationTaskRoutine,
        "DECIMATION_TASK",
        300U,
        (void *)mode,
        tskIDLE_PRIORITY + 2U,
        NULL
    );
    if (xReturned != pdPASS)
    {
        Error_Handler();
    }
    xReturned = xTaskCreate(
        vFFT_TaskRoutine,
        "FFT_TASK",
        300U,
        (void *)mode,
        tskIDLE_PRIORITY + 1U,
        NULL
    );
    if (xReturned != pdPASS)
    {
        Error_Handler();
    }
    vTaskStartScheduler();
}

/**
 * @brief Wait for user to select audio mode via USB.
 *
 * @param u8USBrecieveBuffer Pointer to USB receive buffer
 * @return DeviceMode_t Selected mode (AUDIO_PLAYBACK, AUDIO_EQUALIZER, ...)
 */
static DeviceMode_t wait_for_user_select_mode(uint8_t *u8USBrecieveBuffer)
{

    uint8_t u8ModeSelected = 0U;
    DeviceMode_t Mode = AUDIO_IDLE;
    UsbModeState_t usbState = USB_MODE_WAIT;

    while (u8ModeSelected == 0U) 
    {
        switch (usbState)
         {
            case USB_MODE_WAIT:
                // Wait for USB buffer to be filled externally
                usbState = USB_MODE_CHECK;
                break;
            case USB_MODE_CHECK:
                if (0U == uBuffercmp(u8USBrecieveBuffer, "PB_MODE", 7U))
                 {
                    u8ModeSelected = 1U;
                    Mode = AUDIO_PLAYBACK;
                    usbState = USB_MODE_SELECTED;
                }
                 else if (0U == uBuffercmp(u8USBrecieveBuffer, "EQ_MODE", 7U)) 
                {
                    u8ModeSelected = 1U;
                    Mode = AUDIO_EQUALIZER;
                    usbState = USB_MODE_SELECTED;
                } 
                else 
                {
                    // No valid mode yet, keep checking
                    usbState = USB_MODE_WAIT;
                }
                break;
            case USB_MODE_SELECTED:

                break;
            default:
                usbState = USB_MODE_WAIT;
                break;
        }
    }
    return Mode;
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
