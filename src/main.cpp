#include <Arduino.h>
#include "Encoder.h"

const uint8_t PIN_ENC_A   = D5;
const uint8_t PIN_ENC_B   = D6;
const uint8_t PIN_BUTTON  = D7;

Encoder enc(PIN_ENC_A, PIN_ENC_B, PIN_BUTTON, 4);   // TICKS_PER_CLICK = 4

void setup() {
    Serial.begin(115200);
    delay(200);

    enc.begin();
    Serial.println("Encoder + Botón iniciado.");
}

void loop() {
    enc.update();

    int pos = enc.getPosition();  // 0..19
    static int lastPos = -1;

    if (pos != lastPos) {
        lastPos = pos;
        Serial.print("Posicion: ");
        Serial.println(pos);
    }

    if (enc.wasClicked()) {
        Serial.println("CLICK!");
    }

    delay(1);
}
