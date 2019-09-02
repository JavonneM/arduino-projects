#include <DallasTemperature.h>
#include <OneWire.h>
// Definitions and setup for DS
#define PIN_DS_ONE_WIRE_BUS 2
#define PIN_DS_HIGH 3
#define MAX_ANALOG_READ 1024.0
#define MAX_VIN 5
#define BAD_READ -127

OneWire oneWire(PIN_DS_ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Definitions and setup for EC Meter
#define PIN_EC_READ A6
#define PIN_EC_LOW A0
#define PIN_EC_HIGH A5

#define R1 1000
#define Ra 25 //Internal Resistance of powering Pins

#define PPMconversion 0.7
#define TemperatureCoef 0.019
#define K 2.88 // K value (Cell Constant)

// Definitions for float switch
#define PIN_FLOAT_SWITCH_HIGH 5
#define PIN_FLOAT_SWITCH_READ 4

// Variables 
float temperature = 0;
float rawECValue = 0;
float EC = 0;
float EC25 = 0;
float ppm = 0;

void setup() {
  // Setup Float Switch
  pinMode(PIN_FLOAT_SWITCH_HIGH, OUTPUT);
  pinMode(PIN_FLOAT_SWITCH_READ, INPUT);
  digitalWrite(PIN_FLOAT_SWITCH_HIGH, HIGH);
  // Setup EC Pins
  pinMode(PIN_EC_READ, INPUT);
  pinMode(PIN_EC_HIGH, OUTPUT);
  pinMode(PIN_EC_LOW, OUTPUT);
  digitalWrite(PIN_EC_HIGH, HIGH);
  digitalWrite(PIN_EC_LOW, HIGH);
  // put your setup code here, to run once:
  pinMode(PIN_DS_HIGH, OUTPUT);
  digitalWrite(PIN_DS_HIGH, HIGH);
  Serial.begin(9600);
  // Start up the library
  sensors.begin();
}

void loop() {
  temperature = getTemperature();  
  readEC();
  computeECValue(rawECValue, temperature);
  getFloatSwitch();
  displayResults();
  delay(5000);
}

float readEC() {
  digitalWrite(PIN_EC_HIGH, HIGH);
  rawECValue = analogRead(PIN_EC_READ);
  rawECValue = analogRead(PIN_EC_READ);
  digitalWrite(PIN_EC_HIGH, LOW);
  Serial.print("Raw Analog value: "); Serial.println(rawECValue);
}

float getTemperature() {
  sensors.requestTemperatures();
  temperature = sensors.getTempCByIndex(0);
  Serial.print("Temperature: "); Serial.println(temperature);
  return temperature;
}

void computeECValue(float rawECValue, float temperature) {
  // Converts to EC
  float vDrop = (MAX_VIN * rawECValue)/MAX_ANALOG_READ;
  float r = (vDrop * R1)/MAX_VIN;
  r = r - Ra;
  EC = 1000/(r * K);
  
  // temp compensation

  EC25 = EC / (1 + TemperatureCoef * (temperature - 25.0));
  ppm = EC25 * (PPMconversion * 1000);
}

bool getFloatSwitch() {
  Serial.print("Float switch status: ");
  Serial.println(digitalRead(PIN_FLOAT_SWITCH_READ));
}

void displayResults() {
  Serial.print("**Temperature: "); Serial.println(temperature);
  Serial.print("*****EC   value: "); Serial.println(EC);
  Serial.print("*****EC25 value: "); Serial.println(EC25);
  Serial.print("****PPM value: "); Serial.println(ppm);
  Serial.println();
}
