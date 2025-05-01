#include "server_helper.h"

ServerHelper::ServerHelper(int port) : server_(port) {}

void ServerHelper::begin() {
    server_.on("/", [this]() { handleRoot(); });
    server_.on("/start", [this]() { handleStart(); });
    server_.on("/stop", [this]() { handleStop(); });
    server_.on("/setSpeed", [this]() { handleSetSpeed(); });
    server_.on("/data", [this]() { handleData(); }); // New endpoint for real-time data
    server_.begin();
    Serial.println("Web server started.");
}

void ServerHelper::handleClient() {
    server_.handleClient();
}

void ServerHelper::setStartCallback(std::function<void()> callback) {
    startCallback_ = callback;
}

void ServerHelper::setStopCallback(std::function<void()> callback) {
    stopCallback_ = callback;
}

void ServerHelper::setSpeedCallback(std::function<void(int)> callback) {
    speedCallback_ = callback;
}

void ServerHelper::handleRoot() {
    String html = R"rawliteral(
        <!DOCTYPE html>
        <html>
        <head>
            <title>Slot Car Control</title>
        </head>
        <body>
            <h1>Slot Car Control</h1>
            <button onclick="fetch('/start')">Start</button>
            <button onclick="fetch('/stop')">Stop</button>
            <br><br>
            <input type="number" id="speed" placeholder="Enter speed (0-100)">
            <button onclick="setSpeed()">Set Speed</button>
            <br><br>
            <h2>Real-Time Data</h2>
            <p>Motor Speed: <span id="motorSpeed">0</span></p>
            <p>Sensor Data: <span id="sensorData">N/A</span></p>
            <script>
                function setSpeed() {
                    const speed = document.getElementById('speed').value;
                    fetch(`/setSpeed?value=${speed}`);
                }

                function updateData() {
                    fetch('/data')
                        .then(response => response.json())
                        .then(data => {
                            document.getElementById('motorSpeed').innerText = data.motorSpeed;
                            document.getElementById('sensorData').innerText = data.sensorData;
                        })
                        .catch(error => console.error('Error fetching data:', error));
                }

                setInterval(updateData, 1000); // Update data every second
            </script>
        </body>
        </html>
    )rawliteral";
    server_.send(200, "text/html", html);
}

void ServerHelper::handleStart() {
    if (startCallback_) startCallback_();
    server_.send(200, "text/plain", "Car started");
}

void ServerHelper::handleStop() {
    if (stopCallback_) stopCallback_();
    server_.send(200, "text/plain", "Car stopped");
}

void ServerHelper::handleSetSpeed() {
    if (server_.hasArg("value")) {
        int speed = server_.arg("value").toInt();
        if (speedCallback_) speedCallback_(speed);
        server_.send(200, "text/plain", "Speed set to " + String(speed));
    } else {
        server_.send(400, "text/plain", "Missing speed value");
    }
}

void ServerHelper::handleData() {
    String json = "{";
    json += "\"motorSpeed\":" + String(motorSpeed_) + ",";
    json += "\"sensorData\":\"" + sensorData_ + "\"";
    json += "}";
    server_.send(200, "application/json", json);
}