#include <Arduino.h>
#include "Encoder.h"
#include "GammaCurve.h"
#include "Composer.h"
#include "PwmMapper.h"
#include "Transition.h"

const uint8_t PIN_ENC_A   = D5;
const uint8_t PIN_ENC_B   = D6;
const uint8_t PIN_BUTTON  = D7;
const uint8_t PIN_WARM_PWM = D1;  
const uint8_t PIN_COOL_PWM = D2;  

Encoder enc(PIN_ENC_A, PIN_ENC_B, PIN_BUTTON);  
GammaCurve gC(2.8f);
Composer   composer(
    0.5f,  // coolStart: desde 40% de brillo empieza a entrar el frío
    0.75f,  // coolEnd:   a 80% ya llegó a su mezcla máxima
    0.20f, // coolMaxShare: a lo sumo 35% del budget se va al frío
    2.0f   // coolGamma: entra suave al principio, más decidido al final
);

PwmMapper pwmMapper(
    25,   // pwmWarmMin
    1023, // pwmWarmMax
    20,   // pwmCoolMin
    350  // pwmCoolMax
);

Transition transition(PIN_WARM_PWM, PIN_COOL_PWM, 150);
PwmLevels currentPwm = {0, 0};  // estado inicial lógico (opcional)


void setup() {
    Serial.begin(115200);
    delay(200);

    enc.begin();
    transition.begin();
    Serial.println("Lampara inicializada.");
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

            
        // Si cambió el destino, arrancamos una nueva transición
        static PwmLevels lastTarget = {0, 0};
        if (targetPwm.warm != lastTarget.warm || targetPwm.cool != lastTarget.cool) {
          transition.setTarget(targetPwm);
          lastTarget = targetPwm;
        }



    }

    // Avanzamos la transición según el tiempo
    transition.update();

    if (enc.wasClicked()) {
        Serial.println("CLICK!");
    }

    if (enc.wasLongClicked()) {
        Serial.println("LONG CLICKED!");
    }

    delay(1);
}
