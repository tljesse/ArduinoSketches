#include<SoftwareSerial.h>
#include <stdlib.h>

#define _SS_MAX_RX_BUFF 256

#define TX_PIN 5
#define RX_PIN 10
#define CH_PD 13

#define DEBUG true
                            
char responseBuffer[600];

SoftwareSerial esp8266(TX_PIN, RX_PIN);

void setup() {
  pinMode(CH_PD, OUTPUT);
  digitalWrite(CH_PD, HIGH);
  Serial.setTimeout(5000);
  Serial.begin(9600);
  esp8266.begin(9600);
  delay(1000);
}

void loop() {

  sendData("AT+CWLAP\r\n",2000,DEBUG);
  
  if(esp8266.overflow()){
    Serial.println("Soft Overflow");
  }

}

void sendData(String command, const int timeout, boolean debug){
  // Init array variables
  int ndx = 0;
  char rc = (char)0;
  
  // Clear array
  memset(responseBuffer, 0, sizeof(responseBuffer));
  
  Serial.println("Command : " + command);
  
  // Send command to module
  esp8266.print(command);
  
  // Record current time
  long int time = millis();
  
  // Timeout
  while(millis() - time < timeout){
    while(esp8266.available() > 0){
      rc = esp8266.read();
      
      responseBuffer[ndx] = rc;
      ndx++;
    }    
  }
   
  if(debug){
    char* tokens;
    
    tokens = strtok(responseBuffer, "\"");
    String espData[21];
    
    int counter = 0;
    int index = 0;
    while(tokens != NULL){
      counter++;
      if(strstr(tokens, "+CWLAP") == NULL && index < 21){
        espData[index] = tokens;
        index++;
      }
      counter % 2 == 0 ? tokens = strtok(NULL, "\"") : tokens = strtok(NULL, ",");
    }
    for(int x = 0; x < index-1; x++){
      size_t nullTerm = espData[x].length();
      espData[x][nullTerm - 1] = '\0';
      String output;
      output.reserve(espData[x].length());
      for(size_t i = 0; i < espData[x].length(); ++i){
        if(espData[x][i] != ':') output += espData[x][i];
      }
      espData[x] = output;
      Serial.println(espData[x]);
    }
   
  }
   
}
