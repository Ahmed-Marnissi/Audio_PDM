/*
 * decimation_task.c
 *
 *  Created on: Apr 12, 2024
 *      Author: 21624
 */

#include "decimation._task.h"
#include "FreeRTOS.h"
#include "queue.h"


#include "usbd_cdc_if.h"

#include "stm32f4xx_hal.h"

#include "microphone.h"

#include "arm_math.h"

#include "fir_coeffes.h"

#include "decimation_cfg.h"

#include "debug.h"
arm_fir_decimate_instance_f32 DecimateInstance ;

float32_t DecimateState[ FIR_LENGTH + PDM_BITS_SIZE  -1 ]; // numTaps+blockSize-1


BaseType_t xHigherPriorityTaskWoken = pdFALSE ;

QueueHandle_t xDecimationQueue ;
QueueHandle_t xFFTQueue ;

uint16_t u16PDM_Buffer [ PDM_BUFFER_SIZE ];

uint16_t  u16IntermidatePDMBuf[ PDM_BUFFER_SIZE  ];

float32_t  f32PDM_BITS_Buffer[PDM_BITS_SIZE ] ;

float32_t  f32PCM_Buffer [PCM_BUFFER_SIZE  ];

float32_t f32FFT_Buffer [FFT_SIZE ];

uint16_t u16CounterFFT = 0U ;
static void expandPDMbuffer( const uint16_t *inBuffer , float32_t *outBuffer   );

void HAL_I2S_RxCpltCallback (I2S_HandleTypeDef *hi2s )
{
	DEBUG_PIN_HW_ISR_TOGGLE() ;
	xQueueSendFromISR( xDecimationQueue , & u16PDM_Buffer , &xHigherPriorityTaskWoken );

}



void vDecimationTaskRoutine( void * pvParameters )
{
	uint32_t Mode =   pvParameters   ;
	/* create decimation  queue */

	xDecimationQueue =  xQueueCreate( DECMATION_QUEUE_SIZE , sizeof(  uint16_t  ) * PDM_BUFFER_SIZE   ) ;
	/* Create  FFT queue */
	xFFTQueue =  xQueueCreate( 10U , sizeof(  float32_t  ) * FFT_SIZE   ) ;

	/* Init  arm  CMSIS  Deciamtion filter  */
	arm_fir_decimate_init_f32	(&DecimateInstance  , FIR_LENGTH , DECIMATOR , 	fir_coeff_arrary , DecimateState , PDM_BITS_SIZE );


	/* Microphone Init  */
	u8Microphone_Init ();

	/* Microphone Start */
	u8Microphone_Start(  u16PDM_Buffer ,  PDM_BUFFER_SIZE  );

    while ( 1 )
    {
        /* Task code goes here. */
    	DEBUG_PIN_TASK_DECIMATION_TOGGLE();
    	if (  xQueueReceive( xDecimationQueue , u16IntermidatePDMBuf , 0U  ) == pdPASS  )
    	{

    		/* expand pdm u16buffer to  float  buffer ( bits ) */

    		expandPDMbuffer( u16IntermidatePDMBuf  ,  f32PDM_BITS_Buffer  ) ;

    		/* process ar decimation filter */
    		arm_fir_decimate_f32(&DecimateInstance , f32PDM_BITS_Buffer , f32PCM_Buffer , PDM_BITS_SIZE  );

    		/* Send Smaples to  fft Task */
    		if  ( u16CounterFFT == FFT_SIZE  )
    		{
    			xQueueSend( xFFTQueue , ( void * )f32FFT_Buffer ,0  );
    			u16CounterFFT =0U ;

    		}
    		else
    		{
    			memcpy( &f32FFT_Buffer[ u16CounterFFT ] , f32PCM_Buffer ,sizeof(float32_t)* PCM_BUFFER_SIZE );
    			u16CounterFFT = u16CounterFFT + PCM_BUFFER_SIZE ;

    		}
#define TEST_RECORD
#ifdef TEST_RECORD
    		/* send samples via usb */
    		if ( Mode == 1U )
    		{
    		 CDC_Transmit_FS( (uint8_t * ) f32PCM_Buffer, PCM_BUFFER_SIZE  * sizeof(float));

    		}
#endif
    	}

    	DEBUG_PIN_TASK_DECIMATION_TOGGLE();

    	vTaskDelay(  1U  ) ;

    }
}




static void expandPDMbuffer( const uint16_t *inBuffer , float32_t *outBuffer   )
{
    uint32_t bitIndex = 0;
    uint16_t modifiedCurrentWord = 0;

    memset(outBuffer, 0, sizeof(PDM_BITS_SIZE ));

    /* Move each bit from each uint16_t word to an element of output array. */
    for(bitIndex=0;
        bitIndex < PDM_BITS_SIZE ;
        bitIndex++)
    {
        if (bitIndex % 16 == 0)
        {
            modifiedCurrentWord = inBuffer[bitIndex/16];
        }

        if (modifiedCurrentWord & 0x8000)
        {
            outBuffer[bitIndex] = INT16_MAX;
        }
        else
        {
            outBuffer[bitIndex] = INT16_MIN;
        }

        modifiedCurrentWord = modifiedCurrentWord << 1;
    }
}



