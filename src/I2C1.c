#include "I2C1.h"
#include "stm32f103xb.h"

#define SCL_CLK_SPEED 300000
#define PCLK1_SPEED F_CPU

void I2C1_Init(void) {
    // Enable PB and I2C1 clocks
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

    // Set PB6, PB7 as alternate open drain output
    GPIOB->CRL &= ~(GPIO_CRL_MODE6 | GPIO_CRL_CNF6 | GPIO_CRL_MODE7 | GPIO_CRL_CNF7);
    GPIOB->CRL |=
            (0b01 << GPIO_CRL_MODE6_Pos) | (0b11 << GPIO_CRL_CNF6_Pos) |
            (0b01 << GPIO_CRL_MODE7_Pos) | (0b11 << GPIO_CRL_CNF7_Pos);

    // Reset I2C1 bus
    RCC->APB1RSTR |= RCC_APB1RSTR_I2C1RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C1RST;
    I2C1->CR1 |= I2C_CR1_SWRST;
    I2C1->CR1 &= ~I2C_CR1_SWRST;

    // I2C1 config
    I2C1->CR2 |= (PCLK1_SPEED / 1000000) << I2C_CR2_FREQ_Pos; // system clock speed
    I2C1->CCR |=
            ((PCLK1_SPEED / 2 / SCL_CLK_SPEED) << I2C_CCR_CCR_Pos) | // scl clock speed
            I2C_CCR_FS;                                              // fast mode
    I2C1->TRISE = F_CPU / 2 / SCL_CLK_SPEED + 1;                     // maximum rise time

    // Enable I2C1
    I2C1->CR1 |= I2C_CR1_PE;
}

void I2C1_MasterStart(void) {
    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB));
}

void I2C1_MasterStop(void) {
    I2C1->CR1 |= I2C_CR1_STOP;
    while (I2C1->CR1 & I2C_CR1_STOP);
}

void I2C1_MasterAddress(uint8_t addr, bool readBit) {
    I2C1->DR = addr << 1 | readBit;
    while (!(I2C1->SR1 & I2C_SR1_ADDR));
    uint8_t clearAddr = I2C1->SR1 | I2C1->SR2;
    (void)clearAddr;
}

void I2C1_MasterWrite(uint32_t len, const uint8_t data[static len]) {
    for (uint32_t i = 0; i < len; i++) {
        I2C1->DR = data[i];
        while (!(I2C1->SR1 & I2C_SR1_TXE));
    }
}

void I2C1_MasterRead(uint32_t len, uint8_t data[len]) {
    I2C1->CR1 |= I2C_CR1_ACK;
    for (uint32_t i = 0; i < len; i++) {
        while (!(I2C1->SR1 & I2C_SR1_RXNE));
        data[i] = I2C1->DR;
    }
    I2C1->CR1 &= ~I2C_CR1_ACK;
}