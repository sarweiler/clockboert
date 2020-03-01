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
long glitchTimer = 0;

long intervalOffset1x = 0;
long intervalOffset2x = 0;
long intervalOffsetPo = 0;
long intervalOffset4x = 0;
long intervalOffset6x = 0;

long timeMultiplicator = 1000;

long  pulseWidth1x = 10 * timeMultiplicator;
long  pulseWidthPo = 10 * timeMultiplicator;
long  pulseWidth2x = 10 * timeMultiplicator;
long  pulseWidthLSDJ = 10 * timeMultiplicator;
long  pulseWidthUrzwerg = 1 * timeMultiplicator;

long gateRef1x = pulseWidth1x;
long gateRef2x = pulseWidth2x;
long gateRefPo = pulseWidthPo;
long gateRef4x = pulseWidthLSDJ;
long gateRef6x = pulseWidthUrzwerg;

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
  
  resetAnalogClockTimer();
  resetGlitchTimer();
  buttonDebouncer = getTime();
}

long getTime() {
  return micros();
}

void resetIntervalOffsets() {
  intervalOffset2x = 0;
  intervalOffsetPo = 0;
  intervalOffset4x = 0;
  intervalOffset6x = 0;
}

void resetGateRefs() {
  gateRef1x = pulseWidth1x;
  gateRef2x = pulseWidth2x;
  gateRefPo = pulseWidthPo;
  gateRef4x = pulseWidthLSDJ;
  gateRef6x = pulseWidthUrzwerg;
}

void resetAnalogClockTimer() {
  clockTimer = getTime();
}

void resetGlitchTimer() {
  glitchTimer = getTime();
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
}

void setAnalogGateLow() {
  digitalWrite(gateOut1, LOW);
  digitalWrite(gateLed, LOW);
  gateHigh = false;
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

// 4X / LSDJ GATE
void set4xGateHigh() {
  digitalWrite(gateOutLsdj, HIGH);
  gate4xHigh = true;
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
}

void setUrzwergGateLow() {
  digitalWrite(urzwergOut, LOW);
  digitalWrite(urzwergClockLed, LOW);
  digitalWrite(urzwergStartLed, LOW);
  urzwergGateHigh = false;
}


// PO / 2x
void setPoGateHigh() {
  //Serial.println("PO HIGH");
  digitalWrite(gateOutPo, HIGH);
  gatePoHigh = true;
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
  
  long interval1x = bpmToMillis(bpmRead) / bpmDivider;
  long interval2x = round(interval1x / 2);
  long intervalPo = round(interval1x / 1);
  long interval4x = round(interval1x / 4);
  long interval6x = round(interval1x / 6);

  long intervalWithOffset2x = interval2x + intervalOffset2x;
  long intervalWithOffsetPo = intervalPo + intervalOffsetPo;
  long intervalWithOffset4x = interval4x + intervalOffset4x;
  long intervalWithOffset6x = interval6x + intervalOffset6x;
  
  long now = getTime();
  long analogInterval = now - clockTimer;
  long glitchInterval = now - glitchTimer;


  // START/STOP BUTTON
  if(digitalRead(startIn) == HIGH) {
    if((now - buttonDebouncer) >= debounceInterval) {
      digitalWrite(urzwergStartLed, HIGH);
      delay(100);
      if(!running) {
        Serial.println("START");
        // Weird Urzwerg needs a delay and
        // an extra impulse for syncing correctly
        setUrzwergGateHigh(); 
        running = true;
      } else {
        Serial.println("STOP");
        running = false;
        allLedsOff();
      }
      resetAnalogClockTimer();
      resetGlitchTimer();
      buttonDebouncer = getTime();
    }
  }


  // STOP BUTTON
  if(digitalRead(stopIn) == HIGH) {
    if((now - buttonDebouncer) >= debounceInterval) {
      Serial.println("STOP");
      resetAnalogClockTimer();
      resetGlitchTimer();
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
    
    // 2x clock
    if(analogInterval >= intervalWithOffset2x) {
      set2xGateHigh();
      intervalOffset2x += interval2x;
      gateRef2x = intervalWithOffset2x + pulseWidth2x;
    }

    if(gate2xHigh && (analogInterval >= gateRef2x)) {
      set2xGateLow();
    }


    // PO clock
    if(analogInterval >= (intervalWithOffsetPo)) {
      setPoGateHigh();
      intervalOffsetPo += intervalPo;
      gateRefPo = intervalWithOffsetPo + pulseWidthPo;
    }

    if(gatePoHigh && (analogInterval >= gateRefPo)) {
      setPoGateLow();
    }


    // LSDJ/4x clock
    if(analogInterval >= (intervalWithOffset4x)) {
      set4xGateHigh();
      intervalOffset4x += interval4x;
      gateRef4x = intervalWithOffset4x + pulseWidthLSDJ;
    }

    if(gate4xHigh && (analogInterval >= gateRef4x)) {
      set4xGateLow();
    }


    // Urzwerg/6x clock
    if(analogInterval >= (intervalWithOffset6x)) {
      setUrzwergGateHigh();
      intervalOffset6x += interval6x;
      gateRef6x = intervalWithOffset6x + pulseWidthUrzwerg;
    }
  
    if(urzwergGateHigh && (analogInterval >= gateRef6x)) {
      setUrzwergGateLow();
    }

    // Glitch clock
    if(glitchInterval >= (interval1x / glitchIntervalDivider)) {
      setGlitchGateHigh();
    }

    if(glitchGateHigh && ((now - glitchTimer) >= pulseWidth1x)) {
      digitalWrite(glitchLed, LOW);
      setGlitchGateLow();
    }


    // Analog clock
    if(gateHigh && (analogInterval >= gateRef1x)) {
      setAnalogGateLow();
    }
    
    if(analogInterval >= interval1x) {
      if(glitchActive) {
        glitchIntervalDivider = round(glitchIntervalDividers[random(5)]);
      }
      gateRef1x = interval1x + pulseWidth1x;
      setAnalogGateHigh();
      setGlitchGateHigh();
      resetIntervalOffsets();
      resetGateRefs();
    }
  }
}
