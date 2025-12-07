#include <Arduino.h>
#include "Encoder.h"
#include "GammaCurve.h"

const uint8_t PIN_ENC_A   = D5;
const uint8_t PIN_ENC_B   = D6;
const uint8_t PIN_BUTTON  = D7;

Encoder enc(PIN_ENC_A, PIN_ENC_B, PIN_BUTTON);  
GammaCurve gC(2.2f);

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
        float gammaValue = gC.apply(normPos);
        Serial.print("Posicion gamma: ");
        Serial.println(gammaValue);
    }

    if (enc.wasClicked()) {
        Serial.println("CLICK!");
    }

    delay(1);
}
