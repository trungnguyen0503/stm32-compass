#include "StateCtrl.h"
#include "SysTick.h"

#include "stm32f103xb.h"

static bool isPrevOn = false;
static bool isOn = false;

static bool prevToggleActiveBtnState = true;
static bool toggleActiveBtnState = true;

void StateCtrl_Init(void) {
    // Enable clock for switches and AFIO for external interrupt
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;

    // Set PB0, PB1 as input pullup
    GPIOB->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0 | GPIO_CRL_MODE1 | GPIO_CRL_CNF1);
    GPIOB->CRL |=
            (0b00 << GPIO_CRL_MODE0_Pos) | (0b10 << GPIO_CRL_CNF0_Pos) |
            (0b00 << GPIO_CRL_MODE1_Pos) | (0b10 << GPIO_CRL_CNF1_Pos);
    GPIOB->BSRR |= GPIO_BSRR_BS0 | GPIO_BSRR_BS1;

    // Use external interrupt on switch 2
    AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI1_PB;
    EXTI->FTSR |= EXTI_FTSR_FT1;
    EXTI->IMR |= EXTI_IMR_IM1;
    NVIC_EnableIRQ(EXTI1_IRQn);
}

void StateCtrl_Update(void) {
    isPrevOn = isOn;
    prevToggleActiveBtnState = toggleActiveBtnState;

    toggleActiveBtnState = GPIOB->IDR & (1 << 0);
    if (prevToggleActiveBtnState == true && toggleActiveBtnState == false) {
        SysTick_Delay(50);
        toggleActiveBtnState = GPIOB->IDR & (1 << 0);
        if (toggleActiveBtnState == false) {
            isOn = !isOn;
        }
    }
}

bool StateCtrl_IsPrevOn(void) {
    return isPrevOn;
}

bool StateCtrl_IsOn(void) {
    return isOn;
}

void EXTI1_IRQHandler(void) {
    NVIC_SystemReset();
}