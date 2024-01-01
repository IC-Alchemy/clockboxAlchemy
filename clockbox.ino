
#include <uClock.h>
byte ANALOG_SYNC_RATIO = 16;   //  needs to be larger here so that it can be divided down for the outputs

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
// Function to send digital output to a specific pin
void sendDigitalOut(bool state, byte pin) {
  byte pinState = state ? HIGH : LOW;
  digitalWrite(digitalPinOut[pin], pinState); // Set the state of the specific digital output pin
}

void clockOutput32PPQN(uint32_t* tick) {
  if (currentState) {
    for (byte i = 0; i < pinCount; i++) {
      if ((*tick % (ANALOG_SYNC_RATIO << i)) == 0) {   //  this functions so that each output is at 1/2 the frequency of the previous
        sendDigitalOut(true, i); // Send high signal to specific digital output
      } else {
        sendDigitalOut(false, i); // Send low signal to specific digital output
      }
    }
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

void loop() {
  float tempo = readTempoFromPotentiometer(A0);
  setClockTempo(tempo);
  handleSwitchState();
  
}
