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

arm_fir_decimate_instance_f32 DecimateInstance ;

float32_t DecimateState[ FIR_LENGTH + PDM_BITS_SIZE  -1 ]; // numTaps+blockSize-1


BaseType_t xHigherPriorityTaskWoken = pdFALSE ;

QueueHandle_t xDecimationQueue ;

uint16_t u16PDM_Buffer [ PDM_BUFFER_SIZE ];

uint16_t  u16IntermidatePDMBuf[ PDM_BUFFER_SIZE  ];

float32_t  f32PDM_BITS_Buffer[PDM_BITS_SIZE ] ;

float32_t  f32PCM_Buffer [PCM_BUFFER_SIZE  ];


static void expandPDMbuffer( const uint16_t *inBuffer , float32_t *outBuffer   );

void HAL_I2S_RxCpltCallback (I2S_HandleTypeDef *hi2s)
{
	 xQueueSendFromISR( xDecimationQueue , & u16PDM_Buffer , &xHigherPriorityTaskWoken );

}



void vDecimationTaskRoutine( void * pvParameters )
{
	/* create decimation  queue */

	xDecimationQueue =  xQueueCreate( DECMATION_QUEUE_SIZE , sizeof(  uint16_t  ) * PDM_BUFFER_SIZE   ) ;

	/* Init  arm  CIMSIS  Deciamtion filter  */
	arm_fir_decimate_init_f32	(&DecimateInstance  , FIR_LENGTH , DECIMATOR , 	fir_coeff_arrary , DecimateState , PDM_BITS_SIZE );


	/* Microphone Init  */
	u8Microphone_Init ();

	/* Microphone Start */
	u8Microphone_Start(  u16PDM_Buffer ,  PDM_BUFFER_SIZE  );


    while ( 1 )
    {
        /* Task code goes here. */

    	if (  xQueueReceive( xDecimationQueue , u16IntermidatePDMBuf , 0U  ) == pdPASS  )
    	{

    		/* expand pdm u16buffer to  float  buffer ( bits ) */

    		expandPDMbuffer( u16IntermidatePDMBuf  ,  f32PDM_BITS_Buffer  ) ;

    		/* process ar decimation filter */
    		arm_fir_decimate_f32(&DecimateInstance , f32PDM_BITS_Buffer , f32PCM_Buffer , PDM_BITS_SIZE  );

    		/* send samples via usb */

    		 CDC_Transmit_FS( (uint8_t * ) f32PCM_Buffer, PCM_BUFFER_SIZE  * sizeof(float));

    	}


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



