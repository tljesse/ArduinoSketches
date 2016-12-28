/*
 * Monitors button presses and can tell between a short or
 * long press, will run through a state machine depending 
 * on type of press
 * 
 * Two buttons, both of which are run through interrupts
 * which are only usable on Arduino pins 2 and 3
 */

#define SEL 3
#define OPT 2

volatile bool normalPress = false;
volatile int selectVal = 0;
volatile int optionVal = 0;
int stateMachine = 1;
int optionLast = 0;
int selectLast = 0;
bool ignoreUp = false;
bool optionUp = false;
bool enableRpm = true;
long debounceDelay = 50;

unsigned long optionDnTm = 0;
unsigned long optionUpTm = 0;
unsigned long selectDnTm = 0;
unsigned long selectUpTm = 0;

char handlePress = 'n';

void selectIsr() {
  selectVal = digitalRead(SEL);
}

void optionIsr() {
  optionVal = digitalRead(OPT);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(SEL, INPUT);
  attachInterrupt(1, selectIsr, CHANGE);
  
  pinMode(OPT, INPUT);
  attachInterrupt(0, optionIsr, CHANGE);

  Serial.begin(9600);

}

void loop() {
  
  if (normalPress) {
    handlePress = 'o';
    delay(500);
    handlePress = 'n';
    normalPress = false;
  }
   // check for option pressed
  if (optionVal == HIGH && optionLast == LOW && (millis() - optionUpTm) > debounceDelay){
    optionDnTm = millis();
    handlePress = '1';
  }
  // check if options is longer than 4 second hold -- goes to deep settings
  if (optionVal == HIGH && selectVal == LOW && enableRpm && (millis() - optionDnTm) > 4000) {
    if( !optionUp ){
      optionUp = true;
      normalPress = true;
      stateMachine = 1;
      handlePress = '2';
    }
    selectDnTm = millis();
  }
  
  // check for option release - less than 4 seconds to quick settings
  if (optionVal == LOW && optionLast == HIGH && enableRpm && (millis() - optionDnTm) > 1000){
    if(optionUp){
      optionUp = false;
    } else {
      normalPress = true;
      stateMachine = 3;
    }
    handlePress = '3';
    optionUpTm = millis();
  }

  optionLast = optionVal;

  // check for select pressed
  if (selectVal == HIGH && selectLast == LOW && (millis() - selectUpTm) > debounceDelay){
    handlePress = '4';
    selectDnTm = millis();
  }

  // check for select release
  if (selectVal == LOW && selectLast == HIGH && (millis() - selectDnTm) > debounceDelay){
    if ( ignoreUp ) ignoreUp = false;
    handlePress = '5';
    selectUpTm = millis();
  }

  // check for 1 second select hold
  if (selectVal == HIGH && optionVal == HIGH && (millis() - selectDnTm) > 1000 && (millis() - optionDnTm) > 1000){
    if ( !ignoreUp ) {
      enableRpm = !enableRpm;
      ignoreUp = true;
      optionUp = true;
    }
    handlePress = '6';
    selectDnTm = millis();
  }

  selectLast = selectVal;
  delay(500);
}



