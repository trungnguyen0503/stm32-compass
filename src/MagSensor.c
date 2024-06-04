#include "MagSensor.h"
#include "I2C1.h"
#include "SysTick.h"

#define ADDR 0x0D
#define REG_OUT_X_LSB 0x00
#define REG_STATUS 0x06
#define REG_CTRL1 0x09
#define REG_CTRL2 0x0A
#define REG_SET 0x0B

// Track max and min values to calculate offsets and scales
static int16_t maxCoords[3] = { INT16_MIN, INT16_MIN, INT16_MIN };
static int16_t minCoords[3] = { INT16_MAX, INT16_MAX, INT16_MAX };

// Shift center of value ranges to (0, 0, 0)
static float offsets[3] = { 0 };
// Scale values to get normalized amounts
static float scales[3] = { 1, 1, 1 };
static float scaleAvg = 1;

static void WriteToReg(uint8_t reg, uint8_t data);

void MagSensor_Init(void) {
    // Reset sensor
    WriteToReg(REG_CTRL2, 0b10000000);

    // over sample ratio = 512, measure scale = 8G, output rate = 200hz, mode = continuous
    WriteToReg(REG_CTRL1, 0b00011101);
    // Enable pointer rollover for quick read, disable interrupt pin
    WriteToReg(REG_CTRL2, 0b01000001);
    // Set to run
    WriteToReg(REG_SET, 0b00000001);

    // Wait for sensor to initialize
    SysTick_Delay(100);
}

void MagSensor_ReadRawCoords(int16_t coords[3]) {
    uint8_t buf[6] = { 0 };

    // Set sensor pointer to the start of output data registers
    I2C1_MasterStart();
    I2C1_MasterAddress(ADDR, false);
    I2C1_MasterWrite(1, &(uint8_t){ REG_OUT_X_LSB });
    I2C1_MasterStop();

    // Read all output data registers
    I2C1_MasterStart();
    I2C1_MasterAddress(ADDR, true);
    I2C1_MasterRead(6, buf);
    I2C1_MasterStop();

    // Convert to int16 coords
    for (int i = 0; i < 3; i++) {
        coords[i] = buf[2 * i] | (buf[2 * i + 1] << 8);
    }
}

void MagSensor_ReadCalibratedCoords(float coords[3]) {
    int16_t raw[3] = { 0 };
    bool needCalib = false;

    MagSensor_ReadRawCoords(raw);

    // Check for new max and min values
    for (int i = 0; i < 3; i++) {
        if (raw[i] > maxCoords[i]) {
            maxCoords[i] = raw[i];
            needCalib = true;
        }
        if (raw[i] < minCoords[i]) {
            minCoords[i] = raw[i];
            needCalib = true;
        }
    }

    // Re-calibrate if needed
    if (needCalib) {
        for (int i = 0; i < 3; i++) {
            offsets[i] = minCoords[i] / 2.0f + maxCoords[i] / 2.0f;
            scales[i] = maxCoords[i] / 2.0f - minCoords[i] / 2.0f;
        }
        scaleAvg = (scales[0] + scales[1] + scales[2]) / 3.0f;
    }

    // Calibrate raw results
    for (int i = 0; i < 3; i++) {
        coords[i] = (raw[i] - offsets[i]) / scales[i] * scaleAvg;
    }
}

static void WriteToReg(uint8_t reg, uint8_t data) {
    uint8_t writeData[2] = { reg, data };

    I2C1_MasterStart();
    I2C1_MasterAddress(ADDR, 0);
    I2C1_MasterWrite(sizeof writeData, writeData);
    I2C1_MasterStop();
}