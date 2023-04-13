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

const char* server = "mqtt3.thingspeak.com";
const char* channelID = "2105573";
const char* mqttUserName = SECRET_MQTT_USERNAME;
const char* mqttPass = SECRET_MQTT_PASSWORD;
const char* clientID = SECRET_MQTT_CLIENT_ID;

#define SSID "540_ROOM"
#define PASSWORD "12345555"

//BH1750 lightMeter; // ประกาศ object lightMeter ของ BH1750 class
int LED_BUILTIN = 2;
WiFiClient client;
PubSubClient mqtt(client);
LineNotifyClient line;

unsigned long startMillis;
unsigned long totalPersons = 0;
int sensorState = 0;

void countpeople(int distance){
  
  if (distance > 0 && distance <= 10) {
    if (sensorState == 0){
      sensorState = 1;
      totalPersons++;
      }  
  }
  if (distance > 10 && sensorState == 1){
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
  Serial.begin(115200);
  WiFi.begin(SSID, PASSWORD);
  mqtt.setServer(server, 1883);
  Serial.printf("WiFi connecting to %s\n",  SSID);
  while(WiFi.status() != WL_CONNECTED) { Serial.print("."); delay(400); }
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP());
  line.reconnect_wifi = true;
  line.token = LINE_TOKEN;
  bmecheck();
  rtcCheck();
}
void loop() {
  GetData datasensor = AllSenData();
  int distance =  datasensor.distance;
  String Time = datasensor.Time;
  float temp = datasensor.temp;
  float humidity = datasensor.humidity;
  int light = datasensor.light;
  //
  countpeople(distance);
  static unsigned long last_notify_time = 0;
  unsigned long current_time = millis();
  if (current_time - last_notify_time >= 5 * 60 * 1000) {
    last_notify_time = current_time;
    LineNoti(distance,light,temp);}
    
  //Check if MQTT Server is connected
  if (!mqtt.connected()) {
    Serial.println("Connecting to MQTT Broker.");
    mqtt.connect(clientID, mqttUserName, mqttPass);
    while (!mqtt.connected()) {
      delay(500);
      Serial.print(".");
    }
  }
  unsigned static int half_second_count = 0;
  //Post http get to ThingSpeak Every 5 minutes
  if (half_second_count >= 30) {
    //Reset Timer
    half_second_count = 0;
    
    String topicString = "channels/" + String(channelID) + "/publish";
    String dataString = "&field1=" + String(light) + "&field2=" + String(temp) + "&field3=" + String(humidity);
    mqtt.publish(topicString.c_str(), dataString.c_str());
    
  }
  Serial.println(Time);
  Serial.println(distance);
  Serial.println(temp);
  Serial.println(humidity);
  Serial.println(light);
  delay(500);
  half_second_count++;
}
