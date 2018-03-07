#include <PLKit_Connection.hpp>

int ledPin = 6;

Connection connection;

void setup() {

  pinMode(ledPin, OUTPUT);
  
  connection.open();
  connection.receive("on", ledOn);
  connection.receive("off", ledOff);
}

void loop() {
  connection.update();
}

void ledOn() {
  digitalWrite(ledPin, HIGH);
}

void ledOff() {
  digitalWrite(ledPin, LOW);
}

