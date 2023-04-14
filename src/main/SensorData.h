#ifndef SensorData
#define SensorData
#include <Wire.h>
#include "RTClib.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <BH1750.h>

#define SEALEVELPRESSURE_HPA (1013.25)
#define TRIGGER_PIN 4  //ตั้งขา Trig เป็น Output (D4)
#define ECHO_PIN 5   //ตั้งขา Echo เป็น Input (D5)

RTC_DS3231 rtc;
BH1750 lightMeter;
Adafruit_BME280 bme;
unsigned long delayTime;

void rtcCheck(){
  rtc.begin();
  Serial.println("rtcCheck!");
  if (!rtc.begin()) {
    Serial.println("RTC is not working!");
  }
  
  /*if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    rtc.adjust(DateTime((__DATE__), (__TIME__)));
  }*/
  rtc.adjust(DateTime((__DATE__), (__TIME__)));
  }
  
  
void bmecheck(){
  Serial.println(F("BME280 test"));
  bool status;
  status = bme.begin(0x76);  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
//    while (1);
  }

  Serial.println("-- Default Test --");
  delayTime = 1000;}


int motioncheck(void){
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  
  int distance = duration*0.034/2;
  return distance;
  }
  
String getRtcString(void) {
  DateTime now = rtc.now();
  char Time[100];
  //rtcCheck();
  sprintf(Time, "%02d:%02d",
          now.hour(), now.minute());
  return String(Time);
}

int lightcheck(void){
  Wire.begin(21, 22); // เริ่มต้นใช้งาน I2C โดยระบุ GPIO 21 และ 22
  lightMeter.begin(); // ตั้งค่าให้ object lightMeter เชื่อมต่อกับ BH1750
  uint16_t lux = lightMeter.readLightLevel(); // อ่านค่าความสว่างจาก BH1750
  return lux;
  }
  
struct GetData{
  String Time;
  int distance;
  float temp;
  int light;
  float humidity; 
};



GetData AllSenData(){
  GetData datasensor;
  //bmecheck();
  String Time = getRtcString();
  int distance = motioncheck();
  int light = lightcheck();
  float temp = bme.readTemperature();
  float humidity =  bme.readHumidity();

  
  datasensor.distance = distance;
  datasensor.Time = Time;
  datasensor.temp = temp;
  datasensor.humidity = humidity;
  datasensor.light = light;
  
  return datasensor;
}


#endif
