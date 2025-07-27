/*
 * task_fft.c
 *
 *  Created on: May 14, 2024
 *      Author: 21624
 */


#include "FreeRTOS.h"
#include "queue.h"


#include "stm32f407xx.h"

#include "arm_math.h"

#include "queue.h"
#include "debug.h"
#include "../../../APP/Task_Decimation/Inc/decimation_cfg.h"
#include "../../../APP/Task_fft/Inc/fft_task.h"
#include "usbd_cdc_if.h"



/**
 * @brief FFT task states
 */
typedef enum {
    FFT_IDLE = 0U,
    FFT_COLLECT_SAMPLE,
    FFT_PROCESS,
    FFT_SET_OUTPUT
} fft_app_state_t;

/**
 * @brief FFT handle struct to encapsulate state/config
 */
typedef struct {
    fft_app_state_t appState;
    arm_cfft_radix4_instance_f32 fftInstance;
    arm_status status;
    uint32_t fftCounter;
    uint32_t taskTicks;
    uint32_t taskPeriod_ms;
    uint8_t mode;
} fft_handle_t;

//=====================[ Global Buffers/Queues ]==================//
float32_t  InputBuffer[ FFT_SAMPLES ];
float32_t  PCM_Samples [FFT_SIZE];
float32_t  OutputBuffer [FFT_SIZE ];
extern QueueHandle_t xFFTQueue;

//=====================[ Prototypes ]============================//
static uint8_t vFFT_Init(fft_handle_t *handle);
static uint8_t vFFT_Process(fft_handle_t *handle);

/**
 * @brief FFT Task Routine (FreeRTOS entry point)
 * @param pvParameters Task parameters (mode)
 */
void vFFT_TaskRoutine(void *pvParameters)
{
    fft_handle_t fft_handle;
    fft_handle.mode = (uint32_t)pvParameters;
    fft_handle.taskPeriod_ms = 1U;
    fft_handle.fftCounter = 0U;
    fft_handle.taskTicks = 0U;
    fft_handle.appState = FFT_IDLE;

    if (vFFT_Init(&fft_handle) != 0U)
    {
        ERROR_HANDLER();
    }

    while (1)
    {
        DEBUG_PIN_TASK_FFT_TOGGLE();
        vFFT_Process(&fft_handle);
        DEBUG_PIN_TASK_FFT_TOGGLE();
        vTaskDelay(fft_handle.taskPeriod_ms);
    }
}

/**
 * @brief Initializes the FFT handle and CMSIS-DSP instance
 * @param[in,out] handle Pointer to FFT handle struct
 * @retval 0U on success, 1U on failure
 */
static uint8_t vFFT_Init(fft_handle_t *handle)
{
    if (handle == NULL)
        return 1U;
    handle->status = ARM_MATH_SUCCESS;
    handle->status = arm_cfft_radix4_init_f32(&handle->fftInstance, FFT_SIZE, 0, 1);
    return (handle->status == ARM_MATH_SUCCESS) ? 0U : 1U;
}

/**
 * @brief Executes one cycle of the FFT state machine
 * @param[in,out] handle Pointer to FFT handle struct
 * @retval 0U always (could be extended for error handling)
 */
static uint8_t vFFT_Process(fft_handle_t *handle)
{
    switch (handle->appState)
    {
        case FFT_IDLE:
            handle->appState = FFT_COLLECT_SAMPLE;
            break;
        case FFT_COLLECT_SAMPLE:
            if (pdTRUE == xQueueReceive(xFFTQueue, PCM_Samples, 0))
            {
                handle->appState = FFT_PROCESS;
            }
            else
            {
                handle->appState = FFT_IDLE;
            }
            break;
        case FFT_PROCESS:
        {
            /* Add imaginary part */
            uint16_t j = 0;
            for (uint16_t i = 0; i < FFT_SAMPLES; i += 2U)
            {
                InputBuffer[i] = PCM_Samples[j];
                InputBuffer[i + 1] = 0.0f;
                j++;
            }
            /* FFT Process */
            arm_cfft_radix4_f32(&handle->fftInstance, InputBuffer);
            /* Calculate FFT Magnitudes */
            arm_cmplx_mag_f32(InputBuffer, OutputBuffer, FFT_SIZE);
            handle->appState = FFT_SET_OUTPUT;
            break;
        }
        case FFT_SET_OUTPUT:
            if (handle->mode == 2U)
            {
                CDC_Transmit_FS((uint8_t *)OutputBuffer, FFT_SIZE * sizeof(float32_t));
            }
            handle->appState = FFT_IDLE;
            break;
        default:
            handle->appState = FFT_IDLE;
            break;
    }
    handle->taskTicks++;
    return 0U;
}
