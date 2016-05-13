#include <SoftwareSerial.h>

#define RST 5
#define CH_PD 7

SoftwareSerial espSerial (12, 11);

void setup() {
  // put your setup code here, to run once:
  espSerial.begin(9600);
  Serial.begin(9600);
  espSerial.println("AT+CWLAP");
  if(espSerial.available()){
    Serial.println("Reading: ");
    while(espSerial.available()){
      char c = espSerial.read();
      Serial.write(c);
    }
    Serial.println("::>-");
  }
  delay(5000);
  espSerial.println("AT+GSLP");
  espSerial.end();
  //Serial.begin(115200);
  //delay(1000);
  //Serial.println("AT+UART_DEF=9600,8,1,0,0");

}

void loop() {
  // put your main code here, to run repeatedly:
  
  
}
