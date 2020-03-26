/*
   Created by Carl Erick Rowan

   Email: carlerickrowan@gmail.com

   Github: https://github.com/carlerickrowan/Covid19-Patient-Wireless-Monitor-Kit

   Facebook: fb.com/fastproto

*/

#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

// Replace with your network credentials
#include <OneWire.h>
#include <DallasTemperature.h>

String patient_number = "/0001";        //change this based on the device number

#define UPDATE_INTERVAL (10000)
#define BATTERY_PIN (A0)

#define FIREBASE_HOST "FILL THIS!" //Do not include https:// in FIREBASE_HOST
#define FIREBASE_AUTH "FILL THIS!"

#define WIFI_SSID "FILL THIS!"
#define WIFI_PASSWORD "FILL THIS!"

// GPIO where the DS18B20 is connected to
const int oneWireBus = 13;

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);

//Define Firebase Data object
FirebaseData firebaseData;

uint32_t current_millis = 0;
uint32_t prev_millis = 0;

uint16_t adc_count = 0;
float battery_voltage = 0;

void setup()
{
  Serial.begin(115200);

  sensors.begin();
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(firebaseData, "tiny");
}

void loop()
{
  ArduinoOTA.handle();

  current_millis = millis();

  sensors.requestTemperatures();
  float celcius = sensors.getTempCByIndex(0);

  adc_count = analogRead(BATTERY_PIN);
  battery_voltage = adc_count * 0.0048;
  
  Serial.print("OTA Mode: ");
  Serial.print(celcius);
  Serial.print("ºC . ");

  Serial.print("Battery: ");
  Serial.print(battery_voltage);
  Serial.println(" V");
  
  if (current_millis - prev_millis > UPDATE_INTERVAL)
  {
    adc_count = analogRead(BATTERY_PIN);
    battery_voltage = adc_count * 0.0048;
    prev_millis = current_millis;
    Serial.println("Send Firebase!");
    Firebase.setFloat(firebaseData, patient_number + "/Temperature", celcius);
    Firebase.setFloat(firebaseData, patient_number + "/Battery", battery_voltage);
  }
}
