int gateLed = 0;
int urzwergClockLed = 1;
int urzwergStartLed = 2;
int glitchLed = 3;

int gateOut1 = 14;
int gateOutPo = 11;
int gateOut2x = 12;
int gateOutLsdj = 13;
int urzwergOut = 15;
int glitchOut = 16;

int glitchIn = 8;
int startIn = 9;
int stopIn = 10;
int bpmIn = A0;

int bpm = 160;

long clockTimer = 0;
long clockTimer2x = 0;
long clockTimer4x = 0;
long poTimer = 0;
long urzwergTimer = 0;
long glitchTimer = 0;

long gateTimer = 0;
long gateTimer2x = 0;
long gateTimer4x = 0;
long gateTimerPo = 0;
long urzwergGateTimer = 0;

long timeMultiplicator = 1000;

long  pulseWidth = 1 * timeMultiplicator;
long  pulseWidthPo = 10 * timeMultiplicator;
long  pulseWidth2x = 10 * timeMultiplicator;
long  pulseWidthLSDJ = 10 * timeMultiplicator;
long  urzwergPulseWidth = 1 * timeMultiplicator;
long  glitchLedLength = 25 * timeMultiplicator;
long  urzwergLedLength = 10 * timeMultiplicator;
long  gateLedLength = 25 * timeMultiplicator;

long buttonDebouncer = 0;
long debounceInterval = 100 * timeMultiplicator;

long bpmRef = 60000 * timeMultiplicator;

int glitchIntervalDivider = 1;
float glitchIntervalDividers[] = {0.25, 0.5, 4.0, 8.0, 16.0};

boolean gateHigh = false;
boolean gate2xHigh = false;
boolean gate4xHigh = false;
boolean glitchGateHigh = false;
boolean urzwergGateHigh = false;
boolean gatePoHigh = false;

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
  pinMode(gateOutPo, OUTPUT);
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
  digitalWrite(gateOutPo, LOW);
  digitalWrite(gateOut2x, LOW);
  digitalWrite(gateOutLsdj, LOW);
  
  resetAnalogTimers();
  buttonDebouncer = getTime();
}

long getTime() {
  return micros();
}

void resetAnalogTimers() {
  resetAnalogClockTimer();
  resetAnalogClockTimer2x();
  resetAnalogClockTimer4x();
  resetPoTimer();
  resetGlitchTimer();
  resetUrzwergTimer();
  resetUrzwergGateTimer();
  resetGateTimer();
  resetGateTimer2x();
  resetGateTimer4x();
  resetGateTimerPo();
}

void resetAnalogClockTimer() {
  clockTimer = getTime();
}

void resetAnalogClockTimer2x() {
  clockTimer2x = getTime();
}

void resetAnalogClockTimer4x() {
  clockTimer4x = getTime();
}

void resetPoTimer() {
  poTimer = getTime();
}

void resetGlitchTimer() {
  glitchTimer = getTime();
}

void resetGateTimer() {
  gateTimer = getTime();
}

void resetGateTimer2x() {
  gateTimer2x = getTime();
}

void resetGateTimer4x() {
  gateTimer4x = getTime();
}

void resetGateTimerPo() {
  gateTimerPo = getTime();
}

void resetUrzwergTimer() {
  urzwergTimer = getTime();
  resetUrzwergGateTimer();
}

void resetUrzwergGateTimer() {
  urzwergGateTimer = getTime();
}

long bpmToMillis(long bpm) {
  return round(bpmRef/bpm);
}

long millisToBpm(long milliseconds) {
  return round(bpmRef/milliseconds);
}

long bpmToMillis24Ppqn(long bpm) {
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
  digitalWrite(glitchLed, HIGH);
  gateHigh = true;
  gateLedOn = true;
  resetAnalogClockTimer();
  resetGateTimer();
}

void setAnalogGateLow() {
  digitalWrite(gateOut1, LOW);
  resetGateTimer();
  gateHigh = false;
}

// 2X GATE
void set2xGateHigh() {
  digitalWrite(gateOut2x, HIGH);
  gate2xHigh = true;
  resetAnalogClockTimer2x();
  resetGateTimer2x();
}

void set2xGateLow() {
  digitalWrite(gateOut2x, LOW);
  gate2xHigh = false;
}

// 4X / LSDJ GATE
void set4xGateHigh() {
  digitalWrite(gateOutLsdj, HIGH);
  gate4xHigh = true;
  resetAnalogClockTimer4x();
  resetGateTimer4x();
}

void set4xGateLow() {
  digitalWrite(gateOutLsdj, LOW);
  gate4xHigh = false;
}

// GLITCH GATE
void setGlitchGateHigh() {
  digitalWrite(glitchOut, HIGH);
  digitalWrite(glitchLed, HIGH);
  glitchGateHigh = true;
  glitchLedOn = true;
  resetGlitchTimer();
}

