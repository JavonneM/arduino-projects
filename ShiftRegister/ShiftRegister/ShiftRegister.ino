int PIN_DS = 6;
int PIN_OE = 9;
int PIN_STORE = 10;
int PIN_CLOCK = 11;

//int UPDATE_CYCLE = 100;
int BRIGHTNESS = 50;
int counter;
int DISPLAY_DURATION = 100;
int DELAY = 1;

#define DEBUG 1
//Outputs 7 -> 0 
byte data = 0b01010101;
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
}

// the loop routine runs over and over again forever:
void loop() {
    analogWrite(PIN_OE, 255);
    writeData();
    Serial.print("ENABLING"); Serial.print("\n");
    analogWrite(PIN_OE, 255 - BRIGHTNESS);
//    data = updateData(data);
   
//  }
  Serial.print("END COUNTER AT ");
  Serial.print(counter); Serial.print("\n");
  counter++;
  delay(DISPLAY_DURATION); 
//  analogWrite(PIN_OE, 0);
}

byte updateData(byte bytePattern) {
  byte newData = bytePattern;
  byte toAddBit = (newData & 0b10000000) >> 7;
  
  newData = (newData << 1) | toAddBit;
  
  Serial.print("Data "); Serial.print(bytePattern); Serial.print("\n");
  Serial.print("Bit to add "); Serial.print(toAddBit); Serial.print("\n");
  Serial.print(data);
  Serial.print("\n");
  Serial.print("New Data ");
  Serial.print(newData);
  Serial.print("\n");
  return newData;
}
void writeData() {
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

