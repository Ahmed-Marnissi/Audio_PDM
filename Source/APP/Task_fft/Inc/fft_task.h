/**
 * @file fft_task.h
 * @brief FFT task header file.
 *
 * Declares the FreeRTOS FFT task routine.
 *
 * @author ahmed marnissi
 * @date May 14, 2024
 */

#ifndef TASK_FFT_FFT_TASK_H_
#define TASK_FFT_FFT_TASK_H_

#include "stdint.h"

/**
 * @brief FFT task function (FreeRTOS entry point)
 *
 * @param pvParameters Task parameters
 */
void vFFT_TaskRoutine( void * pvParameters );

#endif /* TASK_FFT_FFT_TASK_H_ */
