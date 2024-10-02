# Microcontroller used: STM32F103C8T6 board

# Actuators
- QMC5883L Magnetometer
- MPU6050 Accelerometer
- LCD1602
- Generic LEDs and buttons

# Components
General timing is done using SysTick.

The sensors and LCD screen are controlled using the I2C1 peripheral.

The status LEDs use TIM2 peripheral for timing.

Updating button states, reading sensors and writing to screen is done synchronously in the main loop.
