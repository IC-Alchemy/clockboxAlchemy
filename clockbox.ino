
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
