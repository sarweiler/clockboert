int gateLed = 0;
int urzwergClockLed = 1;
int urzwergStartLed = 2;
int glitchLed = 3;

int gateOut1 = 14;
int gateOutPO = 12;
int gateOut2x = 11;
int gateOutLsdj = 13;
int urzwergOut = 15;
int glitchOut = 16;

int glitchIn = 8;
int startIn = 9;
int stopIn = 10;
int bpmIn = A0;

int bpm = 160;

long clockTimer = 0;
long urzwergTimer = 0;
long glitchTimer = 0;

long gateTimer = 0;
long urzwergGateTimer = 0;

int pulseWidth = 1;
int urzwergPulseWidth = 1;
int glitchLedLength = 25;
int urzwergLedLength = 10;
int gateLedLength = 25;

long buttonDebouncer = 0;

int glitchIntervalDivider = 1;
float glitchIntervalDividers[] = {0.25, 0.5, 4.0, 8.0, 16.0};

boolean gateHigh = false;
boolean gate2xHigh = false;
boolean glitchGateHigh = false;
boolean urzwergGateHigh = false;
boolean poGateHigh = false;

boolean glitchLedOn = false;
boolean urzwergLedOn = false;
boolean gateLedOn = false;

boolean glitchActive = false;
boolean running = false;