void setGlitchGateLow() {
  digitalWrite(glitchOut, LOW);
  glitchGateHigh = false;
}


// URZWERG GATE
void setUrzwergGateHigh() {
  digitalWrite(urzwergOut, HIGH);
  digitalWrite(urzwergClockLed, HIGH);
  urzwergGateHigh = true;
  urzwergLedOn = true;
  resetUrzwergTimer();
  resetUrzwergGateTimer();
}

void setUrzwergGateLow() {
  digitalWrite(urzwergOut, LOW);
  digitalWrite(urzwergStartLed, LOW);
  urzwergGateHigh = false;
}


// PO / 2x
void setPoGateHigh() {
  digitalWrite(gateOutPo, HIGH);
  gatePoHigh = true;
  resetPoTimer();
  resetGateTimerPo();
}

void setPoGateLow() {
  digitalWrite(gateOutPo, LOW);
  gatePoHigh = false;
}


// LOOP
void loop() {
  long bpmRead = round(analogRead(bpmIn) / 5);
  bpmRead = bpmRead >= 10 ? bpmRead : 10;
  long bpmDivider = 4;
  long interval = bpmToMillis(bpmRead) / bpmDivider;
  
  long now = getTime();
  long urzwergInterval = now - urzwergTimer;
  long analogInterval = now - clockTimer;
  long analogInterval2x = now - clockTimer2x;
  long analogInterval4x = now - clockTimer4x;
  long poInterval = now - poTimer;
  long glitchInterval = now - glitchTimer;


  // START/STOP BUTTON
  if(digitalRead(startIn) == HIGH) {
    if((now - buttonDebouncer) >= debounceInterval) {
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
      buttonDebouncer = getTime();
    }
  }


  // STOP BUTTON
  if(digitalRead(stopIn) == HIGH) {
    if((now - buttonDebouncer) >= debounceInterval) {
      Serial.println("STOP");
      resetAnalogTimers();
      running = false;
      buttonDebouncer = getTime();
      allLedsOff();
    }
  }


  // GLITCH BUTTON
  if(digitalRead(glitchIn) == HIGH) {
    if((now - buttonDebouncer) >= 100) {
      Serial.println("GLITCH");
      glitchActive = true;
      buttonDebouncer = getTime();
    }
  } else {
    glitchActive = false;
    glitchIntervalDivider = 1;
  }


  // CLOCK
  if(running) {
    // Analog clock
    if(analogInterval >= (interval)) {
      if(glitchActive) {
        glitchIntervalDivider = round(glitchIntervalDividers[random(5)]);
      }
      setAnalogGateHigh();
      setGlitchGateHigh();
    }
  
    if(gateHigh && ((now - urzwergPulseWidth  - gateTimer) >= pulseWidth)) {
      setAnalogGateLow();
    }

    if(gateLedOn && ((now - urzwergPulseWidth  - gateTimer) >= gateLedLength)) {
      digitalWrite(gateLed, LOW);
      gateLedOn = false;
    }

    // 2x clock
    if(analogInterval2x >= (interval / 2)) {
      set2xGateHigh();
    }

    if(gate2xHigh && ((now - gateTimer2x) >= pulseWidth2x)) {
      set2xGateLow();
    }

    // LSDJ/4x clock
    if(analogInterval4x >= (interval / 4)) {
      set4xGateHigh();
    }

    if(gate4xHigh && ((now - gateTimer4x) >= pulseWidthLSDJ)) {
      set4xGateLow();
    }

    // PO/2x clock
    if(poInterval >= (interval / 2)) {
      setPoGateHigh();
    }

    if(gatePoHigh && ((now - gateTimerPo) >= pulseWidthPo)) {
      setPoGateLow();
    }

    // Urzwerg/6x clock
    if(urzwergInterval >= (interval / 6)) {
      setUrzwergGateHigh();
    }
  
    if(urzwergGateHigh && ((now - urzwergGateTimer) >= urzwergPulseWidth)) {
      setUrzwergGateLow();
    }

    if(urzwergLedOn && ((now - urzwergGateTimer) >= urzwergLedLength)) {
      digitalWrite(urzwergClockLed, LOW);
      urzwergLedOn = false;
    }

    // Glitch clock
    if(glitchInterval >= (interval / glitchIntervalDivider)) {
      setGlitchGateHigh();
    }

    if(glitchGateHigh && ((now - glitchTimer) >= pulseWidth)) {
      setGlitchGateLow();
    }

    if(glitchLedOn && ((now - glitchTimer) >= glitchLedLength)) {
      digitalWrite(glitchLed, LOW);
      glitchLedOn = false;
    }
  }
}
