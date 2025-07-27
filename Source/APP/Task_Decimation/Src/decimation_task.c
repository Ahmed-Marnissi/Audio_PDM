/*
 * decimation_task.c
 *
 *  Created on: Apr 12, 2024
 *      Author: ahmed marnissi
 */

//=======================[ Includes ]========================//
#include "FreeRTOS.h"
#include "queue.h"
#include "usbd_cdc_if.h"
#include "stm32f4xx_hal.h"
#include "microphone.h"
#include "arm_math.h"
#include "debug.h"
#include "../../../APP/Task_Decimation/Inc/decimation._task.h"
#include "../../../APP/Task_Decimation/Inc/decimation_cfg.h"
#include "../../../APP/Task_Decimation/Inc/fir_coeffes.h"

//=====================[ Macros & Constants ]================//
#define DECIMATION_QUEUE_LENGTH     DECMATION_QUEUE_SIZE
#define FFT_QUEUE_LENGTH            10U

//=====================[ Types & Handles ]===================//
typedef enum {
    DECIMATION_IDLE = 0U,
    DECIMATION_COLLECT_SAMPLE,
    DECIMATION_PROCESS,
    DECIMATION_SET_OUTPUT
} decimation_app_state_t;

typedef struct {
    decimation_app_state_t appState;
    arm_fir_decimate_instance_f32 decimateInstance;
    float32_t *pDecimationState;
    float32_t *pFirCoeffs;
    uint32_t decimationIterationCounter;
    uint32_t fftCounter;
    uint32_t taskTicks;
    uint32_t taskPeriod_ms;
    uint8_t mode;
} decimation_handle_t;

//=====================[ Global Handle & Buffers ]===========//
static float32_t decimationStateArray[FIR_LENGTH + PDM_BITS_SIZE - 1];
static uint16_t u16PDM_Buffer[PDM_BUFFER_SIZE] = {0};
static uint16_t u16IntermidatePDMBuf[PDM_BUFFER_SIZE] = {0};
static float32_t f32PDM_BITS_Buffer[PDM_BITS_SIZE] = {0};
static float32_t f32PCM_Buffer[PCM_BUFFER_SIZE] = {0};
static float32_t f32FFT_Buffer[FFT_SIZE] = {0};
static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
static decimation_handle_t decimation_handle;

QueueHandle_t xDecimationQueue ;
QueueHandle_t xFFTQueue ;

//=====================[ Function Prototypes ]===============//
static void expandPDMbuffer(const uint16_t *inBuffer, float32_t *outBuffer);
uint8_t vDecimationInit(decimation_handle_t *handle);
uint8_t vDecimationProcess(decimation_handle_t *handle);

//=====================[ Callbacks ]========================//
void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s);
void vDecimationTaskRoutine(void *pvParameters);

//=====================[ Interrupt Callback ]=================//
static void vPDM_Rx_callback()
{
    DEBUG_PIN_HW_ISR_TOGGLE();
    xQueueSendFromISR( xDecimationQueue, &u16PDM_Buffer, &xHigherPriorityTaskWoken);
}

void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s)
{
    vPDM_Rx_callback();
}

//=====================[ Decimation Task Routine ]============//
void vDecimationTaskRoutine(void *pvParameters)
{
    UNUSED(pvParameters);
    uint8_t ret = 0U;

    ret = vDecimationInit(&decimation_handle);
    if (ret != 0U)
    {
        ERROR_HANDLER();
    }

    while (1)
    {
        DEBUG_PIN_TASK_DECIMATION_TOGGLE();
        vDecimationProcess(&decimation_handle);
        DEBUG_PIN_TASK_DECIMATION_TOGGLE();
        vTaskDelay(decimation_handle.taskPeriod_ms);
    }
}

/**
 * @brief Initializes the decimation handle, queues, hardware, and filter.
 *
 * Sets up the decimation state machine, allocates queues, and initializes hardware and filter configuration.
 *
 * @param[in,out] handle Pointer to the decimation handle struct
 * @retval 0U on success, 1U if handle is NULL, 2U if queue allocation fails
 */
