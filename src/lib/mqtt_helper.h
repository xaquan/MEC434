#ifndef MQTT_HELPER_H
#define MQTT_HELPER_H

#include <functional>
#include <AsyncMqttClient.h>
#include <WiFi.h>

class MqttHelper {
public:
    using MessageCallback = std::function<void(const String& topic, const String& message)>;

    MqttHelper(const String& serverAddress, uint16_t port, const String& clientId, const String& username = "", const String& password = "");
    ~MqttHelper();

    void connect();
    void disconnect();
    void publish(const String& topic, const String& message, int qos = 0, bool retained = false);
    void subscribe(const String& topic, int qos = 0);
    void setMessageCallback(MessageCallback callback);

private:
    String serverAddress_;
    uint16_t port_;
    String clientId_;
    String username_;
    String password_;
    AsyncMqttClient mqttClient_;
    MessageCallback messageCallback_;

    void onMqttConnect(bool sessionPresent);
    void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
    void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
};

#endif // MQTT_HELPER_H
