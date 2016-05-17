/****************************
 * 
 * Sketch: Campus Track Prototype
 * Author: Tristan Jesse
 * Rev: 1.0
 * Rev Date: 4/21/2016  Date: 4/20/2016
 * 
 * This sketch is for the PCB prototype device to send GPS, BSSID
 * and cell over GRPS to get an exact position on a webpage
 * at the press of a button.
 * 
 *****************************/

#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <avr/sleep.h>
 
#define SIM800_TX_PIN 4
#define SIM800_RX_PIN 10

#define SIM_PWR 11
#define BUTTON_PIN 2
#define WAP_LENGTH 10

volatile boolean simPower = false;
volatile boolean interruptTrigger = false;
volatile bool justAwoke = false;
unsigned long startTime = millis();
//String espData[WAP_LENGTH][3];
String ssidData[WAP_LENGTH];
String rssiData[WAP_LENGTH];
String macData[WAP_LENGTH];
bool textOnce = true;
void btn_ISR();
 
//Create software serial object to communicate with SIM800
SoftwareSerial simSerial(SIM800_TX_PIN,SIM800_RX_PIN);
SoftwareSerial gpsSerial(14, 15);
SoftwareSerial espSerial(5, 6);

TinyGPS gps;

void wake() {
  sleep_disable();
  detachInterrupt(0);
  justAwoke = true;
  textOnce = true;
}
 
