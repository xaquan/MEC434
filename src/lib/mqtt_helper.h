#ifndef MQTT_HELPER_H
#define MQTT_HELPER_H

#include <functional>
#include <WiFi.h>
#include <PubSubClient.h>

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
    void loop(); // Call this in the main loop to maintain the connection

private:
    String serverAddress_;
    uint16_t port_;
    String clientId_;
    String username_;
    String password_;
    WiFiClient wifiClient_;
    PubSubClient mqttClient_;
    MessageCallback messageCallback_;

    void onMqttMessage(char* topic, byte* payload, unsigned int length);
    void reconnect();
};

#endif // MQTT_HELPER_H
