/**
 * @file decimation._task.h
 * @brief Decimation task header file
 *
 * This file contains the function prototype for the decimation task.
 *
 * @author 21624
 * @date April 12, 2024
 */

#ifndef TASK_DECIMATION_DECIMATION__TASK_H_
#define TASK_DECIMATION_DECIMATION__TASK_H_

typedef enum
{
	AUDIO_IDLE = 0U ,
	AUDIO_PLAYBACK =1U ,
	AUDIO_EQUALIZER =2U,

}DeviceMode_t ;

extern DeviceMode_t mode ;

/**
 * @brief Decimation task function
 *
 * This function is the entry point for the decimation task.
 *
 * @param pvParameters task parameters
 */
void vDecimationTaskRoutine( void * pvParameters );

#endif /* TASK_DECIMATION_DECIMATION__TASK_H_ */
