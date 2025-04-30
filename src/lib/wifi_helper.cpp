#include "wifi_helper.h"

WiFiHelper::WiFiHelper(const char* ssid, const char* password)
    : ssid_(ssid), password_(password) {}

void WiFiHelper::connect() {
    Serial.print("Connecting to Wi-Fi...");
    WiFi.begin(ssid_, password_);

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("Wi-Fi connected.");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

bool WiFiHelper::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

String WiFiHelper::getIPAddress() {
    return WiFi.localIP().toString();
}