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
////////////////////Interrupts Part////////////////////////
volatile int interruptCounter_temp;
volatile int interruptCounter_ldr;
//String currentTime_RTC "16:00";
void interrupts_iram() {
  bool stateCurrent = currentStateJSON();
//  SensorData sensorData = readJSONFile();
//  String tempStart = sensorData.tempStart;
//  String tempEnd = sensorData.tempEnd;
//  String ldrStart = sensorData.ldrStart;
//  String ldrEnd = sensorData.ldrEnd;
//  if (stateCurrent == 1) {
//    if (tempStart == currentTime_RTC) {
//      interruptCounter_temp++;
//      Serial.print("interrupt!!");
//    }
//    else if (tempEnd == currentTime_RTC) {
//      interruptCounter_temp--;
//      Serial.print("Out interrupt!!");
//    }
//    if (ldrStart  == currentTime_RTC) {
//      interruptCounter_ldr++;
//      Serial.print("interrupt!!");
//    }
//    else if (ldrEnd == currentTime_RTC) { // fix typo here
//      interruptCounter_ldr--;
//      Serial.print("Out interrupt!!");
//    }
//  }
//  interruptCounter_temp--;
//  interruptCounter_ldr--;
}

//////////////////////////////////////////////////////////
long lastTime = millis();

void loop() {
  interrupts_iram();
//  temp_test();
//    if (interruptCounter_temp > 0 && interruptCounter_ldr > 0) {
//      Serial.print("InterruptCounter_temp and InterruptCounter_ldr!!");
//    }
//    else if (interruptCounter_temp > 0 && interruptCounter_ldr < 0) {
//      Serial.print("InterruptCounter_temp!!");
//    }
//    else if (interruptCounter_temp < 0 && interruptCounter_ldr > 0) {
//      Serial.print("InterruptCounter_temp!!");
//    }
//    else{
//      Serial.print("Idel Loop");
//    }
}
