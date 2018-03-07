#include <PLKit_Connection.hpp>

int buttonPin = 8;
int lastReading = HIGH;

Connection connection;

void setup() {

  pinMode(buttonPin, INPUT_PULLUP);

  connection.open();
}

void loop() {

  int reading = digitalRead(buttonPin);

  if (reading == LOW && lastReading == HIGH) {
    connection.send("bang");
    delay(50);
  }

  lastReading = reading;
}
