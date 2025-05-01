#include <Arduino.h>
#include "lib/MPU6050_Reading.h" // Adjust the path as necessary
#include "lib/Motor_Control.h"   // Adjust the path as necessary
#include "lib/mqtt_helper.h"     // Adjust the path as necessary
#include "lib/wifi_helper.h"     // Include the WiFiHelper header
#include "lib/server_helper.h"   // Include the ServerHelper header

#define ON_TRACK_PIN 15 // Pin to read the state of the ON_TRACK signal
#define MOTOR_PIN1 13   // Pin for motor control (example pin, adjust as necessary)
#define MOTOR_PIN2 12   // Pin for motor control (example pin, adjust as necessary)

#define WIFI_SSID "XQuaniPhone"       // Replace with your Wi-Fi SSID
#define WIFI_PASSWORD "12345677" // Replace with your Wi-Fi password

#define MQTT_SERVER "d0a180a333ba47b0b83f52ffc2409042.s1.eu.hivemq.cloud" // Example MQTT server, adjust as necessary
#define MQTT_PORT 8883                    // Example MQTT port, adjust as necessary
#define MQTT_CLIENT_ID "SlotCarChamp"     // Example MQTT client ID, adjust as necessary
#define MQTT_TOPIC "sensor/data"          // Example MQTT topic, adjust as necessary
#define MQTT_USERNAME "hivemq.webclient.1746056380653"            // Example MQTT username, adjust as necessary
#define MQTT_PASSWORD "dH5,!S$4j3Mu9<ItWRex"      // Example MQTT password, adjust as necessary
#define MQTT_QOS 0                        // Example MQTT QoS level, adjust as necessary
#define MQTT_RETAINED false               // Example MQTT retained message flag, adjust as necessary

MPU6050_Reading sensor;
MotorControl motor(MOTOR_PIN1, MOTOR_PIN2); // Example motor pins, adjust as necessary
int motorSpeed = 0;                         // Variable to store motor speed
MqttHelper mqtt(MQTT_SERVER, 1883, MQTT_CLIENT_ID); // Example MQTT server and client ID, adjust as necessary
WiFiHelper wifi(WIFI_SSID, WIFI_PASSWORD);          // Create an instance of WiFiHelper
ServerHelper server(80); // Create a server on port 80

void setSpeedByPercent(int inputSpeedPercent); // Function prototype for setting motor speed
void printSensorData();                        // Function prototype for printing sensor data
void onTrackSafety();                          // Function prototype for printing ON_TRACK state
bool isOnTrack();         
void publishSensorDataToMQTT(MPU6050_Reading sensor);    
void readMqttMotorStop();              // Function prototype for checking ON_TRACK state

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        delay(10);
    }

    // Connect to Wi-Fi
    wifi.connect();

    // Initialize the server
    server.setStartCallback([]() {
        Serial.println("Start button pressed");
        motor.forward(20); // Example: Start motor at full speed
    });
    server.setStopCallback([]() {
        Serial.println("Stop button pressed");
        motor.stop(); // Example: Stop motor
    });
    server.setSpeedCallback([](int speed) {
        Serial.println("Set speed to: " + String(speed));
        motor.forward(map(speed, 0, 100, 0, 255)); // Map speed to motor range
    });
    server.begin();

    // Initialize other components
    sensor.begin();
    pinMode(ON_TRACK_PIN, INPUT_PULLDOWN);
}

void loop() {
  if (!wifi.isConnected()) {
    Serial.println("Wi-Fi disconnected. Reconnecting...");
    wifi.connect(); // Reconnect to Wi-Fi if disconnected
  }
  
  // Check the ON_TRACK signal and control the motor accordingly
  // If the ON_TRACK signal is LOW, stop the motor
  onTrackSafety();

  sensor.readSensor(); // Read sensor data

  // Start the web server and handle incoming requests
  server.handleClient();// Update real-time data
  server.updateMotorSpeed(motorSpeed); // Update motor speed
  server.updateSensorData("AccelX: " + String(sensor.getAccelerationX(), 2)); // Update sensor data


  // mqtt.loop(); // Call the MQTT loop to maintain the connection


  // // Control the motor speed based on Serial input
  // if (Serial.available() > 0 and isOnTrack()) {
  //   int inputSpeedPercent = Serial.parseInt(); // Read integer from Serial input
  //   if (inputSpeedPercent >= 0 && inputSpeedPercent <= 100) {
  //     setSpeedByPercent(inputSpeedPercent);
  //   } else {
  //     Serial.println("Invalid speed. Enter a value between 0 and 100.");
  //   }
  // }

  // publishSensorDataToMQTT(sensor); // Publish sensor data to MQTT
  // readMqttMotorStop();
  // delay(1000); // Delay for 1 second before the next loop iteration
}

void publishSensorDataToMQTT(MPU6050_Reading sensor) {
  Serial.println("Publishing sensor data to MQTT...");
  // Read data from the MPU6050 sensor

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

void readMqttMotorStop() {
  // Read the ON_TRACK signal from the MQTT topic
  Serial.println("Subscribing to MQTT topic: motorStop");
  mqtt.subscribe("motorStop", MQTT_QOS);
  mqtt.setMessageCallback([](const String& topic, const String& message) {
    if (topic == "motorStop") {
      Serial.println("Received MQTT message on topic: " + topic);
      Serial.println("Message: " + message);
      if (message == "1") {
        motor.stop(); // Stop the motor if the message is "stop"
        Serial.println("Motor stopped by MQTT command.");
      }
    }
  });
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
      // Delay to allow the motor to start
      delay(1000); // Adjust the delay as necessary

    } else {
      Serial.println("ON_TRACK signal is LOW. Motor is OFF.");
      motor.stop(); // Stop the motor if ON_TRACK signal is LOW
    }
  }
}

void printSensorData() {
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

void setSpeedByPercent(int inputSpeedPercent) {
  motorSpeed = map(inputSpeedPercent, 0, 100, 0, 255); // Map percentage to 0-255 range
  motor.forward(motorSpeed);                           // Set motor speed
  Serial.print("Motor Speed set to: ");
  Serial.print(inputSpeedPercent);
  Serial.println(" %");
}
