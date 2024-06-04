#pragma once

#include <stdint.h>

void MagSensor_Init(void);
void MagSensor_ReadRawCoords(int16_t coords[3]);
void MagSensor_ReadCalibratedCoords(float coords[3]);