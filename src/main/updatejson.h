#ifndef updatejson
#define updatejson
#include "Arduino.h"
#include <ArduinoJson.h>
#include <FS.h>

String readJSON() {
  // Open the JSON file for reading
  if (!SPIFFS.exists("/state.json")) {
    Serial.println("Creating initial state.json");
    return "{}";
  }
  File file = SPIFFS.open("/state.json", "r");
  if (!file) {
    Serial.println("Failed to open state.json for reading");
    return "{}";
  }

  // Read the content of the JSON file
  String jsonContent = file.readString();

  // Close the file
  file.close();

  return jsonContent;
}

bool currentStateJSON() {
  // Open the JSON file for reading
  if (!SPIFFS.exists("/state.json")) {
    Serial.println("Creating initial state.json");
    return false;
  }
  File file = SPIFFS.open("/state.json", "r");
  if (!file) {
    Serial.println("Failed to open state.json for reading");
    return false;
  }

  // Parse the JSON file
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    Serial.println("Failed to parse state.json");
    file.close();
    return false;
  }

  // Close the file
  file.close();

  // Get the values from the JSON object
  JsonObject root = doc.as<JsonObject>();
  bool state = root["state"];
  String tempStart = root["temp"]["start"];
  String tempEnd = root["temp"]["end"];
  String ldrStart = root["ldr"]["start"];
  String ldrEnd = root["ldr"]["end"];

  return state;
}

struct SensorData{
  String tempStart;
  String tempEnd;
  String ldrStart;
  String ldrEnd;
};
SensorData readJSON_return() {
  SensorData sensorData;
  // Open the JSON file for reading
  if (!SPIFFS.exists("/state.json")) {
    Serial.println("Creating initial state.json");
  }
  File file = SPIFFS.open("/state.json", "r");
  if (!file) {
    Serial.println("Failed to open state.json for reading");
    return sensorData;
  }

  // Parse the JSON file
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    Serial.println("Failed to parse state.json");
    file.close();
    return sensorData;
  }

  // Close the file
  file.close();

  // Get the values from the JSON object
  JsonObject root = doc.as<JsonObject>();
  bool state = root["state"];
  String tempStart = root["temp"]["start"];
  String tempEnd = root["temp"]["end"];
  String ldrStart = root["ldr"]["start"];
  String ldrEnd = root["ldr"]["end"];
  return sensorData;
}

void readJSONFile() {
  // Open the JSON file for reading
  if (!SPIFFS.exists("/state.json")) {
    Serial.println("Creating initial state.json");
  }
  File file = SPIFFS.open("/state.json", "r");
  if (!file) {
    Serial.println("Failed to open state.json for reading");
    return;
  }

  // Parse the JSON file
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    Serial.println("Failed to parse state.json");
    file.close();
    return;
  }

  // Close the file
  file.close();

  // Get the values from the JSON object
  JsonObject root = doc.as<JsonObject>();
  bool state = root["state"];
  String tempStart = root["temp"]["start"];
  String tempEnd = root["temp"]["end"];
  String ldrStart = root["ldr"]["start"];
  String ldrEnd = root["ldr"]["end"];

  // Print the values
  Serial.println("/////////JSON READ//////////");
  Serial.println("State: " + String(state ? "true" : "false"));
  Serial.println("Temperature Sensor Start Time: " + tempStart);
  Serial.println("Temperature Sensor End Time: " + tempEnd);
  Serial.println("LDR Sensor Start Time: " + ldrStart);
  Serial.println("LDR Sensor End Time: " + ldrEnd);
  Serial.println("///////////END//////////");
}

void updateJSONFile(const char* key, bool switchState, const String& startTime, const String& endTime) {
  DynamicJsonDocument doc(1024);
  JsonObject root;

  // Check if the JSON file exists
  if (SPIFFS.exists("/state.json")) {
    // Open the JSON file for reading
    File file = SPIFFS.open("/state.json", "r");
    if (!file) {
      Serial.println("Failed to open state.json");
      return;
    }

    // Parse the JSON file
    DeserializationError error = deserializeJson(doc, file);
    if (error) {
      Serial.println("Failed to parse state.json");
      file.close();
      return;
    }

    // Close the file
    file.close();

    // Get the JSON object
    root = doc.as<JsonObject>();
  } else {
    // Create the JSON object
    root = doc.to<JsonObject>();

    // Initialize the sub-objects for temp and ldr
    root.createNestedObject("temp");
    root.createNestedObject("ldr");
  }

  // Update the JSON object with new values
  if (String(key) != "switch") {
    root[key]["start"] = startTime;
    root[key]["end"] = endTime;
  }
  root["state"] = switchState;

  // Open the JSON file for writing
  File file = SPIFFS.open("/state.json", "w");
  if (!file) {
    Serial.println("Failed to open state.json for writing");
    return;
  }

  // Serialize and write the updated JSON object to the file
  if (serializeJson(doc, file) == 0) {
    Serial.println("Failed to write to state.json");
  }

  // Close the file
  file.close();
}
#endif
