#pragma once

#include <stdint.h>

void AccelSensor_Init(void);
void AccelSensor_ReadRawCoords(int16_t coords[3]);
void AccelSensor_ReadCalibratedCoords(float coords[3]);