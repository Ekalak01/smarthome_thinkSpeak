#include <WiFi.h>
#include "SPIFFS.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "updatejson.h"

////////////////////////////////////////////////////
#include "mqtt_secrets.h"
#include <HTTPClient.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <BH1750.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_GFX.h>
#include "RTClib.h"
#include <ESP_Line_Notify.h>
#include "SensorData.h"

#define SEALEVELPRESSURE_HPA (1013.25)
#define LINE_TOKEN "5Awu3lpOGJwOYNYI2ZGfzqLXM5kCuQkjnuJUCh8Hil7"

const char* server_mqtt = "mqtt3.thingspeak.com";
const char* channelID = "2105573";
const char* mqttUserName = SECRET_MQTT_USERNAME;
const char* mqttPass = SECRET_MQTT_PASSWORD;
const char* clientID = SECRET_MQTT_CLIENT_ID;

// Set web server port number to 80
AsyncWebServer server(80);

WiFiClient client;
PubSubClient mqtt(client);
LineNotifyClient line;

///////////////////////////////////////////////////


// Replace with your network credentials
const char* ssid = "540_ROOM";
const char* password = "12345555";



bool checkBoxValue = false;

unsigned long startMillis;
unsigned long totalPersons = 0;
int sensorState = 0;

void countpeople(int distance) {

  if (distance > 0 && distance <= 20) {
    if (sensorState == 0) {
      sensorState = 1;
      totalPersons++;
    }
  }
  if (distance > 20 && sensorState == 1) {
    sensorState = 0;
  }
  unsigned long currentMillis = millis();
  if (currentMillis - startMillis >= 86400000) { // 1 day in milliseconds
    Serial.print("Total persons today: ");
    Serial.println(totalPersons);
    startMillis = currentMillis;
    totalPersons = 0;
  }
  Serial.println(totalPersons);
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
  // Mqtt
  mqtt.setServer(server_mqtt, 1883);

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

  // Line Connect
  line.reconnect_wifi = true;
  line.token = LINE_TOKEN;

  // Sensor Check
  bmecheck();
  rtcCheck();

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
  server.on("/check-on-off", HTTP_GET, switchstate);

  // Set up the route for handling on/off switch control
  server.on("/switchstate", HTTP_GET, handleGetSwitchState);

  server.on("/handledatetime", HTTP_GET, handledatetime);

  // Start web server
  server.begin();
}

////////////////////Interrupts Part////////////////////////
int interruptCounter_temp;
int interruptCounter_ldr;
//String currentTime_RTC "16:00";
void interrupts_iram() {
  bool stateCurrent = currentStateJSON();
  String currentTime_RTC = getRtcString();
  SensorData_re sensorData_rex = readJSON_return();
  String tempStart = sensorData_rex.tempStart;
  String tempEnd = sensorData_rex.tempEnd;
  String ldrStart = sensorData_rex.ldrStart;
  String ldrEnd = sensorData_rex.ldrEnd;
  if (stateCurrent == 1) {
    Serial.println("tempStart"+tempStart);
    Serial.println("currentTime_RTC"+currentTime_RTC);
    if (tempStart == currentTime_RTC) {
      interruptCounter_temp = 1;
      Serial.print("interruptCounter_temp_tempStart  ");
      Serial.println(interruptCounter_temp);
    }
    if (tempEnd == currentTime_RTC) {
      interruptCounter_temp = 0;
      Serial.print("interruptCounter_temp_tempEnd  "+interruptCounter_temp);
      Serial.println(interruptCounter_temp);
    }
    if (ldrStart  == currentTime_RTC) {
      interruptCounter_ldr = 1;
      Serial.print(interruptCounter_ldr);
    }
    if (ldrEnd == currentTime_RTC) { // fix typo here
      interruptCounter_ldr = 0;
      Serial.print(interruptCounter_ldr);
    }
  }
  else {
    interruptCounter_temp = 0;
    interruptCounter_ldr = 0;
  }
  
}

//////////////////////////////////////////////////////////
long lastTime = millis();

void reconnectMqtt() {
  //Check if MQTT Server is connected
  if (!mqtt.connected()) {
    Serial.println("Connecting to MQTT Broker.");
    mqtt.connect(clientID, mqttUserName, mqttPass);
    while (!mqtt.connected()) {
      delay(500);
      Serial.print(".");
    }
  }
}

void loop() {
  // Get Data
  GetData datasensor = AllSenData();
  int distance =  datasensor.distance;
  String Time = datasensor.Time;
  float temp = datasensor.temp;
  float humidity = datasensor.humidity;
  int light = datasensor.light;

  countpeople(distance);
  /*static unsigned long last_notify_time = 0;
  unsigned long current_time = millis();
  if (current_time - last_notify_time >= 5 * 60 * 1000) {
    last_notify_time = current_time;
    LineNoti(totalPersons, 3); //ส่งแค่ totalPersons
  }*/

  interrupts_iram();
  Serial.print("interrupts_iram_temp ");
  Serial.println(interruptCounter_temp);
  if (interruptCounter_temp > 0 && interruptCounter_ldr > 0) {
    reconnectMqtt();
//    Serial.print("InterruptCounter_temp and InterruptCounter_ldr!!");
    static unsigned long last_notify_time = 0;
    unsigned long current_time = millis();
    if (current_time - last_notify_time >= 15 * 60 * 1000) {
      last_notify_time = current_time;
      LineNoti(totalPersons, 3);
      LineNoti(light,1);
      LineNoti(temp,2);
      LineNoti(humidity,4);
      String topicString = "channels/" + String(channelID) + "/publish";
      String dataString = "&field1=" + String(light) + "&field2=" + String(temp) + "&field3=" + String(humidity);
      mqtt.publish(topicString.c_str(), dataString.c_str());
    }
    
  }
  if (interruptCounter_temp > 0 && interruptCounter_ldr == 0) {
    reconnectMqtt();
    Serial.print("InterruptCounter_temp!!");
    static unsigned long last_notify_time = 0;
    unsigned long current_time = millis();
    if (current_time - last_notify_time >= 15 * 60 * 1000) {
      last_notify_time = current_time;
      LineNoti(totalPersons, 3);
      LineNoti(temp,2);
      LineNoti(humidity,4);
      String topicString = "channels/" + String(channelID) + "/publish";
      String dataString = "&field2=" + String(temp) + "&field3=" + String(humidity);
      mqtt.publish(topicString.c_str(), dataString.c_str());
    }
    
  }
  if (interruptCounter_temp == 0 && interruptCounter_ldr > 0) {
    reconnectMqtt();
    Serial.print("InterruptCounter_ldr!!");
    static unsigned long last_notify_time = 0;
    unsigned long current_time = millis();
    if (current_time - last_notify_time >= 15 * 60 * 1000) {
      last_notify_time = current_time;
      LineNoti(totalPersons, 3);
      LineNoti(light,1);
      String topicString = "channels/" + String(channelID) + "/publish";
      String dataString = "&field1=" + String(light);
      mqtt.publish(topicString.c_str(), dataString.c_str());
    }
    
  }
}