uint8_t vDecimationInit(decimation_handle_t *handle)
{
    uint8_t ret = 0U;
    if (handle == NULL)
        return 1U;

    handle->appState = DECIMATION_IDLE;
    handle->pDecimationState = decimationStateArray;
    handle->pFirCoeffs = fir_coeff_arrary;
    handle->decimationIterationCounter = 0U;
    handle->fftCounter = 0U;
    handle->taskTicks = 0U;
    handle->taskPeriod_ms = 1U;
    handle->mode = 0U;
    /* Create queues */
    xDecimationQueue = xQueueCreate(DECMATION_QUEUE_SIZE , sizeof(uint16_t) * PDM_BUFFER_SIZE);
    xFFTQueue = xQueueCreate(FFT_QUEUE_LENGTH, sizeof(float32_t) * FFT_SIZE);
    /* Optionally create DTMF queue if needed */
    /* xDTMFQueue = xQueueCreate(DTMF_QUEUE_SIZE, sizeof(float32_t) * DTMF_SIZE); */

    if ((xDecimationQueue == NULL) || (xFFTQueue == NULL))
    {
        return 2U;
    }

    /* Initialize hardware and filter */
    ret += u8Microphone_Init();
    ret += u8Microphone_Start(u16PDM_Buffer, PDM_BUFFER_SIZE);
    arm_fir_decimate_init_f32(&handle->decimateInstance, FIR_LENGTH, DECIMATOR, handle->pFirCoeffs, handle->pDecimationState, PDM_BITS_SIZE);

    return ret;
}


/**
 * @brief Executes the decimation state machine for one cycle.
 *
 * Handles sample collection, decimation filtering, and output queueing.
 *
 * @param[in,out] handle Pointer to the decimation handle struct
 * @retval 0U on success, 1U if handle is NULL
 */
uint8_t vDecimationProcess(decimation_handle_t *handle)
{
    if (handle == NULL)
        return 1U;

    switch (handle->appState)
    {
        case DECIMATION_IDLE:
            handle->appState = DECIMATION_COLLECT_SAMPLE;
            break;
        case DECIMATION_COLLECT_SAMPLE:
            if (xQueueReceive(xDecimationQueue, u16IntermidatePDMBuf, 0U) == pdPASS)
            {
                handle->appState = DECIMATION_PROCESS;
            }
            else
            {
                handle->appState = DECIMATION_IDLE;
            }
            break;
        case DECIMATION_PROCESS:
            expandPDMbuffer(u16IntermidatePDMBuf, f32PDM_BITS_Buffer);
            arm_fir_decimate_f32(&handle->decimateInstance, f32PDM_BITS_Buffer, f32PCM_Buffer, PDM_BITS_SIZE);
            if (handle->fftCounter == FFT_SIZE)
            {
                handle->fftCounter = 0U;
                handle->appState = DECIMATION_SET_OUTPUT;
            }
            else
            {
                memcpy(&f32FFT_Buffer[handle->fftCounter], f32PCM_Buffer, sizeof(float32_t) * PCM_BUFFER_SIZE);
                handle->fftCounter += PCM_BUFFER_SIZE;
                handle->appState = DECIMATION_IDLE;
            }
            break;
        case DECIMATION_SET_OUTPUT:
            xQueueSend(xFFTQueue, (void *)f32FFT_Buffer, 0);
            handle->appState = DECIMATION_IDLE;
            break;
        default:
            handle->appState = DECIMATION_IDLE;
            break;
    }
    handle->taskTicks++;
    return 0U;
}

//=====================[ PDM Buffer Expansion ]===============//
/**
 * @brief Expands a PDM buffer of uint16_t words into a float32_t bit buffer for further DSP processing.
 *
 * Each bit from each uint16_t word is moved to an element of the output array.
 *
 * @param[in]  inBuffer   Pointer to the input PDM buffer (uint16_t array)
 * @param[out] outBuffer  Pointer to the output float32_t buffer (bit-expanded)
 */
static void expandPDMbuffer(const uint16_t *inBuffer, float32_t *outBuffer)
{
    uint32_t bitIndex = 0;
    uint16_t modifiedCurrentWord = 0;

    memset(outBuffer, 0, sizeof(float32_t) * PDM_BITS_SIZE);

    /* Move each bit from each uint16_t word to an element of output array */
    for (bitIndex = 0; bitIndex < PDM_BITS_SIZE; bitIndex++)
    {
        if (bitIndex % 16 == 0)
        {
            modifiedCurrentWord = inBuffer[bitIndex / 16];
        }

        if (modifiedCurrentWord & 0x8000)
        {
            outBuffer[bitIndex] = INT16_MAX;
        }
        else
        {
            outBuffer[bitIndex] = INT16_MIN;
        }

        modifiedCurrentWord <<= 1;
    }
}

