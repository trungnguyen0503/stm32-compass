#include "LedCtrl.h"

#include "stm32f103xb.h"

#define TIM_2_4_CLK_SPEED F_CPU
#define PRESCALE_2000HZ (TIM_2_4_CLK_SPEED / 2000 - 1)
#define PRESCALE_1000HZ (TIM_2_4_CLK_SPEED / 1000 - 1)
#define AUTO_RELOAD 1000

uint32_t ledPBx = 1 << 10;

void LedCtrl_Init(void) {
    // Clocks for TIM2 and switch pins and led pins
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

    // Setup led PB10, PB11 as output push pull
    GPIOB->CRH &= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10 | GPIO_CRH_MODE11 | GPIO_CRH_CNF11);
    GPIOB->CRH |=
            (0b01 << GPIO_CRH_MODE10_Pos) | (0b00 << GPIO_CRH_CNF10_Pos) |
            (0b01 << GPIO_CRH_MODE11_Pos) | (0b00 << GPIO_CRH_CNF11_Pos);

    // Timer settings
    TIM2->PSC = PRESCALE_1000HZ;
    TIM2->ARR = AUTO_RELOAD;
    TIM2->DIER |= TIM_DIER_UIE;
    TIM2->CR1 |= TIM_CR1_CEN;
    NVIC_EnableIRQ(TIM2_IRQn);
}

void LedCtrl_BlinkRed(void) {
    GPIOB->BRR |= ledPBx;
    ledPBx = 1 << 10;
    GPIOB->BSRR |= ledPBx;
    TIM2->PSC = PRESCALE_1000HZ;
}

void LedCtrl_BlinkGreen(void) {
    GPIOB->BRR |= ledPBx;
    ledPBx = 1 << 11;
    GPIOB->BSRR |= ledPBx;
    TIM2->PSC = PRESCALE_2000HZ;
}

void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~TIM_SR_UIF;
        if (GPIOB->ODR & ledPBx) {
            GPIOB->BRR |= ledPBx;
        } else {
            GPIOB->BSRR |= ledPBx;
        }
    }
}