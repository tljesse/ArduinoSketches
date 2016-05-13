#include<SoftwareSerial.h>
#include <stdlib.h>

#define WAP_LENGTH 10

SoftwareSerial esp8266(5, 6);

//String espData[WAP_LENGTH][3];
String ssidData[WAP_LENGTH];
String rssiData[WAP_LENGTH];
String macData[WAP_LENGTH];

void setup() {
  Serial.setTimeout(5000);
  Serial.begin(115200);
  esp8266.begin(9600);
  delay(1000);
}

void loop() {
  delay(2000);
  String command="AT+CWLAP";
  char espIncoming;
  String ssid = "";
  String rssi = "";
  String mac = "";
  float tester = -32.17459;
  char outstr[15];
  String dataString = "";
  boolean firstRead = false;
  boolean lastRead = false;
  boolean rsiFirstRead = false;
  boolean rsiLastRead = false;
  boolean macFirstRead = false;
  boolean macLastRead = false;
  boolean contains = false;
  int dot = 0;

  for(int x = 0; x < WAP_LENGTH; x++) {
    if (ssidData[x] == "") {
      dot = x;
      break;
    }
  }

  esp8266.println(command);
  if(esp8266.available())
  {
    Serial.print("Reading: ");
    while(esp8266.available())
    {
      char c = esp8266.read();
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
      /*if ( espIncoming == "+CWLAP:(" && espIncoming != ")" ){
        Serial.print(espIncoming[dot++]);
      }
      //Serial.print(c);    */      
      Serial.print(espIncoming);
    }

    for (int x = 0; x < WAP_LENGTH; x++) {
      if(ssidData[x] != "") {
        //sprintf(dataString,"%s;%s,%s,%s;",dataString,ssidData[x],rssiData[x],macData[x]);
        dataString += ssidData[x] + "," + rssiData[x] + "," + macData[x] + ";";
      }
    }
    Serial.println();
    Serial.println(dataString);
    Serial.println(dtostrf(tester,2,3,outstr));
    //dot++;
    //Serial.print(espData);
    Serial.println(ssid);
    Serial.println(rssi);
    Serial.println(mac);
    Serial.println(" - ");
  }
}
