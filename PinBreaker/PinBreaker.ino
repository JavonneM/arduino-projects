#include <Keyboard.h>

#include <Keyboard.h>


int pinNumber = 0;
String pinString;
void setup() {
  // put your setup code here, to run once:
  Keyboard.begin()
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  String padding = "";
  if (pinNumber / 10 == 0) {
    padding += "0";
  } if (pinNumber / 100 == 0) {
    padding += "0";
  } if (pinNumber / 1000 == 0) {
    padding += "0";
  }
  String finalString = padding + pinNumber;
  Serial.println(finalString);
  pinNumber++;
  if (pinNumber > 9999) {
    pinNumber = 0;
    Keyboard.print(finalString);
    Keyboard.print(KEY_RETURN);
  }
}
