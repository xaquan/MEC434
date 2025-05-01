#ifndef SERVER_HELPER_H
#define SERVER_HELPER_H

#include <WiFi.h>
#include <WebServer.h>

class ServerHelper {
public:
    ServerHelper(int port);
    void begin();
    void handleClient();
    void setStartCallback(std::function<void()> callback);
    void setStopCallback(std::function<void()> callback);
    void setSpeedCallback(std::function<void(int)> callback);
    void updateMotorSpeed(int speed) { motorSpeed_ = speed; }
    void updateSensorData(const String& data) { sensorData_ = data; }

private:
    WebServer server_;
    std::function<void()> startCallback_;
    std::function<void()> stopCallback_;
    std::function<void(int)> speedCallback_;

    int motorSpeed_ = 0; // Store motor speed
    String sensorData_ = "N/A"; // Store sensor data

    void handleRoot();
    void handleStart();
    void handleStop();
    void handleSetSpeed();
    void handleData(); // New function to handle data requests
};

#endif // SERVER_HELPER_H