#include <DallasTemperature.h>
#include <OneWire.h>
// Definitions and setup for DS
#define PIN_DS_ONE_WIRE_BUS 2
#define PIN_DS_HIGH 3
#define MAX_ANALOG_READ 1023.0
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

// LED NUMBER DISPLAY
int PIN_DS = 12;
int PIN_OE = 8;
int PIN_STORE = 9;
int PIN_CLOCK = 10;
int PIN_CLR = 11;

//int UPDATE_CYCLE = 100;
int BRIGHTNESS = 250;
int counter;
int DISPLAY_DURATION = 1;
int DELAY = 0;


#define DEBUG 0

// Prototypes
void led_write(float value);
unsigned char get_byte_for_digit(struct led_digit digit, unsigned char character, bool enable_decimal);

// This is to represent a digit
typedef struct led_digit {
  unsigned char DECIMAL;
  unsigned char BOTTOM;
  unsigned char BOTTOM_LEFT;
  unsigned char BOTTOM_RIGHT;
  unsigned char MID;
  unsigned char TOP;
  unsigned char TOP_LEFT;
  unsigned char TOP_RIGHT;
} led_digit;
// For the array
typedef struct led_digit_array {
  unsigned char* driver;
  led_digit* digits;
  int count;
} led_digit_array;

// Structure
led_digit_array digits;
//led_digit digit;
// LED NUMBER DISPLAY END