void setup() {
  Serial.begin(38400);
  
  pinMode(gateLed, OUTPUT);
  pinMode(urzwergClockLed, OUTPUT);
  pinMode(urzwergStartLed, OUTPUT);
  pinMode(glitchLed, OUTPUT);
  
  pinMode(gateOut1, OUTPUT);
  pinMode(urzwergOut, OUTPUT);
  pinMode(glitchOut, OUTPUT);
  pinMode(gateOutPO, OUTPUT);
  pinMode(gateOut2x, OUTPUT);
  pinMode(gateOutLsdj, OUTPUT);
  
  pinMode(startIn, INPUT);
  pinMode(stopIn, INPUT);
  pinMode(glitchIn, INPUT);

  digitalWrite(gateLed, LOW);
  digitalWrite(urzwergClockLed, LOW);
  digitalWrite(urzwergStartLed, LOW);
  digitalWrite(glitchLed, LOW);
  digitalWrite(gateOut1, LOW);
  digitalWrite(urzwergOut, LOW);
  digitalWrite(glitchOut, LOW);
  digitalWrite(gateOutPO, LOW);
  digitalWrite(gateOut2x, LOW);
  digitalWrite(gateOutLsdj, LOW);
  
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

void allLedsOff() {
  digitalWrite(gateLed, LOW);
  digitalWrite(glitchLed, LOW);
  digitalWrite(urzwergStartLed, LOW);
  digitalWrite(urzwergClockLed, LOW);
}


// ANALOG GATE
void setAnalogGateHigh() {
  digitalWrite(gateLed, HIGH);
  digitalWrite(gateOut1, HIGH);
  setLsdjGateHigh();
  digitalWrite(glitchLed, HIGH);
  resetAnalogTimers();
  gateHigh = true;
  gateLedOn = true;
}

void setAnalogGateLow() {
  digitalWrite(gateOut1, LOW);
  setLsdjGateLow();
  resetGateTimer();
  gateHigh = false;
}

void setLsdjGateHigh() {
  Serial.println("LSDJ HIGH");
  digitalWrite(gateOutLsdj, HIGH);
}

void setLsdjGateLow() {
  Serial.println("LSDJ LOW");
  digitalWrite(gateOutLsdj, LOW);
}

// 2X GATE
void set2xGateHigh() {
  digitalWrite(gateOut2x, HIGH);
  gate2xHigh = true;
}

void set2xGateLow() {
  digitalWrite(gateOut2x, LOW);
  gate2xHigh = false;
}

// GLITCH GATE
void setGlitchGateHigh() {
  digitalWrite(glitchOut, HIGH);
  digitalWrite(glitchLed, HIGH);
  resetGlitchTimer();
  glitchGateHigh = true;
  glitchLedOn = true;
}

void setGlitchGateLow() {
  digitalWrite(glitchOut, LOW);
  resetGlitchTimer();
  glitchGateHigh = false;
}


// URZWERG GATE
void setUrzwergGateHigh() {
  resetUrzwergTimer();
  digitalWrite(urzwergOut, HIGH);
  digitalWrite(urzwergClockLed, HIGH);
  urzwergGateHigh = true;
  urzwergLedOn = true;
}

void setUrzwergGateLow() {
  resetUrzwergGateTimer();
  digitalWrite(urzwergOut, LOW);
  digitalWrite(urzwergStartLed, LOW);
  urzwergGateHigh = false;
}


// PO
void swapPoGate() {
  if(poGateHigh) {
    Serial.println("PO high");
    digitalWrite(gateOutPO, HIGH);
  } else {
    Serial.println("PO low");
    digitalWrite(gateOutPO, LOW);
  }
  poGateHigh = !poGateHigh;
}


// LOOP
void loop() {
  int bpmRead = round(analogRead(bpmIn) / 5);
  bpmRead = bpmRead >= 10 ? bpmRead : 10;
  int intervalMillis = bpmToMillis(bpmRead) / 2;
  
  long now = millis();
  long urzwergInterval = now - urzwergTimer;
  long analogInterval = now - clockTimer;
  long glitchInterval = now - glitchTimer;


  // START/STOP BUTTON
  if(digitalRead(startIn) == HIGH) {
    if((now - buttonDebouncer) >= 1000) {
      if(!running) {
        Serial.println("START");
        // Weird Urzwerg needs a delay and
        // an extra impulse for syncing correctly
        digitalWrite(urzwergStartLed, HIGH);
        delay(100);
        setUrzwergGateHigh(); 
        resetAnalogTimers();
        running = true;
      } else {
        digitalWrite(urzwergStartLed, HIGH);
        delay(100);
        digitalWrite(urzwergStartLed, LOW);
        Serial.println("STOP");
        resetAnalogTimers();
        running = false;
        allLedsOff();
      }
      buttonDebouncer = millis();
    }
  }


  // STOP BUTTON
  if(digitalRead(stopIn) == HIGH) {
    if((now - buttonDebouncer) >= 100) {
      Serial.println("STOP");
      resetAnalogTimers();
      running = false;
      buttonDebouncer = millis();
      allLedsOff();
    }
  }


  // GLITCH BUTTON
  if(digitalRead(glitchIn) == HIGH) {
    if((now - buttonDebouncer) >= 100) {
      Serial.println("GLITCH");
      glitchActive = true;
      buttonDebouncer = millis();
    }
  } else {
    glitchActive = false;
    glitchIntervalDivider = 1;
  }


  // CLOCK
  if(running) {
    // Analog clock
    if(analogInterval >= (intervalMillis)) {
      //Serial.print("bpm: ");
      //Serial.print(bpmRead);
      //Serial.print(" / ms: ");
      //Serial.print(intervalMillis);
      //Serial.print(" / r: ");
      //Serial.println(bpmRead);

      if(glitchActive) {
        glitchIntervalDivider = round(glitchIntervalDividers[random(5)]);
      }
      setAnalogGateHigh();
      setGlitchGateHigh();
      swapPoGate();
    }
  
    if(gateHigh && ((now - urzwergPulseWidth  - gateTimer) >= pulseWidth)) {
      setAnalogGateLow();
    }

    if(gateLedOn && ((now - urzwergPulseWidth  - gateTimer) >= gateLedLength)) {
      digitalWrite(gateLed, LOW);
      gateLedOn = false;
    }

    // 2x clock
    if(analogInterval >= (intervalMillis / 2)) {
      set2xGateHigh();
    }

    if(gate2xHigh && ((now - gateTimer) >= pulseWidth)) {
      set2xGateLow();
    }

    // Glitch clock
    if(glitchInterval >= (intervalMillis / glitchIntervalDivider)) {
      setGlitchGateHigh();
    }

    if(glitchGateHigh && ((now - glitchTimer) >= pulseWidth)) {
      setGlitchGateLow();
    }

    if(glitchLedOn && ((now - glitchTimer) >= glitchLedLength)) {
      digitalWrite(glitchLed, LOW);
      glitchLedOn = false;
    }
  
    // Urzwerg clock
    if(urzwergInterval >= (intervalMillis / 6)) {
      setUrzwergGateHigh();
    }
  
    if(urzwergGateHigh && ((now - urzwergGateTimer) >= urzwergPulseWidth)) {
      setUrzwergGateLow();
    }

    if(urzwergLedOn && ((now - urzwergGateTimer) >= urzwergLedLength)) {
      digitalWrite(urzwergClockLed, LOW);
      urzwergLedOn = false;
    }
  }
}
