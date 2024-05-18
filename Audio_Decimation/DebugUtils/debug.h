/*
 * utils.h
 *
 *  Created on: May 18, 2024
 *      Author: 21624
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include "debug.h"
#include "stm32f4xx_hal.h"
/* GPIO PINS for debug */

#define DEBUG_PINS_GPIO_PORT         GPIOD
#define DEBUG_PIN_TASK_DECIMATION    GPIO_PIN_12
#define DEBUG_PIN_TASK_FFT			 GPIO_PIN_13
#define DEBUG_PIN_HW_ISR             GPIO_PIN_14
#define GPIO_CLK					__HAL_RCC_GPIOD_CLK_ENABLE


#define GENERATE_GPIO_PINS  DEBUG_PIN_HW_ISR | DEBUG_PIN_TASK_DECIMATION | DEBUG_PIN_TASK_FFT

#define DEBUG_UTILS_INIT() \
		do { \
			GPIO_InitTypeDef GPIO_InitStruct = {0}; \
			\
			GPIO_CLK(); 																		\
			/*Configure GPIO pin Output Level */ \
			HAL_GPIO_WritePin(DEBUG_PINS_GPIO_PORT ,GENERATE_GPIO_PINS , GPIO_PIN_RESET); 		\
																								\
			/*Configure GPIO pins */															\
			GPIO_InitStruct.Pin = GENERATE_GPIO_PINS ; 											\
			GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;						 						\
			GPIO_InitStruct.Pull = GPIO_NOPULL;								\
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;						\
			HAL_GPIO_Init(DEBUG_PINS_GPIO_PORT , &GPIO_InitStruct); 							\
		}while ( 0U ) ; 																\



#define DEBUG_PIN_TASK_FFT_TOGGLE() \
	do \
	{		\
		HAL_GPIO_TogglePin( DEBUG_PINS_GPIO_PORT ,  DEBUG_PIN_TASK_FFT); \
	}while ( 0U);


#define DEBUG_PIN_TASK_DECIMATION_TOGGLE( ) \
	do \
	{		\
		HAL_GPIO_TogglePin( DEBUG_PINS_GPIO_PORT ,  DEBUG_PIN_TASK_DECIMATION); \
	}while ( 0U);

#define DEBUG_PIN_HW_ISR_TOGGLE( ) \
	do \
	{		\
		HAL_GPIO_TogglePin( DEBUG_PINS_GPIO_PORT ,  DEBUG_PIN_HW_ISR); \
	}while ( 0U);


/* Timers Debug */


#define DEBUG_TOGGLE_PIN_DECIATION_TASK

#endif /* DEBUG_H_ */
