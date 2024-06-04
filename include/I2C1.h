#pragma once

#include <stdbool.h>
#include <stdint.h>

void I2C1_Init(void);
void I2C1_MasterStart(void);
void I2C1_MasterStop(void);
void I2C1_MasterAddress(uint8_t addr, bool readBit);
void I2C1_MasterWrite(uint32_t len, const uint8_t data[static len]);
void I2C1_MasterRead(uint32_t len, uint8_t data[len]);