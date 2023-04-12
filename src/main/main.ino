#include <WiFi.h>
#include "SPIFFS.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "updatejson.h"
// Replace with your network credentials
const char* ssid = "540_ROOM";
const char* password = "12345555";
 
// Set web server port number to 80
AsyncWebServer server(80);

bool checkBoxValue = false;

void handledatetime(AsyncWebServerRequest* request) {
  String jsonResponse = readJSON();
  Serial.println(jsonResponse);
  request->send(200, "application/json", jsonResponse);
}

void handleGetSwitchState(AsyncWebServerRequest* request) {
  bool switchState = currentStateJSON();
  request->send(200, "text/plain", switchState ? "1" : "0");
}

void getCurrentSwitchState(bool switchState) {
  checkBoxValue = switchState;
  updateJSONFile("switch", checkBoxValue, "", "");
  readJSONFile();
}

void switchstate(AsyncWebServerRequest *request) {
  if (request->hasParam("checkBox")) {
    String checkBox = request->getParam("checkBox")->value();
    // Handle the received checkBox value
//    Serial.print("CheckBox value: ");
//    Serial.println(checkBox);
    // Convert the checkBox value to a boolean
    bool checkBoxValue = checkBox == "true";
    // Handle the checkBoxValue as needed
    // Respond with the switch state
    getCurrentSwitchState(checkBoxValue);
    request->send(200, "text/plain", checkBoxValue ? "1" : "0");
  } else {
    request->send(400, "text/plain", "Invalid request");
  }
}

void handleSensorControlTemp(AsyncWebServerRequest* request) {
  if (request->hasParam("start") && request->hasParam("end")) {
    String startTime = request->getParam("start")->value();
    String endTime = request->getParam("end")->value();
    // Handle the received start and end times for temperature sensor
//    Serial.print("Temperature Sensor Start Time: ");
//    Serial.println(startTime);
//    Serial.print("Temperature Sensor End Time: ");
//    Serial.println(endTime);
    // Update the JSON file with the new start and end times for the temperature sensor
    updateJSONFile("temp", checkBoxValue, startTime, endTime);
  }
  request->send(200, "text/plain", "OK");
}

void handleSensorControlLDR(AsyncWebServerRequest* request) {
  if (request->hasParam("start") && request->hasParam("end")) {
    String startTime = request->getParam("start")->value();
    String endTime = request->getParam("end")->value();

    // Handle the received start and end times for LDR sensor
  //    Serial.print("LDR Sensor Start Time: ");
  //    Serial.println(startTime);
  //    Serial.print("LDR Sensor End Time: ");
  //    Serial.println(endTime);
    // Update the JSON file with the new start and end times for the LDR sensor
    updateJSONFile("ldr", checkBoxValue, startTime, endTime);
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

  // Initialize the state.json file if it doesn't exist
  if (!SPIFFS.exists("/state.json")) {
    Serial.println("Creating initial state.json");
    updateJSONFile("switch", checkBoxValue, "", "");
    updateJSONFile("temp", checkBoxValue, "", "");
    updateJSONFile("ldr", checkBoxValue, "", "");
  }

  // Serve the HTML file and CSS file
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/sensor_control.html", "text/html");
  });
  
  server.on("/css.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/css.css", "text/css");
  });

  // Set up the route for handling sensor control for temperature sensor
  server.on("/sensor-control-temp", HTTP_GET, handleSensorControlTemp);

  // Set up the route for handling sensor control for LDR sensor
  server.on("/sensor-control-ldr", HTTP_GET, handleSensorControlLDR);

  // Set up the route for handling on/off switch control
  server.on("/check-on-off", HTTP_GET,switchstate);
  
  // Set up the route for handling on/off switch control
  server.on("/switchstate", HTTP_GET, handleGetSwitchState);

  server.on("/handledatetime", HTTP_GET, handledatetime);

  // Start web server
  server.begin();
}

void loop() {
}
