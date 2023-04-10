#include <WiFi.h>
#include "SPIFFS.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Replace with your network credentials
const char* ssid = "540_ROOM";
const char* password = "12345555";

// Set web server port number to 80
AsyncWebServer server(80);

void handleSensorControlTemp(AsyncWebServerRequest* request) {
  if (request->hasParam("start") && request->hasParam("end")) {
    String startTime = request->getParam("start")->value();
    String endTime = request->getParam("end")->value();
    // Handle the received start and end times for temperature sensor
    Serial.print("Temperature Sensor Start Time: ");
    Serial.println(startTime);
    Serial.print("Temperature Sensor End Time: ");
    Serial.println(endTime);
  }
  request->send(200, "text/plain", "OK");
}

void handleSensorControlCO2(AsyncWebServerRequest* request) {
  if (request->hasParam("start") && request->hasParam("end")) {
    String startTime = request->getParam("start")->value();
    String endTime = request->getParam("end")->value();

    // Handle the received start and end times for CO2 sensor
    Serial.print("CO2 Sensor Start Time: ");
    Serial.println(startTime);
    Serial.print("CO2 Sensor End Time: ");
    Serial.println(endTime);
  }
  request->send(200, "text/plain", "OK");
}

void setup() {
  // Start the serial communication
  Serial.begin(115200);

  // Mount SPIFFS file system
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Print local IP address
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Serve the HTML file and CSS file
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/sensor_control.html", "text/html");
  });
  server.on("/css.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/css.css", "text/css");
  });

  // Set up the route for handling sensor control for temperature sensor
  server.on("/sensor-control-temp", HTTP_GET, handleSensorControlTemp);

  // Set up the route for handling sensor control for CO2 sensor
  server.on("/sensor-control-co2", HTTP_GET, handleSensorControlCO2);

  // Set up the route for handling on/off switch control
  server.on("/switch-control", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (request->hasParam("state")) {
      String state = request->getParam("state")->value();
      // Handle the received state for on/off switch
      Serial.print("Switch state: ");
      Serial.println(state);
    }
    request->send(200, "text/plain", "OK");
  });

  // Start web server
  server.begin();
}

void loop() {
}
