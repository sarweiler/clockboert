int led = 12;
int gateOut1 = 14;
int urzwergOut = 15;
int startIn = 9;

int bpm = 160;

int clockTimer = 0;
int urzwergTimer = 0;

int gateTimer = 0;
int urzwergGateTimer = 0;

int pulseWidth = 10;
int urzwergPulseWidth = 10;

int startDebouncer = 0;

boolean gateHigh = false;
boolean urzwergGateHigh = false;
boolean running = false;

void setup() {
  Serial.begin(38400);
  
  pinMode(led, OUTPUT);
  pinMode(gateOut1, OUTPUT);
  pinMode(urzwergOut, OUTPUT);
  pinMode(startIn, INPUT);

  digitalWrite(led, LOW);
  digitalWrite(gateOut1, LOW);
  digitalWrite(urzwergOut, LOW);
  
  resetAnalogTimers();
  startDebouncer = millis();
}

void resetAnalogTimers() {
  resetAnalogClockTimer();
  resetGateTimer();
  resetUrzwergTimer();
  resetUrzwergGateTimer();
}

void resetAnalogClockTimer() {
  clockTimer = millis();
}

void resetGateTimer() {
  gateTimer = millis();
}

void resetUrzwergTimer() {
  urzwergTimer = millis();
  resetUrzwergGateTimer();
}

void resetUrzwergGateTimer() {
  urzwergGateTimer = millis();
}

int bpmToMillis(int bpm) {
  return round(60000/bpm);
}

int millisToBpm(int milliseconds) {
  return round(60000/milliseconds);
}

int bpmToMillis24Ppqn(int bpm) {
  return round(bpmToMillis(bpm) / 24);
}

void setAnalogGateHigh() {
  digitalWrite(led, HIGH);
  digitalWrite(gateOut1, HIGH);
  resetAnalogTimers();
  gateHigh = true;
}

void setUrzwergGateHigh() {
  resetUrzwergTimer();
  digitalWrite(urzwergOut, HIGH);
  urzwergGateHigh = true;
}

void setUrzwergGateLow() {
  resetUrzwergGateTimer();
  digitalWrite(urzwergOut, LOW);
  urzwergGateHigh = false;
}

void loop() {
  int now = millis();
  int bpmRead = round(analogRead(0) / 5);
  bpmRead = bpmRead >= 10 ? bpmRead : 10;
  int intervalMillis = bpmToMillis(bpmRead) / 4;

  if(digitalRead(startIn) == HIGH) {
    if((now - startDebouncer) >= 1000) {
      if(!running) {
        Serial.println("START");
        // Weird Urzwerg needs a delay and
        // an extra impulse for syncing correctly
        delay(100);
        setUrzwergGateHigh(); 
        resetAnalogTimers();
      } else {
        Serial.println("STOP");
        resetAnalogTimers();
      }
      running = !running;
      startDebouncer = millis();
    }
  }

  if(running) {
    int urzwergInterval = now - urzwergTimer;
    int analogInterval = now - clockTimer;
    
    // Analog clock
    if(analogInterval >= (intervalMillis)) {
      Serial.print("bpm: ");
      Serial.print(bpmRead);
      Serial.print(" / ms: ");
      Serial.print(intervalMillis);
      Serial.print(" / r: ");
      Serial.println(bpmRead);
      
      setAnalogGateHigh();
      setUrzwergGateHigh();
    }
  
    if(gateHigh && ((now - urzwergPulseWidth  - gateTimer) >= pulseWidth)) {
      digitalWrite(led, LOW);
      digitalWrite(gateOut1, LOW);
      resetGateTimer();
      gateHigh = false;
    }
  
    // Urzwerg clock
    if(urzwergInterval >= (intervalMillis / 6)) {
      setUrzwergGateHigh();
    }
  
    if(urzwergGateHigh && ((now - urzwergGateTimer) >= urzwergPulseWidth)) {
      setUrzwergGateLow();
    }
  }
}
