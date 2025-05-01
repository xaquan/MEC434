#include "mqtt_helper.h"

MqttHelper::MqttHelper(const String& serverAddress, uint16_t port, const String& clientId, const String& username, const String& password)
    : serverAddress_(serverAddress), port_(port), clientId_(clientId), username_(username), password_(password), mqttClient_(wifiClient_) {
    mqttClient_.setServer(serverAddress_.c_str(), port_);
    mqttClient_.setCallback([this](char* topic, byte* payload, unsigned int length) {
        onMqttMessage(topic, payload, length);
    });
}

MqttHelper::~MqttHelper() {
    disconnect();
}

void MqttHelper::connect() {
    reconnect();
}

void MqttHelper::disconnect() {
    mqttClient_.disconnect();
}

void MqttHelper::publish(const String& topic, const String& message, int qos, bool retained) {
    mqttClient_.publish(topic.c_str(), message.c_str(), retained);
}

void MqttHelper::subscribe(const String& topic, int qos) {
    mqttClient_.subscribe(topic.c_str());
}

void MqttHelper::setMessageCallback(MessageCallback callback) {
    messageCallback_ = callback;
}

void MqttHelper::loop() {
    if (!mqttClient_.connected()) {
        reconnect();
    }
    mqttClient_.loop();
}

void MqttHelper::onMqttMessage(char* topic, byte* payload, unsigned int length) {
    String topicStr = String(topic);
    String messageStr = String((char*)payload).substring(0, length);
    if (messageCallback_) {
        messageCallback_(topicStr, messageStr);
    }
}

void MqttHelper::reconnect() {
    while (!mqttClient_.connected()) {
        Serial.print("Connecting to MQTT broker...");
        if (mqttClient_.connect(clientId_.c_str(), username_.c_str(), password_.c_str())) {
            Serial.println("connected.");
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqttClient_.state());
            Serial.println(" try again in 5 seconds.");
            delay(5000);
        }
    }
}


