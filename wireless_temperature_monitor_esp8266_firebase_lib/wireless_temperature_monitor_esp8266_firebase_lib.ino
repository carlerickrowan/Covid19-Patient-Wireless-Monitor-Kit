/*
   Created by Carl Erick Rowan

   Email: carlerickrowan@gmail.com

   Github: https://github.com/carlerickrowan/Covid19-Patient-Wireless-Monitor-Kit

   Facebook: fb.com/fastproto

*/

//FirebaseESP8266.h must be included before ESP8266WiFi.h
#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#define UPDATE_INTERVAL (10000)

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

String patient_number = "/0001";

void setup()
{
  // Start the Serial Monitor
  Serial.begin(115200);
  // Start the DS18B20 sensor
  sensors.begin();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }

  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

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
  current_millis = millis();

  sensors.requestTemperatures();
  float celcius = sensors.getTempCByIndex(0);
  Serial.print(celcius);
  Serial.println("ºC");

  if (current_millis - prev_millis > UPDATE_INTERVAL)
  {
    prev_millis = current_millis;

    Firebase.setFloat(firebaseData, patient_number + "/Temperature", celcius);
  }
}
