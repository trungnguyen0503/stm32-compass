#include "SysTick.h"

#include "stm32f103xb.h"

volatile uint32_t tickCounter = 0;

#define TICK_INTERVAL_MS 1

void SysTick_Init(void) {
    SysTick->LOAD = F_CPU / 1000 * TICK_INTERVAL_MS;
    SysTick->CTRL |=
            SysTick_CTRL_ENABLE_Msk |
            SysTick_CTRL_CLKSOURCE_Msk |
            SysTick_CTRL_TICKINT_Msk;
}

void SysTick_Delay(uint32_t tick) {
    uint32_t start = tickCounter;
    while (tickCounter - start < tick);
}

uint32_t SysTick_GetStamp(void) {
    return tickCounter;
}

void SysTick_Handler(void) {
    tickCounter++;
}