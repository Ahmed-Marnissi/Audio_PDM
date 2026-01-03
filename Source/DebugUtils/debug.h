/*
 * utils.h
 *
 *  Created on: May 18, 2024
 *      Author: Ahmed Marnissi
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include "debug.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>

/* GPIO PINS for debug */


#ifdef DEBUG
    #define ERROR_HANDLER() __BKPT(0)
#else
    #define ERROR_HANDLER() NVIC_SystemReset()
#endif
#define DEBUG_PINS_GPIO_PORT         GPIOD
#define DEBUG_PIN_TASK_DECIMATION    GPIO_PIN_12
#define DEBUG_PIN_TASK_FFT			 GPIO_PIN_13
#define DEBUG_PIN_HW_ISR             GPIO_PIN_14
#define GPIO_CLK					__HAL_RCC_GPIOD_CLK_ENABLE


#define GENERATE_GPIO_PINS  DEBUG_PIN_HW_ISR | DEBUG_PIN_TASK_DECIMATION | DEBUG_PIN_TASK_FFT

#define DEBUG_UTILS_INIT() \
		do { \
		GPIO_InitTypeDef GPIO_InitStruct = {0}; 									\
																						\
			GPIO_CLK(); 																\
			/*Configure GPIO pin Output Level */ 										\
		HAL_GPIO_WritePin(DEBUG_PINS_GPIO_PORT ,GENERATE_GPIO_PINS , GPIO_PIN_RESET); 	\
																						\
		/*Configure GPIO pins */														\
		GPIO_InitStruct.Pin = GENERATE_GPIO_PINS ; 										\
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;						 				\
		GPIO_InitStruct.Pull = GPIO_NOPULL;												\
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;									\
		HAL_GPIO_Init(DEBUG_PINS_GPIO_PORT , &GPIO_InitStruct); 						\
		CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;									\
																						\
		ITM->LAR  = 0xC5ACCE55;         												\
		ITM->TCR  = ITM_TCR_ITMENA_Msk         | ITM_TCR_TSENA_Msk	| ITM_TCR_SWOENA_Msk;\
		ITM->TER  = 1;  																\
	}while ( 0U ) ; 																	\









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

/* ---------- Configuration ---------- */
// 0=OFF, 1=ERROR, 2=WARNING, 3=INFO, 4=DEBUG
#ifndef LOG_LEVEL
#define LOG_LEVEL 4
#endif

/* ---------- Timestamp ---------- */
#define LOG_TIME_MS()   ((unsigned long)HAL_GetTick())

/* ---------- Internal print ---------- */
#define LOG__PRINT(tag, fmt, ...) \
    do { \
        printf("[%lu] [%s] " fmt "\r\n", \
               LOG_TIME_MS(), tag, ##__VA_ARGS__); \
    } while (0)

/* ---------- Public macros ---------- */
#if (LOG_LEVEL >= 1)
  #define LOG_ERROR(fmt, ...)   LOG__PRINT("Error", fmt, ##__VA_ARGS__)
#else
  #define LOG_ERROR(...)        do {} while (0)
#endif

#if (LOG_LEVEL >= 2)
  #define LOG_WARNING(fmt, ...) LOG__PRINT("Warning", fmt, ##__VA_ARGS__)
#else
  #define LOG_WARNING(...)      do {} while (0)
#endif

#if (LOG_LEVEL >= 3)
  #define LOG_INFO(fmt, ...)    LOG__PRINT("Info", fmt, ##__VA_ARGS__)
#else
  #define LOG_INFO(...)         do {} while (0)
#endif

#if (LOG_LEVEL >= 4)
  #define LOG_DEBUG(fmt, ...)   LOG__PRINT("Debug", fmt, ##__VA_ARGS__)
#else
  #define LOG_DEBUG(...)        do {} while (0)
#endif
#endif /* DEBUG_H_ */
