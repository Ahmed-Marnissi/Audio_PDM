/*
 * microphone.h
 *
 *  Created on: Apr 12, 2024
 *      Author: 21624
 */

#ifndef MICROPHONE_INC_MICROPHONE_H_
#define MICROPHONE_INC_MICROPHONE_H_

#include "stdint.h"



uint8_t u8Microphone_Init ( void  );
uint8_t u8Microphone_Start ( uint16_t * u16buffer  , uint16_t  datalen  );

uint8_t u8Microphone_RegisterCallback ( void  );

uint8_t u8Microphone_Stop( void  );

#endif /* MICROPHONE_INC_MICROPHONE_H_ */
