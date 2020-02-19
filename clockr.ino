int led = 12;
int gateOut1 = 14;
int urzwergOut = 15;
int glitchOut = 16;
int glitchIn = 8;
int startIn = 9;
int stopIn = 10;

int bpm = 160;

int clockTimer = 0;
int urzwergTimer = 0;
int glitchTimer = 0;

int gateTimer = 0;
int urzwergGateTimer = 0;

int pulseWidth = 1;
int urzwergPulseWidth = 1;

int buttonDebouncer = 0;

int glitchIntervalDivider = 1;
float glitchIntervalDividers[] = {0.25, 0.5, 4.0, 8.0, 16.0};

boolean gateHigh = false;
boolean glitchGateHigh = false;
boolean urzwergGateHigh = false;

boolean glitchActive = false;
boolean running = false;

void setup() {
  Serial.begin(38400);
  
  pinMode(led, OUTPUT);
  pinMode(gateOut1, OUTPUT);
  pinMode(urzwergOut, OUTPUT);
  pinMode(glitchOut, OUTPUT);
  pinMode(startIn, INPUT);
  pinMode(stopIn, INPUT);
  pinMode(glitchIn, INPUT);

  digitalWrite(led, LOW);
  digitalWrite(gateOut1, LOW);
  digitalWrite(urzwergOut, LOW);
  digitalWrite(glitchOut, LOW);
  
  resetAnalogTimers();
  buttonDebouncer = millis();
}

void resetAnalogTimers() {
  resetAnalogClockTimer();
  resetGlitchTimer();
  resetGateTimer();
  resetUrzwergTimer();
  resetUrzwergGateTimer();
}

void resetAnalogClockTimer() {
  clockTimer = millis();
}

void resetGlitchTimer() {
  glitchTimer = millis();
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

void setGlitchGateHigh() {
  digitalWrite(glitchOut, HIGH);
  resetGlitchTimer();
  glitchGateHigh = true;
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
  int bpmRead = round(analogRead(0) / 5);
  bpmRead = bpmRead >= 10 ? bpmRead : 10;
  int intervalMillis = bpmToMillis(bpmRead) / 2;
  
  int now = millis();
  int urzwergInterval = now - urzwergTimer;
  int analogInterval = now - clockTimer;
  int glitchInterval = now - glitchTimer;

  // Start/Stop button
  if(digitalRead(startIn) == HIGH) {
    //Serial.print("BUTTON: ");
    //Serial.println(now - buttonDebouncer);
    if((now - buttonDebouncer) >= 1000) {
      if(!running) {
        Serial.println("START");
        // Weird Urzwerg needs a delay and
        // an extra impulse for syncing correctly
        delay(100);
        setUrzwergGateHigh(); 
        resetAnalogTimers();
        running = true;
      } else {
        Serial.println("STOP");
        resetAnalogTimers();
        running = false;
      }
      buttonDebouncer = millis();
    }
  }

  // Stop button
  if(digitalRead(stopIn) == HIGH) {
    if((now - buttonDebouncer) >= 1000) {
      Serial.println("STOP");
      resetAnalogTimers();
      running = false;
      buttonDebouncer = millis();
    }
  }

  // Glitch button
  if(digitalRead(glitchIn) == HIGH) {
    if((now - buttonDebouncer) >= 1000) {
      Serial.println("GLITCH");
      glitchActive = true;
      buttonDebouncer = millis();
    }
  } else {
    glitchActive = false;
    glitchIntervalDivider = 1;
  }


  // Clock output
  if(running) {
    // Analog clock
    if(analogInterval >= (intervalMillis)) {
      Serial.print("bpm: ");
      Serial.print(bpmRead);
      Serial.print(" / ms: ");
      Serial.print(intervalMillis);
      Serial.print(" / r: ");
      Serial.println(bpmRead);

      if(glitchActive) {
        glitchIntervalDivider = round(glitchIntervalDividers[random(5)]);
      }
      setAnalogGateHigh();
      setGlitchGateHigh();
      setUrzwergGateHigh();
    }
  
    if(gateHigh && ((now - urzwergPulseWidth  - gateTimer) >= pulseWidth)) {
      digitalWrite(led, LOW);
      digitalWrite(gateOut1, LOW);
      resetGateTimer();
      gateHigh = false;
    }

    // Glitch clock
    if(glitchInterval >= (intervalMillis / glitchIntervalDivider)) {
      setGlitchGateHigh();
    }

    if(glitchGateHigh && ((now - glitchTimer) >= pulseWidth)) {
      digitalWrite(glitchOut, LOW);
      resetGlitchTimer();
      glitchGateHigh = false;
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
