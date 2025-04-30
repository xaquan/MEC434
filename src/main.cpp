#include <Arduino.h>
#include "lib/MPU6050_Reading.h" // Adjust the path as necessary
#include "lib/Motor_Control.h" // Adjust the path as necessary
#include "lib/mqtt_helper.h" // Adjust the path as necessary

#define ON_TRACK_PIN 15 // Pin to read the state of the ON_TRACK signal
#define MOTOR_PIN1 13 // Pin for motor control (example pin, adjust as necessary)
#define MOTOR_PIN2 12 // Pin for motor control (example pin, adjust as necessary)

#define MQTT_SERVER "mqtt.example.com" // Example MQTT server, adjust as necessary
#define MQTT_PORT 1883 // Example MQTT port, adjust as necessary  
#define MQTT_CLIENT_ID "client_id" // Example MQTT client ID, adjust as necessary
#define MQTT_TOPIC "sensor/data" // Example MQTT topic, adjust as necessary
#define MQTT_USERNAME "username" // Example MQTT username, adjust as necessary
#define MQTT_PASSWORD "password" // Example MQTT password, adjust as necessary
#define MQTT_QOS 0 // Example MQTT QoS level, adjust as necessary
#define MQTT_RETAINED false // Example MQTT retained message flag, adjust as necessary


MPU6050_Reading sensor;
MotorControl motor(MOTOR_PIN1, MOTOR_PIN2); // Example motor pins, adjust as necessary
int motorSpeed = 0; // Variable to store motor speed
MqttHelper mqtt(MQTT_SERVER, 1883, MQTT_CLIENT_ID); // Example MQTT server and client ID, adjust as necessary

void setSpeedByPercent(int inputSpeedPercent); // Function prototype for setting motor speed
void printSensorData(); // Function prototype for printing sensor data
void onTrackSafety(); // Function prototype for printing ON_TRACK state
bool isOnTrack(); // Function prototype for checking ON_TRACK state



void setup() {
  // Initialize Serial communication for debugging
  Serial.begin(115200);
  while (!Serial) {
    delay(10); // Wait for Serial to initialize
  }

  // Initialize the MPU6050 sensor
  sensor.begin();
  Serial.println("MPU6050 initialized successfully.");

  pinMode(ON_TRACK_PIN, INPUT_PULLDOWN); // Set ON_TRACK pin as input

}

void loop() {
  
  onTrackSafety(); // Check ON_TRACK signal and control motor accordingly
  // printSensorData();

  // Control the motor speed based on Serial input
  if (Serial.available() > 0 and isOnTrack()) {
    int inputSpeedPercent = Serial.parseInt(); // Read integer from Serial input
    if (inputSpeedPercent >= 0 && inputSpeedPercent <= 100) {
      setSpeedByPercent(inputSpeedPercent);
    } else {
      Serial.println("Invalid speed. Enter a value between 0 and 100.");
    }
  }

  


  // Print motor speed for debugging
  // Serial.print("Current Motor Speed: ");
  // Serial.println(motorSpeed);
}

void publishSensorDataToMQTT() {
  // Read data from the MPU6050 sensor
  sensor.readSensor();

  // Create a JSON string with sensor data
  String payload = "{";
  payload += "\"acceleration_x\":" + String(sensor.getAccelerationX(), 2) + ",";
  payload += "\"acceleration_y\":" + String(sensor.getAccelerationY(), 2) + ",";
  payload += "\"acceleration_z\":" + String(sensor.getAccelerationZ(), 2) + ",";
  payload += "\"gyro_x\":" + String(sensor.getGyroX(), 2) + ",";
  payload += "\"gyro_y\":" + String(sensor.getGyroY(), 2) + ",";
  payload += "\"gyro_z\":" + String(sensor.getGyroZ(), 2);
  payload += "}";

  // Publish the payload to the MQTT server
  mqtt.publish("sensor/data", payload.c_str());

  // Debugging output
  Serial.println("Published sensor data to MQTT:");
  Serial.println(payload);
}

bool isOnTrack() {
  // Read the digital value of pin 15 (ON_TRACK_PIN)
  int onTrackSignal = digitalRead(ON_TRACK_PIN);

  // Return true if the signal is HIGH, false otherwise
  return onTrackSignal == HIGH;
}

// Function to check the ON_TRACK signal and control the motor accordingly
// This function is called in the loop to ensure the motor is controlled based on the ON_TRACK signal
void onTrackSafety() {
  // Static variable to store the last state of ON_TRACK_PIN
  static int lastOnTrackState = -1;

  // Get the current ON_TRACK state using isOnTrack()
  int onTrackState = isOnTrack();

  // Only process if the state has changed
  if (onTrackState != lastOnTrackState) {
    lastOnTrackState = onTrackState; // Update the last state

    if (onTrackState) {
      Serial.println("ON_TRACK signal is HIGH. Motor is ON.");
    } else {
      Serial.println("ON_TRACK signal is LOW. Motor is OFF.");
      motor.stop(); // Stop the motor if ON_TRACK signal is LOW
    }
  }
}

void printSensorData()
{
  // Read data from the MPU6050 sensor
  sensor.readSensor();
  Serial.print("Acceleration X: ");
  Serial.print(sensor.getAccelerationX(), 2);
  Serial.print(" m/s^2, ");
  Serial.print("Y: ");
  Serial.print(sensor.getAccelerationY(), 2);
  Serial.print(" m/s^2, ");
  Serial.print("Z: ");
  Serial.print(sensor.getAccelerationZ(), 2);
  Serial.print(" m/s^2, ");
  Serial.print("Gyro X: ");
  Serial.print(sensor.getGyroX(), 2);
  Serial.print(" °/s, ");
  Serial.print("Y: ");
  Serial.print(sensor.getGyroY(), 2);
  Serial.print(" °/s, ");
  Serial.print("Z: ");
  Serial.print(sensor.getGyroZ(), 2);
  Serial.println(" °/s");
}

void setSpeedByPercent(int inputSpeedPercent)
{
  motorSpeed = map(inputSpeedPercent, 0, 100, 0, 255); // Map percentage to 0-255 range
  motor.forward(motorSpeed);                           // Set motor speed
  Serial.print("Motor Speed set to: ");
  Serial.print(inputSpeedPercent);
  Serial.println(" %");
}
