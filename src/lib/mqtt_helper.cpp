#include "mqtt_helper.h"

MqttHelper::MqttHelper(const String& serverAddress, uint16_t port, const String& clientId, const String& username, const String& password)
    : serverAddress_(serverAddress), clientId_(clientId), username_(username), password_(password), mqttClient_(wifiClient_) {
    mqttClient_.setServer(serverAddress_.c_str(), port);
    mqttClient_.setCallback([this](char* topic, byte* payload, unsigned int length) {
        onMqttMessage(topic, payload, length);
    });
}

void MqttHelper::connect() {
    while (!mqttClient_.connected()) {
        Serial.print("Connecting to MQTT broker...");
        bool connected = username_.isEmpty() ? mqttClient_.connect(clientId_.c_str())
                                             : mqttClient_.connect(clientId_.c_str(), username_.c_str(), password_.c_str());
        if (connected) {
            Serial.println("connected.");
        } else {
            Serial.print("failed, rc=");
            Serial.println(mqttClient_.state());
            delay(1000);
        }
    }
}

void MqttHelper::loop() {
    if (!mqttClient_.connected()) reconnect();
    mqttClient_.loop();
}

void MqttHelper::publish(const String& topic, const String& message, bool retained) {
    mqttClient_.publish(topic.c_str(), message.c_str(), retained);
}

void MqttHelper::subscribe(const String& topic) {
    mqttClient_.subscribe(topic.c_str());
}

void MqttHelper::setMessageCallback(MessageCallback callback) {
    messageCallback_ = callback;
}

void MqttHelper::onMqttMessage(char* topic, byte* payload, unsigned int length) {
    if (messageCallback_) {
        messageCallback_(String(topic), String((char*)payload).substring(0, length));
    }
}

void MqttHelper::reconnect() {
    connect();
}


