#include "mqtt_helper.h"

MqttHelper::MqttHelper(const String& serverAddress, uint16_t port, const String& clientId, const String& username, const String& password)
    : serverAddress_(serverAddress), port_(port), clientId_(clientId), username_(username), password_(password) {
    mqttClient_.onConnect([this](bool sessionPresent) { onMqttConnect(sessionPresent); });
    mqttClient_.onDisconnect([this](AsyncMqttClientDisconnectReason reason) { onMqttDisconnect(reason); });
    mqttClient_.onMessage([this](char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
        onMqttMessage(topic, payload, properties, len, index, total);
    });
}

MqttHelper::~MqttHelper() {
    disconnect();
}

void MqttHelper::connect() {
    mqttClient_.setServer(serverAddress_.c_str(), port_);
    if (!username_.isEmpty() && !password_.isEmpty()) {
        mqttClient_.setCredentials(username_.c_str(), password_.c_str());
    }
    mqttClient_.connect();
}

void MqttHelper::disconnect() {
    mqttClient_.disconnect();
}

void MqttHelper::publish(const String& topic, const String& message, int qos, bool retained) {
    mqttClient_.publish(topic.c_str(), qos, retained, message.c_str());
}

void MqttHelper::subscribe(const String& topic, int qos) {
    mqttClient_.subscribe(topic.c_str(), qos);
}

void MqttHelper::setMessageCallback(MessageCallback callback) {
    messageCallback_ = callback;
}

void MqttHelper::onMqttConnect(bool sessionPresent) {
    Serial.println("Connected to MQTT broker.");
}

void MqttHelper::onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
    Serial.println("Disconnected from MQTT broker.");
}

void MqttHelper::onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
    String topicStr = String(topic);
    String messageStr = String(payload).substring(0, len);
    if (messageCallback_) {
        messageCallback_(topicStr, messageStr);
    }
}