/**
 * @file microphone.h
 * @brief Microphone hardware abstraction API.
 *
 * Provides initialization, start, stop, and callback registration for the microphone interface.
 *
 * @author ahmed marnissi
 * @date April 12, 2024
 */

#ifndef MICROPHONE_INC_MICROPHONE_H_
#define MICROPHONE_INC_MICROPHONE_H_

#include "stdint.h"

/**
 * @brief Initialize microphone hardware.
 * @retval 0U on success, error code otherwise
 */
uint8_t u8Microphone_Init ( void  );

/**
 * @brief Start microphone DMA acquisition.
 * @param u16buffer Pointer to buffer for PDM data
 * @param datalen Number of PDM samples
 * @retval 0U on success, error code otherwise
 */
uint8_t u8Microphone_Start ( uint16_t * u16buffer  , uint16_t  datalen  );

/**
 * @brief Register microphone callback (if supported).
 * @retval 0U on success, error code otherwise
 */
uint8_t u8Microphone_RegisterCallback ( void  );

/**
 * @brief Stop microphone acquisition.
 * @retval 0U on success, error code otherwise
 */
uint8_t u8Microphone_Stop( void  );

#endif /* MICROPHONE_INC_MICROPHONE_H_ */
