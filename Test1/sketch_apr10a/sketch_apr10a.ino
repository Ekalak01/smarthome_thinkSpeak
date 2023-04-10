#include <WiFiClientSecure.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

// WiFi credentials
const char* ssid = "540_ROOM";
const char* password = "12345555";

// ThingSpeak channel settings
const char* channel_id = "1850223";
const char* api_key = "V9P22LSB1ZDVJK8O";
const String apiKey = "V9P22LSB1ZDVJK8O";

// Replace with your DHT type and pin
const int DHTType = DHT11;
const int DHTPin = 27;
DHT dht(DHTPin, DHTType);

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");

  // Initialize DHT sensor
  dht.begin();
}

void loop() {
  // Read temperature and humidity from DHT sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Send data to ThingSpeak
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read data from DHT sensor!");
    return;
  }
  HTTPClient http;
  String url = "https://api.thingspeak.com/update?api_key=" + apiKey + "&field5=" + String(temperature) + "&field6=" + String(humidity);
  http.begin(url);
  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) {
    Serial.println("Data sent to ThingSpeak!");
  } else {
    Serial.printf("Failed to send data to ThingSpeak, error code: %d\n", httpResponseCode);
  }
  http.end();

  delay(20000);
}
