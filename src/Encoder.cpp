#include "Encoder.h"

Encoder::Encoder(uint8_t pinA, uint8_t pinB, uint8_t pinBtn, int ticksPerClick)
    : pinA(pinA), pinB(pinB), pinBtn(pinBtn), ticksPerClick(ticksPerClick) {}

void Encoder::begin() {
    pinMode(pinA,   INPUT_PULLUP);
    pinMode(pinB,   INPUT_PULLUP);
    pinMode(pinBtn, INPUT_PULLUP);

    uint8_t a = digitalRead(pinA);
    uint8_t b = digitalRead(pinB);
    lastEncoded = (a << 1) | b;
}

void Encoder::update() {
   // ===========================
    //   ENCODER
    // ===========================
    uint8_t a = digitalRead(pinA);
    uint8_t b = digitalRead(pinB);
    uint8_t encoded = (a << 1) | b;

    if (encoded != lastEncoded) {
        uint8_t sum = (lastEncoded << 2) | encoded;

        const int32_t MIN_TICKS = 0;
        const int32_t MAX_POS   = 19;
        const int32_t MAX_TICKS = MAX_POS * ticksPerClick;

        // +1
        if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
            if (encoderTicks < MAX_TICKS) {
                encoderTicks++;
            }
        }
        // -1
        else if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
            if (encoderTicks > MIN_TICKS) {
                encoderTicks--;
            }
        }

        // Siempre nos mantenemos sincronizados con el estado del encoder
        lastEncoded = encoded;

        int32_t logicalPos = encoderTicks / ticksPerClick;

        if (logicalPos != lastLogicalPos) {
            lastLogicalPos = logicalPos;
            position       = logicalPos;

            // debug
            // Serial.print("ticks="); Serial.print(encoderTicks);
            // Serial.print(" pos=");   Serial.println(position);
        }
    }

    // ===========================
    //   BOTÓN (DEBOUNCE)
    // ===========================
    int reading = digitalRead(pinBtn);

    if (reading != lastButtonState) {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceMs) {
        if (reading != buttonState) {
            buttonState = reading;

            // Click detectado cuando pasa a LOW
            if (buttonState == LOW) {
                clickFlag = true;
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