// time of last read
unsigned long last_read_time;
void setup() {
  last_read_time = millis();
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
  // Start up the library
  sensors.begin();
  
  // LED NUMBER DISPLAY
    pinMode(PIN_OE, OUTPUT);
  pinMode(PIN_DS, OUTPUT);
  pinMode(PIN_CLOCK, OUTPUT);
  pinMode(PIN_STORE, OUTPUT);
  pinMode(PIN_CLR, OUTPUT);
  counter = 0;
  #if DEBUG == 1
  Serial.begin(9600);
  #endif
  
  // Digit setup
  int digits_size = 4;
  int index = 0;
  char *buff = malloc(digits_size * sizeof(char));
  memset(buff, 0, digits_size * sizeof(char));
  
  digits.count = digits_size;
  digits.digits = malloc(digits_size * sizeof(led_digit));
  digits.driver = malloc(digits_size * sizeof(unsigned char));
  for (index = 0; index < digits_size; index++) {
    digits.digits[index] = {0x02, 0x03, 0x04, 0x01, 0x0, 0x07, 0x06, 0x05};
    digits.driver[index] = (1 << index);
  }

  digitalWrite(PIN_CLR, HIGH);
  // LED NUMBER DISPLAY END
}
void loop() {
  unsigned long now = millis();
  if ((now - last_read_time) > 2500) {
    temperature = getTemperature();  
    readEC();
    computeECValue(rawECValue, temperature);
    getFloatSwitch();
//    displayResults();
    last_read_time = millis();
    Serial.println(now);
  }
  // Do shift register things
  led_write(EC25);
  counter++;
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


void writeData(byte data) {
  int i;
  unsigned int mask = 128;
  for (i = 7; i >= 0; i--) {

    writeBit(data & mask);
     delayMicroseconds(DELAY);
    pulseClock();
     delayMicroseconds(DELAY);
    mask = mask >> 1;
  }
}
void writeBit(int data) {
  if (data == 0) {
    digitalWrite(PIN_DS, LOW);        
  } else {
    digitalWrite(PIN_DS, HIGH);
  }

}
void pulseClock() {
  digitalWrite(PIN_CLOCK, HIGH);
  delayMicroseconds(DELAY);   
  digitalWrite(PIN_CLOCK, LOW);
  digitalWrite(PIN_DS, LOW);
  digitalWrite(PIN_STORE, HIGH);
  delayMicroseconds(DELAY);   
  digitalWrite(PIN_STORE, LOW);
}


// LED NUMBER DISPLAY
void led_write(float value) {
  char buff[8] = {};
  
  float_to_char(value, buff, 8);
  byte data;
  byte enable_digit_data;
  int i = 0;
  int char_index = 7;
  for (i = 0; i < digits.count; i++ ) {
    enable_digit_data = digits.driver[i];
    while (char_index >= 0 && (buff[char_index] == '.' || buff[char_index] == 0 )) {
      char_index -= 1;
    }
    if (char_index < 0) {
      break;
    }
    bool has_decimal = char_index + 1 < 8 && buff[char_index + 1] == '.';
//    Serial.print("Getting digit for: "); Serial.print(buff[char_index]); Serial.print(" at index: "); Serial.println(char_index);
    data = get_byte_for_digit(digits.digits[i], buff[char_index], has_decimal);
//    analogWrite(PIN_OE, 255);
    digitalWrite(PIN_OE, HIGH);
    writeData(enable_digit_data);
    writeData(~data); // this is the actual digit
//    writeData(0b11111111);
//    writeData(0b11111111); // this is the actual digit
    digitalWrite(PIN_OE, LOW);
//    analogWrite(PIN_OE, 255 - BRIGHTNESS);
    delay(DISPLAY_DURATION); 
    char_index--;
  }
//  delay(1000);
}

void float_to_char(float value, char* buff, int chars) {
  // Preprocess value
  float mutated = value;
  String myString = String(mutated);
  myString.toCharArray(buff, chars);
}



byte get_byte_for_digit(struct led_digit digit, unsigned char character, bool enable_decimal) {
  byte data = 0;
//  Serial.print("Input: "); Serial.println(static_cast<char>(character));
  switch (character) {
    case '0': {
      data = (1 << digit.BOTTOM) | (1 << digit.BOTTOM_LEFT) | 
        (1 << digit.BOTTOM_RIGHT) | (1 <<digit.TOP) | 
        (1 << digit.TOP_LEFT) | (1 << digit.TOP_RIGHT);
      break;
    }
    case '1': {
      data =   (1 << digit.BOTTOM_RIGHT) | (1 << digit.TOP_RIGHT);
      break;
    }
    case '2': {
      data =  (1 << digit.BOTTOM) | (1 << digit.BOTTOM_LEFT) | 
        (1 << digit.MID) | (1 << digit.TOP) | 
        (1 << digit.TOP_RIGHT);
      break;
    }
    case '3': {
      data =  (1 << digit.BOTTOM) | (1 << digit.BOTTOM_RIGHT) |
        (1 << digit.MID) | (1 << digit.TOP) | 
        (1 << digit.TOP_RIGHT);
      break;
    }
    case '4': {
      data =  (1 << digit.BOTTOM_RIGHT) | (1 << digit.MID) | 
        (1 << digit.TOP_LEFT) | (1 << digit.TOP_RIGHT);
      break;
    }
    case '5': {
      data =  (1 << digit.BOTTOM) | (1 << digit.BOTTOM_RIGHT) | 
        (1 << digit.TOP) | (1 << digit.TOP_LEFT) | (1 << digit.MID);
      break;
    }
    case '6': {
      data =  (1 << digit.BOTTOM) | (1 << digit.BOTTOM_LEFT) | 
        (1 << digit.BOTTOM_RIGHT) | (1 <<digit.TOP) | 
        (1 << digit.TOP_LEFT) | (1 << digit.MID);
      break;
    }
    case '7': {
      data =  (1 << digit.BOTTOM_RIGHT) | (1 << digit.TOP) | 
        (1 << digit.TOP_LEFT) | (1 << digit.TOP_RIGHT);
      break;
    }
    case '8': {
      data =  (1 << digit.BOTTOM) | (1 << digit.BOTTOM_LEFT) | 
        (1 << digit.BOTTOM_RIGHT) | (1 <<digit.TOP) | 
        (1 << digit.TOP_LEFT) | (1 << digit.TOP_RIGHT) |
        (1 << digit.MID);
      break;
    }
    case '9': {
      data =  (1 << digit.MID) | (1 << digit.BOTTOM_RIGHT) | 
       (1 << digit.TOP) | (1 << digit.TOP_LEFT) | (1 << digit.TOP_RIGHT);
      break;
    }
  }
  if (enable_decimal) {
    data = data | (1 << digit.DECIMAL);
  }
//  Serial.print("Data: "); Serial.println(data);
  return data;
}
// LED NUMBER DISPLAY END