void setup() {
  pinMode(SIM_PWR, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
  //attachInterrupt(0, btn_ISR, RISING);
  
  //Begin serial comunication with Arduino and Arduino IDE (Serial Monitor)
  //Serial.begin(9600);
  //while(!Serial);

  //simSerial.begin(9600);
  
 
  //Serial.println("Waiting for Button");
}

void sendAndEcho(String command) {
  simSerial.println(command);
  if(simSerial.available()){
    //Serial.print("Reading: \"");
    while(simSerial.available())
    {
      char c=simSerial.read();
      //Serial.write(c);          
    }
    //Serial.println("\"");
  }
  delay(2000);
}
 
void loop() {
  if (justAwoke) {
    bool newData = false;
    bool firstRead = false;
    bool lastRead = false;
    bool rsiFirstRead = false;
    bool rsiLastRead = false;
    bool macFirstRead = false;
    bool macLastRead = false;
    bool contains = false;
    int dot = 0;
    char espIncoming;
    String ssid = "";
    String rssi = "";
    String mac = "";
    String locData = "yay test yay";
    String dataString = "";
  
    if (millis() - startTime < 60000 && !interruptTrigger) {
      espSerial.begin(9600);
      delay(1000);
      espSerial.println("AT+CWLAP");
      if(espSerial.available())
      {
        //Serial.print("Reading: ");
        while(espSerial.available())
        {
          char c = espSerial.read();
          //Serial.write(c);
          if(espIncoming == '(' && c != ')') {
            if (firstRead && !lastRead) {
              if (c == '\"') {
                lastRead = true;
                for (int x = 0; x < WAP_LENGTH; x++) {
                  if (ssidData[x] == ssid) {
                    contains = true;
                    dot = x;
                    break;
                  }
                }
                //if (!contains) {
                  //espData[dot][0] = ssid;  
                //}
              } else {
                ssid += c;
              }
            }
            if (c == '\"') firstRead = true;
            if(lastRead) {
              if (rsiFirstRead && !rsiLastRead){
                if (c == ','){
                  rsiLastRead = true;
                  //espData[dot][1] = rssi;
                } else {
                  rssi += c;
                }
              }
              if (c == ',') rsiFirstRead = true;
            }
            if(rsiLastRead) {
              if(macFirstRead && !macLastRead){
                if (c == '\"'){
                  macLastRead = true;
                  ssidData[dot] = ssid;
                  rssiData[dot] = rssi;
                  macData[dot] = mac;
                } else {
                  mac += c;
                }
              }
              if (c == '\"') macFirstRead = true;
            }
          } else {
            espIncoming = c;
            firstRead = false;
            lastRead = false;
            rsiFirstRead = false;
            rsiLastRead = false;
            macFirstRead = false;
            macLastRead = false;
          }      
        }
      } // End of WAP search=
      //Serial.println(espIncoming);
      espSerial.end();
    } else {
      unsigned long chars;
      unsigned short sentences, failed;
      gpsSerial.begin(9600);
      delay(1000);
      //Serial.println(dataString);
      for (unsigned long start = millis(); millis() - start < 1000;) {
        while (gpsSerial.available()) {
          char c = gpsSerial.read();
          if (gps.encode(c))
            newData = true;
        }
      } // end GPS pull
    
      // GPS Data is ready
      if (newData) {
        float flat, flon;
        char latOut[10];
        char lonOut[10];
        unsigned long age;
        gps.f_get_position(&flat, &flon, &age);
        flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6;
        flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6;
  
        dtostrf(flat, 3, 5, latOut);
        dtostrf(flon, 3, 5, lonOut);
        //sprintf(locData,"%s,%s;",latOut,lonOut);
        locData = String(latOut) + "," + String(lonOut);
        interruptTrigger = true;
      }

      //gps.stats(&chars, &sentences, &failed);
      //Serial.print(" CHARS=");
      //Serial.print(chars);
      //Serial.print(" SENTENCES=");
      //Serial.print(sentences);
      //Serial.print(" CSUM ERR=");
      //Serial.println(failed);
      //if (chars == 0)
        //Serial.println("** No characters received from GPS: check wiring **");
      
      gpsSerial.end();
    }
  
    for (int x = 0; x < WAP_LENGTH; x++) {
      if(ssidData[x] != "") {
        //sprintf(dataString,"%s;%s,%s,%s;",dataString,ssidData[x],rssiData[x],macData[x]);
        dataString += ssidData[x] + "," + rssiData[x] + "," + macData[x] + ";";
      }
    }
  
    //Serial.println(dataString+locData);
    
    if (interruptTrigger && textOnce) {
      textOnce = false;
      //Serial.println("Button Press");
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
          //Serial.print("Reading: \"");
          while(simSerial.available())
          {
            char c=simSerial.read();
            //Serial.write(c);          
          }
          //Serial.println("\"");
        }
        simPower = true;
      }
      //dataString += locData;
      //Serial.println("Sending SMS...");
     
      //Set SMS format to ASCII
      sendAndEcho("AT+CMGF=1");
      delay(1000);
     
      //Send new SMS command and message number
      sendAndEcho("AT+CMGS=\"+12064664442\"");
      delay(1000);
       
      //Send SMS content
      simSerial.print(dataString+locData);
      delay(1000);
       
      //Send Ctrl+Z / ESC to denote SMS message is complete
      simSerial.write((char)26);
      delay(1000);
         
      //Serial.println("SMS Sent!");
      interruptTrigger = false;
      simSerial.end();
      justAwoke = false;
    } // end SMS
  } else {
    // Put all modules to low power mode then sleep MC
    // SIM module power down - change this to check status pin when attached
    if(simPower) {
      digitalWrite(SIM_PWR, LOW);
      delay(2000);
      digitalWrite(SIM_PWR, HIGH);
      delay(3000);
      simPower = false;
    }

    espSerial.begin(9600);
    delay(1000);
    espSerial.println("AT+SLEEP=1");
    espSerial.end();

    // Need to wire to on/off pins of other modules

    ADCSRA = 0;

    set_sleep_mode (SLEEP_MODE_PWR_DOWN);
    sleep_enable();

    //don't interrupt before sleep or we won't wake
    noInterrupts();
    attachInterrupt(0, wake, FALLING);
    EIFR = bit(INTF0);

    //turn off brown-out eanbale
    MCUCR = bit (BODS) | bit (BODSE);
    MCUCR = bit (BODS);

    interrupts();
    sleep_cpu();
  }
}

void btn_ISR() {
  interruptTrigger = true;
}
