
#include <uClock.h>
#define ANALOG_SYNC_RATIO 4

bool currentState = false;
bool currentSwitchState = false;
bool needsToSendMidiStart = false;

const byte pinCount = 4;
byte digitalPinOut[pinCount] = {3,5,7,9}; //Define analog clock outputs here

void clockOutput96PPQN(uint32_t* tick) {
  if (needsToSendMidiStart) {
    needsToSendMidiStart = false;
    Serial.write(0xFA);
  }
  Serial.write(0xF8);
}

void clockOutput32PPQN(uint32_t* tick) {
  if (currentState) {
    if ((*tick % ANALOG_SYNC_RATIO ) == 0) {
      sendDigitalOut(true);
    } else {
      sendDigitalOut(false);
    }
  }
}

void sendDigitalOut(bool state) {
  byte pinState = state ? HIGH : LOW;
  
  for (byte i = 0; i < pinCount; i++) {
    digitalWrite(digitalPinOut[i], pinState);
  }
}


void toggleStartStop() {
  if (currentState) {
    Serial.write(0xFC);
    sendDigitalOut(false);
    currentState = false;
    
  } else {
    uClock.stop();
    delay(20);
    currentState = true;
    needsToSendMidiStart = true;
    uClock.start();  
  }
}

float readTempoFromPotentiometer(int pin) {
  int tempoPot = analogRead(pin);
  float tempo = ((float)tempoPot/1024.f)*210.f + 30.f;
  return tempo;
}

void setClockTempo(float tempo) {
  uClock.setTempo(tempo);
}

void handleSwitchState() {
  int switchState = readSwitchState(2);
  if (isSwitchPressed(switchState) && !currentSwitchState) {
    toggleStartStop();
    currentSwitchState = true;
  }
  
  if (isSwitchReleased(switchState)) {
    currentSwitchState = false;
  }
}

int readSwitchState(int pin) {
  return digitalRead(pin);
}

bool isSwitchPressed(int switchState) {
  return switchState == HIGH;
}

bool isSwitchReleased(int switchState) {
  return switchState == LOW;
}


void setup() {

  Serial.begin(31250);
  uClock.init();

  pinMode(2, INPUT);

  for (byte i = 0; i < pinCount; i++) {
    pinMode(digitalPinOut[i], OUTPUT);
  }
  

  uClock.init();
  
  uClock.setClock96PPQNOutput(clockOutput96PPQN);
  uClock.setClock32PPQNOutput(clockOutput32PPQN);
  uClock.setTempo(96);

  uClock.start();
}



void loop() {                        /////  loop
  float tempo = readTempoFromPotentiometer(A0);
  setClockTempo(tempo);
  handleSwitchState();
  delay(30);
}
