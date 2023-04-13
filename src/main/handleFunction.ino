void handledatetime(AsyncWebServerRequest* request) {
  String jsonResponse = readJSON();
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
