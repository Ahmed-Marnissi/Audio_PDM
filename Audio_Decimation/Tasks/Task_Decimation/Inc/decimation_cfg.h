/*
 * decimation_cfg.h
 *
 *  Created on: Apr 12, 2024
 *      Author: 21624
 */

#ifndef TASK_DECIMATION_INC_DECIMATION_CFG_H_
#define TASK_DECIMATION_INC_DECIMATION_CFG_H_

/* Configuration  */
#define MICROPHONE_SAMPLING_FREQ_KH  1024U //khz

#define  DECIMATOR  64U

#define HARDWARE_PERIODE_MS 1U

#define PDM_BITS_SIZE    MICROPHONE_SAMPLING_FREQ_KH * HARDWARE_PERIODE_MS

#define PDM_BUFFER_SIZE  PDM_BITS_SIZE / 16U

#define PCM_BUFFER_SIZE PDM_BITS_SIZE / DECIMATOR

#define FIR_LENGTH  PDM_BITS_SIZE

#define DECMATION_QUEUE_SIZE 4U

#define DECIMATION_TASK_PERIODE 1U

/* FFT defines */



#define FFT_SAMPLES                        512
#define FFT_SIZE                FFT_SAMPLES / 2 //res 31.5hz

#endif /* TASK_DECIMATION_INC_DECIMATION_CFG_H_ */
