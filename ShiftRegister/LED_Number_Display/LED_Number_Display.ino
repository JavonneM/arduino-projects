int PIN_DS = 6;
int PIN_OE = 9;
int PIN_STORE = 10;
int PIN_CLOCK = 11;

//int UPDATE_CYCLE = 100;
int BRIGHTNESS = 250;
int counter;
int DISPLAY_DURATION = 3000;
int DELAY = 1;


#define DEBUG 1

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

//Outputs 7 -> 0 
byte data = 0b01111111;
//byte data = 0b00000001;
// the setup routine runs once when you press reset:
void setup() {
  // declare pin 9 to be an output:
  pinMode(PIN_OE, OUTPUT);
  pinMode(PIN_DS, OUTPUT);
  pinMode(PIN_CLOCK, OUTPUT);
  pinMode(PIN_STORE, OUTPUT);
  counter = 0;
  #if DEBUG == 1
  Serial.begin(9600);
  #endif
  Serial.print(0b10000000 >> 1); Serial.print("\n");
  Serial.print(0b10000000 >> 8); Serial.print("\n");
//  data = get_byte_for_digit(digit, '2' , false);
  Serial.print("Normal data: ");
//  Serial.println(data);
//  data = ~get_byte_for_digit(digit, '2' , false);
  Serial.print("Inverted data: ");
//  Serial.println(data);

  // Digit setup
  randomSeed(0);
  int digits_size = 4;
  int index = 0;
  char *buff = malloc(digits_size * sizeof(char));
  memset(buff, 0, digits_size * sizeof(char));
  
  digits.count = digits_size;
  digits.digits = malloc(digits_size * sizeof(led_digit));
  digits.driver = malloc(digits_size * sizeof(unsigned char));
  for (index = 0; index < digits_size; index++) {
    digits.digits[index] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
    digits.driver[index] = (1 << index);
  }

//  memset(buff, 0, 8 * sizeof(char));
//  float_to_char(4.324, buff, 8);
//  memset(buff, 0, 8 * sizeof(char));
//  float_to_char(123456.324, buff, 8);
}

// the loop routine runs over and over again forever:
void loop() {
  // Get number
//  float value = (random(10000) * 1.0);
//  led_write(value);

  analogWrite(PIN_OE, 255);
//  writeData(~data);
  writeData(data);
  writeData(0b11111110);

  analogWrite(PIN_OE, 255 - BRIGHTNESS);
//  data = updateData(data);
  
//  counter++;
  delay(DISPLAY_DURATION); 
//  analogWrite(PIN_OE, 0);
}

byte updateData(byte bytePattern) {
  byte newData = bytePattern;
  byte toAddBit = (newData & 0b10000000) >> 7;
  
  newData = (newData << 1) | toAddBit;
  
  Serial.print("Data "); Serial.print(bytePattern); Serial.print("\n");
  Serial.print("Bit to add "); Serial.print(toAddBit); Serial.print("\n");
//  Serial.print(data);
  Serial.print("\n");
  Serial.print("New Data ");
  Serial.print(newData);
  Serial.print("\n");
  return newData;
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


//led digit stuff

void led_write(float value) {
  char buff[8] = {};
  
  float_to_char(value, buff, 8);
  byte data;
  byte enable_digit_data;
  int i = 0;
  int char_index = 0;
  for (i = 0; i < 4; i++ ) {
    enable_digit_data = digits.driver[i];
    while (buff[char_index] == '.') {
      char_index += 1;
      if (char_index >= 8) {
        break;
      }      
    }
    if (char_index >= 8) {
      break;
    }
    bool has_decimal = char_index + 1 < 8 && buff[char_index + 1] == '.';
    
    data = get_byte_for_digit(digits.digits[i], buff[char_index], has_decimal);
    
    analogWrite(PIN_OE, 255);
//    writeData(enable_digit_data);
//    writeData(~data); // this is the actual digit
    writeData(0x00000000);
    writeData(0x00000000); // this is the actual digit
    analogWrite(PIN_OE, 255 - BRIGHTNESS);
    delay(DISPLAY_DURATION); 
  }
  delay(1000);
}

void float_to_char(float value, char* buff, int chars) {
  // Preprocess value
  float mutated = value;

  while(mutated > 1000) {
    mutated /= 10;
  }
  // now mutated should be less that 1000


  
  String myString = String(mutated);
  myString.toCharArray(buff, chars);
  int i;
  for (i = 0; i < 8; i++) {
    Serial.print('|');
    Serial.print(buff[i]);
  }
  Serial.println();
}



byte get_byte_for_digit(struct led_digit digit, unsigned char character, bool enable_decimal) {
  byte data = 0;
  switch (character) {
    case '0': {
      data = (1 << digit.BOTTOM) | (1 << digit.BOTTOM_LEFT) | 
        (1 << digit.BOTTOM_RIGHT) | (1 <<digit.TOP) | 
        (1 << digit.TOP_LEFT) | (1 << digit.TOP_RIGHT);
      break;
    }
    case '1': {
      data =   (1 << digit.BOTTOM_RIGHT) | (1 << digit.TOP_LEFT);
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
        (1 << digit.TOP_LEFT);
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
  return data;
}



