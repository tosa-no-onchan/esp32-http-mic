//#include <Arduino.h>

#ifndef WAV_H_
#define WAV_H_

#include "freertos/FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

// 16bit, monoral, 44100Hz,  linear PCM
void CreateWavHeader(uint8_t *header, int waveDataSize);  // size of header is 44
int CreateWavHeader_chunk0(uint8_t* header, int waveDataSize);
int CreateWavHeader_chunk1(uint8_t* header, int waveDataSize);
#ifdef __cplusplus
};
#endif

#endif /* WAV_H_ */
