#include "AccelSensor.h"
#include "Display.h"
#include "I2C1.h"
#include "LedCtrl.h"
#include "MagSensor.h"
#include "StateCtrl.h"
#include "SysTick.h"

#include <inttypes.h>
#include <math.h>

static int32_t CalcAzimuth(float mag[3], float accel[3]);
static const char *CalcCardinal(int32_t azimuth);

int main(void) {
    SysTick_Init();
    StateCtrl_Init();
    LedCtrl_Init();
    I2C1_Init();
    MagSensor_Init();
    AccelSensor_Init();
    Display_Init();

    while (1) {
        StateCtrl_Update();
        bool isPrevOn = StateCtrl_IsPrevOn();
        bool isOn = StateCtrl_IsOn();
        if (isOn && !isPrevOn) {
            LedCtrl_BlinkGreen();
        } else if (!isOn && isPrevOn) {
            LedCtrl_BlinkRed();
            Display_Write("Standby...");
        }

        if (isOn == true) {
            float mag[3] = { 0 };
            MagSensor_ReadCalibratedCoords(mag);

            float accel[3] = { 0 };
            AccelSensor_ReadCalibratedCoords(accel);

            int32_t azimuth = CalcAzimuth(mag, accel);
            const char *cardinal = CalcCardinal(azimuth);

            Display_Write("%3s, %" PRIi32, cardinal, azimuth);

            SysTick_Delay(40);
        }
    }

    return 0;
}

static int32_t CalcAzimuth(float mag[3], float accel[3]) {
    float roll = atan2f(accel[1], accel[2]);
    float pitch = atan2f(-accel[0], sqrtf(accel[1] * accel[1] + accel[2] * accel[2]));

    float sr = sinf(roll);
    float cr = cosf(roll);
    float sp = sinf(pitch);
    float cp = cosf(pitch);

    float x = mag[0] * cp + mag[2] * sp;
    float y = mag[0] * sr * sp + mag[1] * cr - mag[2] * sr * cp;

    float azimuth = atan2f(y, x);
    if (y < 0) {
        azimuth += 2 * M_PI;
    }
    azimuth *= 180 / M_PI;

    return azimuth;
}

static const char *CalcCardinal(int32_t azimuth) {
    const char *CARDINAL_TABLE[8] = { "N", "NE", "E", "SE", "S", "SW", "W", "NW" };
    return CARDINAL_TABLE[(azimuth + 22) / 45 % 8];
}