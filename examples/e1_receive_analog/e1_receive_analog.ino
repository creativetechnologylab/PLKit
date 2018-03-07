#include <PLKit_Connection.hpp>

int ledPin = 6;

Connection connection;

void setup() {
  pinMode(ledPin, OUTPUT);
  
  connection.open();
  connection.receive("brightness", led);
}

void loop() {
  connection.update();
}

void led(int value) {
  analogWrite(ledPin, value);
}

