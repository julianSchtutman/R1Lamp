#include "Encoder.h"

Encoder::Encoder(uint8_t pinA_, uint8_t pinB_, uint8_t pinBtn_,
                 int startPos_, int minPos_, int maxPos_)
: pinA(pinA_), pinB(pinB_), pinBtn(pinBtn_),
  minPos(minPos_), maxPos(maxPos_), startPos(startPos_)
{
}

void Encoder::updateNormalized() {
    const int steps = (maxPos - minPos + 1);
    normalizedPosition = (float)(position - minPos + 1) / (float)steps;   // 1/steps .. 1
}

void Encoder::begin() {
    pinMode(pinA, INPUT_PULLUP);
    pinMode(pinB, INPUT_PULLUP);
    pinMode(pinBtn, INPUT_PULLUP);

    lastEncoded = (digitalRead(pinA) << 1) | digitalRead(pinB);

    position = constrain(startPos, minPos, maxPos);

    // ticks coherentes con position y minPos
    encoderTicks = (int32_t)(position - minPos) * ticksPerClick;

    updateNormalized();
}


void Encoder::update() {
    // ===========================
    //   ENCODER
    // ===========================
    
    if(!freezed){
        uint8_t a = digitalRead(pinA);
        uint8_t b = digitalRead(pinB);
        uint8_t encoded = (a << 1) | b;
    
        if (encoded != lastEncoded) {
            uint8_t sum = (lastEncoded << 2) | encoded;
    
            // ticks en rango 0..(maxPos-minPos)*ticksPerClick
            const int32_t MIN_TICKS = 0;
            const int32_t MAX_TICKS = (int32_t)(maxPos - minPos) * ticksPerClick;
    
            // +1
            if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
                if (encoderTicks < MAX_TICKS) encoderTicks++;
            }
            // -1
            else if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
                if (encoderTicks > MIN_TICKS) encoderTicks--;
            }
    
            lastEncoded = encoded;
    
            int32_t logicalPos = encoderTicks / ticksPerClick;   // 0..(maxPos-minPos)
            int newPosition = minPos + (int)logicalPos;          // minPos..maxPos
    
            if (newPosition != position) {
                position = newPosition;
                if (position < minPos) position = minPos;
                if (position > maxPos) position = maxPos;
                updateNormalized();
            }
        }
    }

    // ===========================
    //   BOTÓN (DEBOUNCE)
    // ===========================
    bool reading = digitalRead(pinBtn);

    if (reading != lastButtonState) {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceMs) {

        if (reading != buttonState) {
            buttonState = reading;

            // PRESIONADO
            if (buttonState == LOW) {
                pressStartTimeMs = millis();
                longFired = false;
            }
            // LIBERADO
            else {
                if (!longFired) clickFlag = true;
            }
        }

        // mientras está presionado, dispara long al cumplir umbral
        if (buttonState == LOW && !longFired) {
            uint32_t held = millis() - pressStartTimeMs;
            if (held >= longClickMs) {
                longClickFlag = true;
                longFired = true;
            }
        }
    }

    lastButtonState = reading;
}

bool Encoder::wasClicked() {
    if (clickFlag) {
        clickFlag = false;
        return true;
    }
    return false;
}

bool Encoder::wasLongClicked() {
    if (longClickFlag) {
        longClickFlag = false;
        return true;
    }
    return false;
}

void Encoder::freeze() {
    freezed = true;
}

void Encoder::unFreeze() {
    freezed = false;
}
