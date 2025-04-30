#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

class MotorControl {
public:
    // Constructor to initialize motor control with two pins
    MotorControl(int pin1, int pin2);

    // Method to set up the motor control
    void setup();

    // Method to move the motor forward
    void forward(int speed);

    // Method to move the motor backward
    void backward(int speed);

    // Method to stop the motor
    void stop();

private:
    int _pin1;
    int _pin2;
};

#endif // MOTOR_CONTROL_H
