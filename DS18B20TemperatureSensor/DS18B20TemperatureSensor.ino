#include <DallasTemperature.h>

#include <OneWire.h>

#define ONE_WIRE_BUS 2
#define DS_HIGH 3

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
void setup() {
  // put your setup code here, to run once:
  pinMode(DS_HIGH, OUTPUT);
  digitalWrite(DS_HIGH, HIGH);
  Serial.begin(9600);
  Serial.println("Dallas Temperature IC Control Library Demo");
  // Start up the library
  sensors.begin();
    Serial.print("Count of dev"); 
  Serial.println(sensors.getDeviceCount());
  Serial.println(sensors.getDS18Count());
}

void loop() {
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
  Serial.print("Temperature for the device 1 (index 0) is: ");
  Serial.println(sensors.getTempCByIndex(0));
}
