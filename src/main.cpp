#include <Arduino.h>
#include "Encoder.h"
#include "GammaCurve.h"
#include "Composer.h"
#include "PwmMapper.h"

const uint8_t PIN_ENC_A   = D5;
const uint8_t PIN_ENC_B   = D6;
const uint8_t PIN_BUTTON  = D7;

Encoder enc(PIN_ENC_A, PIN_ENC_B, PIN_BUTTON);  
GammaCurve gC(2.2f);
Composer   composer(
    0.4f,  // coolStart: desde 40% de brillo empieza a entrar el frío
    0.8f,  // coolEnd:   a 80% ya llegó a su mezcla máxima
    0.35f, // coolMaxShare: a lo sumo 35% del budget se va al frío
    1.6f   // coolGamma: entra suave al principio, más decidido al final
);

PwmMapper pwmMapper(
    40,   // pwmWarmMin
    1023, // pwmWarmMax
    40,   // pwmCoolMin
    1023  // pwmCoolMax
);

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
        
        float brightness = gC.apply(normPos);
        Serial.print("Posicion gamma: ");
        Serial.println(brightness);
        
        LightMix mix     = composer.compose(brightness);
        Serial.print("Warm: ");
        Serial.print(mix.warm);
        Serial.print(" - Cool: ");
        Serial.println(mix.cool);

        PwmLevels targetPwm = pwmMapper.toPwm(mix);
        Serial.print("Warm: pwm ");
        Serial.print(targetPwm.warm);
        Serial.print(" - Cool pwm: ");
        Serial.println(targetPwm.cool);


    }

    if (enc.wasClicked()) {
        Serial.println("CLICK!");
    }

    delay(1);
}
