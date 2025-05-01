#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Arduino.h>
#include "lib/MPU6050_Reading.h" // Adjust the path as necessary
#include "lib/Motor_Control.h"   // Adjust the path as necessary


// WiFi credentials
const char* ssid = "XQuaniPhone";
const char* password = "12345677";

// MQTT broker details
const char* mqtt_server = "172.20.10.3";
const int mqtt_port = 1883;
const char* mqtt_topic = "/sensor/data";

WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_MPU6050 mpu;

void subscribeControl();

void setupWiFi() {
    delay(10);
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");
}

void reconnectMQTT() {
    while (!client.connected()) {
        Serial.println("Connecting to MQTT...");
        if (client.connect("ESP32Client")) {
            Serial.println("Connected to MQTT broker");
        } else {
            Serial.print("Failed to connect, rc=");
            Serial.print(client.state());
            Serial.println(" Retrying in 5 seconds...");
            delay(5000);
        }
    }
}

void setup() {
    Serial.begin(115200);
    Wire.begin();

    if (!mpu.begin()) {
        Serial.println("Failed to find MPU6050 chip");
        while (1) {
            delay(10);
        }
    }
    Serial.println("MPU6050 initialized");

    setupWiFi();
    client.setServer(mqtt_server, mqtt_port);
}

void loop() {
    if (!client.connected()) {
        reconnectMQTT();
    }
    client.loop();

    publishData();
    subscribeControl();

    delay(10); // Publish every second
}

// subscribe to the topic /control/run payload is boolean
void subscribeControl() {
    client.subscribe("/control/run");
    client.setCallback([](char* topic, byte* payload, unsigned int length) {
        String messageTemp;
        for (int i = 0; i < length; i++) {
            messageTemp += (char)payload[i];
        }
        Serial.println("Message arrived: " + messageTemp);

        if (String(topic) == "/control/run") {
            if (messageTemp == "true") {
                Serial.println("Motor ON");
                // Add code to turn on the motor
            } else {
                Serial.println("Motor OFF");
                // Add code to turn off the motor
            }
        }
    });
}

void publishData()
{
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    String payload = "{";
    payload += "\"accel_x\":" + String(a.acceleration.x) + ",";
    payload += "\"accel_y\":" + String(a.acceleration.y) + ",";
    payload += "\"accel_z\":" + String(a.acceleration.z) + ",";
    payload += "\"gyro_x\":" + String(g.gyro.x) + ",";
    payload += "\"gyro_y\":" + String(g.gyro.y) + ",";
    payload += "\"gyro_z\":" + String(g.gyro.z) + ",";
    payload += "\"temp\":" + String(temp.temperature);
    payload += "}";

    client.publish(mqtt_topic, payload.c_str());
    // Serial.println("Published: " + payload);
}
