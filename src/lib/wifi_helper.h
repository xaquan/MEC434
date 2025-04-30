#ifndef WIFI_HELPER_H
#define WIFI_HELPER_H

#include <WiFi.h>

class WiFiHelper {
public:
    WiFiHelper(const char* ssid, const char* password);
    void connect();
    bool isConnected();
    String getIPAddress();

private:
    const char* ssid_;
    const char* password_;
};

#endif // WIFI_HELPER_H