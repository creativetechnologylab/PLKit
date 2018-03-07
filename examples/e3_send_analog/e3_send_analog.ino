#include <PLKit_Connection.hpp>

int potPin = A0;

Connection connection;
int lastReading = 0;

void setup() {
  pinMode(potPin, INPUT);
  
  connection.open();
}

void loop() {
  int reading = analogRead(potPin);
  
  if (reading != lastReading) {
    connection.send("pot", reading);
    
    lastReading = reading;
  }
}

