#include "MPU6050_Reading.h"
#include <Wire.h>

MPU6050_Reading::MPU6050_Reading() {
    Serial.println("MPU6050_Reading constructor called.");
}

bool MPU6050_Reading::begin() {
    // Initialize the MPU6050 sensor
    if (!mpu.begin()) {
        // Initialization failed
        return false;
    }

    // Configure the sensor (optional, based on your needs)
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    return true;
}

void MPU6050_Reading::readSensor() {
    mpu.getEvent(&accel, &gyro, &temp);
}

float MPU6050_Reading::getAccelerationX() const {
    return accel.acceleration.x;
}

float MPU6050_Reading::getAccelerationY() const {
    return accel.acceleration.y;
}

float MPU6050_Reading::getAccelerationZ() const {
    return accel.acceleration.z;
}

float MPU6050_Reading::getGyroX() const {
    return gyro.gyro.x;
}

float MPU6050_Reading::getGyroY() const {
    return gyro.gyro.y;
}

float MPU6050_Reading::getGyroZ() const {
    return gyro.gyro.z;
}