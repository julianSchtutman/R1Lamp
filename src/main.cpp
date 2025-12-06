#include <Arduino.h>
#include "Encoder.h"

const uint8_t PIN_ENC_A   = D5;
const uint8_t PIN_ENC_B   = D6;
const uint8_t PIN_BUTTON  = D7;

Encoder enc(PIN_ENC_A, PIN_ENC_B, PIN_BUTTON);  

void setup() {
    Serial.begin(115200);
    delay(200);

    enc.begin();
    Serial.println("Encoder + Botón iniciado.");
}

void loop() {
    enc.update();

    int pos = enc.getAbsolutePosition();  // 0..19
    static int lastPos = -1;

    if (pos != lastPos) {
        lastPos = pos;
        Serial.print("Posicion: ");
        Serial.println(pos);
        float normPos = enc.getNormalizedPosition();
        Serial.print("Posicion normalizada: ");
        Serial.println(normPos);
    }

    if (enc.wasClicked()) {
        Serial.println("CLICK!");
    }

    delay(1);
}
