#ifndef MQTT_HELPER_H
#define MQTT_HELPER_H

#include <WiFi.h>
#include <PubSubClient.h>
#include <functional>

class MqttHelper {
public:
    using MessageCallback = std::function<void(const String& topic, const String& message)>;

    MqttHelper(const String& serverAddress, uint16_t port, const String& clientId, const String& username = "", const String& password = "");
    void connect();
    void loop();
    void publish(const String& topic, const String& message, bool retained = false);
    void subscribe(const String& topic);
    void setMessageCallback(MessageCallback callback);

private:
    String serverAddress_, clientId_, username_, password_;
    WiFiClient wifiClient_;
    PubSubClient mqttClient_;
    MessageCallback messageCallback_;

    void reconnect();
    void onMqttMessage(char* topic, byte* payload, unsigned int length);
};

#endif // MQTT_HELPER_H
