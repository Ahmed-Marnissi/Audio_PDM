/**
 * @file main.c
 * @brief Application entry point and startup
 *
 * This file initializes the MCU, configures clocks, selects audio mode via USB,
 * and launches the audio application tasks.
 */
/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include "usb_device.h"
#include "FreeRTOS.h"
#include "task.h"
#include "debug.h"
#include "../../APP/Task_Decimation/Inc/decimation._task.h"
#include "../../APP/Task_fft/Inc/fft_task.h"


/* ------------------ typedef -----------------------------------------------------------*/

typedef enum
{
    USB_MODE_WAIT = 0U,
    USB_MODE_CHECK,
    USB_MODE_SELECTED
} UsbModeState_t;


typedef enum
{
	LAUNCH_STATE_IDLE = 0U,
	LAUNCH_STATE_CREATE_DECIMATION_TASK,
	LAUNCH_STATE_CREATE_FFT_TASK,
	LAUNCH_STATE_START_SYSTEM,
	LAUNCH_STATE_EXIT
} launch_app_state_t;

/* ------------------ define ------------------------------------------------------------*/
/* -------------- variables ---------------------------------------------------------*/
uint8_t u8USBrecieveBuffer[64];
launch_app_state_t launch_app_state = LAUNCH_STATE_IDLE;
/* ----------------- function prototypes -----------------------------------------------*/
uint8_t  SystemClock_Config(void);
static uint8_t buffer_compare(uint8_t *buffer1, uint8_t *buffer2, uint16_t length);
static DeviceMode_t wait_for_user_select_mode(uint8_t *u8USBrecieveBuffer);
static uint8_t Launch_Audio_Application();


/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

	uint8_t ret = 0U;

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
   HAL_Init();

  /* Configure the system clock */
  ret = SystemClock_Config();
  if (ret != 0U)
  {
	  LOG_ERROR("Failed to Init System Clock");
	  ERROR_HANDLER ();
  }


  /* Initialize debug utils */
  DEBUG_UTILS_INIT();
  
  /* usb  init */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  ret = USB_DEVICE_Init();
  if (ret != 0U)
  {
	  LOG_ERROR("Failed to Init USB Device ");
	  ERROR_HANDLER ();
  }

  /* Mode selection via USB */
  mode = wait_for_user_select_mode(u8USBrecieveBuffer);

  /* Launch Audio Application */
  ret = Launch_Audio_Application();
  if (ret != 0U)
  {
	  LOG_ERROR("Failed to launch Audio Application");
	  ERROR_HANDLER ();
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
static uint8_t buffer_compare(uint8_t *buffer1, uint8_t *buffer2, uint16_t length)
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
				/* Wait for USB buffer to be filled externally */
				usbState = USB_MODE_CHECK;
				break;
			case USB_MODE_CHECK:
				if (0U == buffer_compare(u8USBrecieveBuffer, (uint8_t *)"PB_MODE", 7U))
				{
					u8ModeSelected = 1U;
					Mode = AUDIO_PLAYBACK;
					usbState = USB_MODE_SELECTED;
				}
				else if (0U == buffer_compare(u8USBrecieveBuffer, (uint8_t *)"EQ_MODE", 7U))
				{
					u8ModeSelected = 1U;
					Mode = AUDIO_EQUALIZER;
					usbState = USB_MODE_SELECTED;
				}
				else
				{
					/* No valid mode yet, keep checking */
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
 * @brief Create and launch the Audio application tasks
 *
 * @return uint8_t 0U on success, non-zero on error
 */
static uint8_t Launch_Audio_Application()
{
	BaseType_t xReturned = pdFALSE;
	uint8_t result = 0U;


	do
	{
		switch (launch_app_state)
		{
			case LAUNCH_STATE_IDLE:
			{
				launch_app_state = LAUNCH_STATE_CREATE_DECIMATION_TASK;
				break;
			}

			case LAUNCH_STATE_CREATE_DECIMATION_TASK:
			{
				xReturned = xTaskCreate(
					vDecimationTaskRoutine,
					"DECIMATION_TASK",
					300U,
					NULL ,
					tskIDLE_PRIORITY + 2U,
					NULL);
				if (xReturned == pdPASS)
				{
					if (mode == AUDIO_EQUALIZER )
					{
						launch_app_state = LAUNCH_STATE_CREATE_FFT_TASK;
					}
					else
					{
						launch_app_state = LAUNCH_STATE_START_SYSTEM;
					}
				}
				else
				{
					launch_app_state = LAUNCH_STATE_EXIT;
					result = 1U;
				}
				break;
			}

			case LAUNCH_STATE_CREATE_FFT_TASK:
			{
				xReturned = xTaskCreate(
					vFFT_TaskRoutine,
					"FFT_TASK",
					300U,
					NULL ,
					tskIDLE_PRIORITY + 1U,
					NULL);
				if (xReturned == pdPASS)
				{
					launch_app_state = LAUNCH_STATE_START_SYSTEM;
				}
				else
				{
					launch_app_state = LAUNCH_STATE_EXIT;
					result = 1U;
				}
				break;
			}

			case LAUNCH_STATE_START_SYSTEM:
			{
				LOG_INFO("Starting Audio Application.....\n");
				vTaskStartScheduler();
				break;
			}

			default:
				result = 1U;
				launch_app_state = LAUNCH_STATE_EXIT;
				break;
		}
	} while (launch_app_state != LAUNCH_STATE_EXIT);

	return result;
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
uint8_t  SystemClock_Config(void)
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
	  return 1U ;
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
	  return 1U ;
  }
  return 0U ;
}


void Error_Handler(void)
{
	while(1);
}


