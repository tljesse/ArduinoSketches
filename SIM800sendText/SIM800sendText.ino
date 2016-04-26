#include <SoftwareSerial.h>
#include <TinyGPS.h>
 
#define SIM800_TX_PIN 4
#define SIM800_RX_PIN 10

#define SIM_PWR 11
#define BUTTON_PIN 2

volatile boolean simPower = false;
volatile boolean interruptTrigger = false;
 
//Create software serial object to communicate with SIM800
SoftwareSerial simSerial(SIM800_TX_PIN,SIM800_RX_PIN);
 
void setup() {
  pinMode(SIM_PWR, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  attachInterrupt(0, btn_ISR, RISING);
  
  //Begin serial comunication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  while(!Serial);

  //simSerial.begin(9600);
  
 
  Serial.println("Waiting for Button");
}

void sendAndEcho(String command) {
  simSerial.println(command);
  if(simSerial.available()){
    Serial.print("Reading: \"");
    while(simSerial.available())
    {
      char c=simSerial.read();
      Serial.write(c);          
    }
    Serial.println("\"");
  }
  delay(2000);
}
 
void loop() {
  //Read SIM800 output (if available) and print it in Arduino IDE Serial Monitor
  
  if (interruptTrigger) {
    Serial.println("Button Press");
    //while (gpsSerial.available()) { gpsSerial.read(); }
    //gpsSerial.end();
    simSerial.begin(9600);
    delay(1000);

    simSerial.listen();

    if (!simPower) {
      digitalWrite(SIM_PWR, LOW);
      delay(2000);
      digitalWrite(SIM_PWR, HIGH);
      delay(3000);
    
      if(simSerial.available()){
        Serial.print("Reading: \"");
        while(simSerial.available())
        {
          char c=simSerial.read();
          Serial.write(c);          
        }
        Serial.println("\"");
      }
      simPower = true;
    }
    //dataString += locData;
    Serial.println("Sending SMS...");
   
    //Set SMS format to ASCII
    sendAndEcho("AT+CMGF=1");
    delay(1000);
   
    //Send new SMS command and message number
    sendAndEcho("AT+CMGS=\"xxxxxxxx\"");
    delay(1000);
     
    //Send SMS content
    simSerial.print("Test Interrupt");
    delay(1000);
     
    //Send Ctrl+Z / ESC to denote SMS message is complete
    simSerial.write((char)26);
    delay(1000);
       
    Serial.println("SMS Sent!");
    interruptTrigger = false;
    simSerial.end();
  }
}

void btn_ISR() {
  interruptTrigger = true;
}
