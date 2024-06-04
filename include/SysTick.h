#pragma once

#include <stdint.h>

void SysTick_Init(void);
void SysTick_Delay(uint32_t tick);
uint32_t SysTick_GetStamp(void);