int led = 12;
int gateOut1 = 14;
int midiOut = 15;
//int startOut = 10;
int startIn = 9;

int bpm = 160;

int clockTimer = 0;
int midiTimer = 0;

int gateTimer = 0;
int midiGateTimer = 0;

int pulseWidth = 10;
int midiPulseWidth = 10;

int startDebouncer = 0;

boolean gateHigh = false;
boolean midiGateHigh = false;
boolean running = false;

void setup() {
  Serial.begin(38400);
  
  pinMode(led, OUTPUT);
  pinMode(gateOut1, OUTPUT);
  pinMode(midiOut, OUTPUT);
  //pinMode(startOut, OUTPUT);
  pinMode(startIn, INPUT);

  digitalWrite(led, LOW);
  digitalWrite(gateOut1, LOW);
  digitalWrite(midiOut, LOW);
  //digitalWrite(startOut, LOW);
  
  resetAnalogTimers();
  startDebouncer = millis();
}

void resetAnalogTimers() {
  resetAnalogClockTimer();
  resetGateTimer();
  resetMidiTimer();
  resetMidiGateTimer();
}

void resetAnalogClockTimer() {
  clockTimer = millis();
}

void resetGateTimer() {
  gateTimer = millis();
}

void resetMidiTimer() {
  midiTimer = millis();
  resetMidiGateTimer();
}

void resetMidiGateTimer() {
  midiGateTimer = millis();
}

void updateBpm(int newBpm) {
  bpm = newBpm;
}

int bpmToMillis(int bpm) {
  return round(60000/bpm);
}

int bpmToMillis24Ppqn(int bpm) {
  return round(bpmToMillis(bpm) / 24);
}

void setAnalogGateHigh() {
  Serial.println(bpm);
  digitalWrite(led, HIGH);
  digitalWrite(gateOut1, HIGH);
  //setMidiGateHigh();
  resetAnalogTimers();
  gateHigh = true;
}

void setMidiGateHigh() {
  //Serial.println("MIDI");
  resetMidiTimer();
  digitalWrite(midiOut, HIGH);
  midiGateHigh = true;
}

void setMidiGateLow() {
  resetMidiGateTimer();
  digitalWrite(midiOut, LOW);
  midiGateHigh = false;
}

void loop() {
  int now = millis();
  int bpmRead = round(analogRead(0));
  bpmRead = bpmRead >= 10 ? bpmRead : 10;
  int bpmMillis24Ppqn = bpmToMillis24Ppqn(bpmRead);

  if(digitalRead(startIn) == HIGH) {
    if((now - startDebouncer) >= 1000) {
      if(!running) {
        Serial.println("START");
        delay(100);
        resetAnalogTimers();
        setMidiGateHigh();
      } else {
        Serial.println("STOP");
        resetAnalogTimers();
      }
      running = !running;
      startDebouncer = millis();
    }
  }

  if(running) {
    int midiInterval = now - midiTimer;
    int analogInterval = now - clockTimer;
    
    // Analog clock
    if(analogInterval >= (bpm * 24)) {
      updateBpm(bpmMillis24Ppqn);
      setAnalogGateHigh();
      setMidiGateHigh();
    }
  
    if(gateHigh && ((now - midiPulseWidth  - gateTimer) >= pulseWidth)) {
      digitalWrite(led, LOW);
      digitalWrite(gateOut1, LOW);
      //digitalWrite(startOut, LOW);
      resetGateTimer();
      gateHigh = false;
    }
  
    // Midi clock
    if(midiInterval >= (bpm * 4)) {
      setMidiGateHigh();
    }
  
    if(midiGateHigh && ((now - midiGateTimer) >= midiPulseWidth)) {
      setMidiGateLow();
    }
  }
}
