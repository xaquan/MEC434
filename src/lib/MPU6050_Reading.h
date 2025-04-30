#ifndef MPU6050_READING_H
#define MPU6050_READING_H

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

class MPU6050_Reading {
public:
    MPU6050_Reading();
    bool begin();
    void readSensor();
    float getAccelerationX() const;
    float getAccelerationY() const;
    float getAccelerationZ() const;
    float getGyroX() const;
    float getGyroY() const;
    float getGyroZ() const;

private:
    Adafruit_MPU6050 mpu;
    sensors_event_t accel, gyro, temp;
};

#endif // MPU6050_READING_H