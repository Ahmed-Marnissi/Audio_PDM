/*
 * task_fft.c
 *
 *  Created on: May 14, 2024
 *      Author: 21624
 */


#include "fft_task.h"
#include "decimation_cfg.h"

#include "FreeRTOS.h"
#include "queue.h"


#include "stm32f407xx.h"

#include "arm_math.h"

#include "queue.h"
#include "debug.h"


arm_cfft_radix4_instance_f32 FFT_Instance ;

float32_t  InputBuffer[ FFT_SAMPLES ];
float32_t  PCM_Samples [FFT_SIZE];
float32_t  OutputBuffer [FFT_SIZE ];


extern QueueHandle_t xFFTQueue ;

void vFFT_TaskRoutine( void * pvParameters )
{
	uint32_t Mode =   pvParameters   ;
	/*01 : Init FFT  */
	arm_status status = ARM_MATH_SUCCESS ;
	status = arm_cfft_radix4_init_f32	( &FFT_Instance ,  FFT_SIZE , 0 ,1 )	;

	while ( 1)
	{
		/*01  Get FFT Input Buffer  */
		DEBUG_PIN_TASK_FFT_TOGGLE();
		if (pdTRUE == xQueueReceive(xFFTQueue , PCM_Samples , 0 ) )
			{
			/* add imaginary part  */
			uint16_t j = 0;
			for( uint16_t i = 0; i < FFT_SAMPLES ; i+= 2U )
			{
				InputBuffer[ i ]= PCM_Samples [ j ];
				InputBuffer[ i +1  ]= 0.0f;
				j++ ;

			}
			/*01  FFT Process  */
			arm_cfft_radix4_f32	(	&FFT_Instance,  InputBuffer )	;

			/*01  Calculate FFT Magintudes  */
			 arm_cmplx_mag_f32(InputBuffer, OutputBuffer, FFT_SIZE);

			/*01  Send FFF Results Via USB  */
	    		if ( Mode == 2U )
	    		{
	    		 CDC_Transmit_FS( (uint8_t * ) OutputBuffer, FFT_SIZE  * sizeof( float32_t ));
	    		}
			}
		DEBUG_PIN_TASK_FFT_TOGGLE();
		/*01  Task  Periode */
		vTaskDelay( 1U );

	}
}
