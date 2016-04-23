#include <SoftwareSerial.h>
 
//SIM800 TX is connected to Arduino D8
#define SIM800_TX_PIN 4
 
//SIM800 RX is connected to Arduino D7
#define SIM800_RX_PIN 10
 
//Create software serial object to communicate with SIM800
SoftwareSerial serialSIM800(SIM800_TX_PIN,SIM800_RX_PIN);
 
void setup() {
  pinMode(11, OUTPUT);
  
  //Begin serial comunication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  while(!Serial);

  
  //Being serial communication witj Arduino and SIM800
  serialSIM800.begin(9600);
  delay(1000);
  digitalWrite(11, LOW);
  delay(2000);
  digitalWrite(11, HIGH);
  delay(3000);

  if(serialSIM800.available()){
    Serial.print("Reading: \"");
    while(serialSIM800.available())
    {
      char c=serialSIM800.read();
      Serial.write(c);          
    }
    Serial.println("\"");
  }
   
  Serial.println("Setup Complete!");
  Serial.println("Sending SMS...");
   
  //Set SMS format to ASCII
  sendAndEcho("AT+CMGF=1");
  delay(1000);
 
  //Send new SMS command and message number
  sendAndEcho("AT+CMGS=\"xxxxxxxxx\"");
  delay(1000);
   
  //Send SMS content
  serialSIM800.write("TEST");
  delay(1000);
   
  //Send Ctrl+Z / ESC to denote SMS message is complete
  serialSIM800.write((char)26);
  delay(1000);
     
  Serial.println("SMS Sent!");
}

void sendAndEcho(String command) {
  serialSIM800.println(command);
  if(serialSIM800.available()){
    Serial.print("Reading: \"");
    while(serialSIM800.available())
    {
      char c=serialSIM800.read();
      Serial.write(c);          
    }
    Serial.println("\"");
  }
  delay(2000);
}
 
void loop() {
  //Read SIM800 output (if available) and print it in Arduino IDE Serial Monitor
  /*sendAndEcho("AT");

  sendAndEcho("ATI");
  sendAndEcho("AT+GSV");
  sendAndEcho("AT&V");
  sendAndEcho("AT+CMEE=?");
  sendAndEcho("ATE0&W");
  sendAndEcho("AT");
  sendAndEcho("AT");
  sendAndEcho("ATE1&W");
  sendAndEcho("AT");
  sendAndEcho("AT+CFUN=0");
  sendAndEcho("AT+IPR?");
  sendAndEcho("AT+CFUN=0");
  sendAndEcho("AT+CFUN?");

  //sendAndEcho("AT+CPIN?");

  //sendAndEcho("AT+CSMINS?");
  
  /*sendAndEcho("AT+CMGF?");

  sendAndEcho("AT+CPIN?");

  

  sendAndEcho("AT+CFUN=?");

  sendAndEcho("AT+CREG?");

  sendAndEcho("AT+COPS?");

  sendAndEcho("AT+COPS=?");
}
