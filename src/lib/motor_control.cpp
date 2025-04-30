#include "motor_control.h"
#include <Arduino.h>

MotorControl::MotorControl(int pin1, int pin2) : _pin1(pin1), _pin2(pin2) {}

void MotorControl::setup() {
    pinMode(_pin1, OUTPUT);
    pinMode(_pin2, OUTPUT);
    stop(); // Ensure motor is stopped during setup
}

void MotorControl::forward(int speed) {
    analogWrite(_pin1, speed);
    analogWrite(_pin2, 0);
}

void MotorControl::backward(int speed) {
    analogWrite(_pin1, 0);
    analogWrite(_pin2, speed);
}

void MotorControl::stop() {
    analogWrite(_pin1, 0);
    analogWrite(_pin2, 0);
}