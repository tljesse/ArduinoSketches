#include <SoftwareSerial.h>

#define DATA_PIN 5

SoftwareSerial espSerial(5, 6);


void setup() { 
  Serial.begin(9600);
  espSerial.begin(9600);
  delay(1000);

  //pinMode(DATA_PIN, OUTPUT);

      
}

void loop() { 
  // Turn the LED on, then pause
  /*digitalWrite(DATA_PIN, HIGH);
  delay(500);
  // Now turn the LED off, then pause
  digitalWrite(DATA_PIN, LOW);
  delay(500);*/
  while(espSerial.available()) {
    char c = espSerial.read();
    Serial.write(c);
  }
}
