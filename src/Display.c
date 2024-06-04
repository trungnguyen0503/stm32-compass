#include "Display.h"
#include "I2C1.h"
#include "SysTick.h"

#include "stm32f103xb.h"

#include <stdarg.h>
#include <stdio.h>

#define ADDR 0x27

#define CMD_CLEAR 0x01
#define CMD_DISPLAY 0x08
#define CMD_FUNCTION 0x20

#define CTRL_CMD 0x80
#define CTRL_DATA 0x40

#define PIN_EN 0b0100
#define PIN_RS 0b0001
#define PIN_BACKLIGHT 0b1000

#define PRINT_BUF_SIZE 17

static void Write4Bit(uint8_t data, bool rs);
static void Write8Bit(uint8_t data, bool rs);

void Display_Init(void) {
    // Turn off and on to reset the display with GPIO output
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    GPIOB->CRH &= ~(GPIO_CRH_MODE12 | GPIO_CRH_CNF12);
    GPIOB->CRH |= (0b01 << GPIO_CRH_MODE12_Pos) | (0b00 << GPIO_CRH_CNF12_Pos);
    GPIOB->BSRR |= GPIO_BSRR_BS12;
    SysTick_Delay(50);

    // Setup 4bit operation
    Write4Bit(CMD_FUNCTION >> 4, false);   // Set 4 bit operation
    Write8Bit(CMD_FUNCTION, false);        // 1 line display, 5x8 font
    Write8Bit(CMD_DISPLAY | 0b100, false); // Dipslay on, no cursor
    SysTick_Delay(50);

    Display_Write("Standby...");
}

__attribute((format(printf, 1, 2))) void Display_Write(const char *fmt, ...) {
    Display_Clear();
    // Write formatted string
    va_list va;
    va_start(va, fmt);
    char buf[PRINT_BUF_SIZE] = { 0 };
    int len = vsnprintf(buf, sizeof buf, fmt, va);
    va_end(va);

    for (int i = 0; i < len; i++) {
        Write8Bit(buf[i], true);
    }
}

void Display_Clear(void) {
    Write8Bit(CMD_CLEAR, false);
}

static void Write4Bit(uint8_t data, bool rs) {
    data = (data << 4) | PIN_BACKLIGHT;
    if (rs) {
        data |= PIN_RS;
    }
    uint8_t writeData[2] = { data | PIN_EN, data };

    I2C1_MasterStart();
    I2C1_MasterAddress(ADDR, 0);
    I2C1_MasterWrite(sizeof writeData, writeData);
    I2C1_MasterStop();
    SysTick_Delay(3);
}

static void Write8Bit(uint8_t data, bool rs) {
    uint8_t msb = (data & 0xF0) | PIN_BACKLIGHT;
    uint8_t lsb = (data << 4) | PIN_BACKLIGHT;
    if (rs) {
        msb |= PIN_RS;
        lsb |= PIN_RS;
    }
    uint8_t writeData[4] = {
        msb | PIN_EN, msb,
        lsb | PIN_EN, lsb
    };

    I2C1_MasterStart();
    I2C1_MasterAddress(ADDR, 0);
    I2C1_MasterWrite(sizeof writeData, writeData);
    I2C1_MasterStop();
    SysTick_Delay(3);
}