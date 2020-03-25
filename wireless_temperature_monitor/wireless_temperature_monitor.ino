
#include <OneWire.h>
#include <DallasTemperature.h>

#define UPDATE_INTERVAL (2000)

// GPIO where the DS18B20 is connected to
const int oneWireBus = 13;

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);

uint32_t current_millis = 0;
uint32_t prev_millis = 0;

void setup()
{
  // Start the Serial Monitor
  Serial.begin(115200);
  // Start the DS18B20 sensor
  sensors.begin();
}

void loop()
{
  current_millis = millis();
  
  if (current_millis - prev_millis > UPDATE_INTERVAL)
  {
    sensors.requestTemperatures();
    float celcius = sensors.getTempCByIndex(0);
    Serial.print(celcius);
    Serial.println("ºC");
  }
}
