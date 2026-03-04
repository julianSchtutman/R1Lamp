#include <Arduino.h>
#include "Encoder.h"
#include "GammaCurve.h"
#include "Composer.h"
#include "PwmMapper.h"
#include "Transition.h"

const uint8_t PIN_ENC_A    = 32;
const uint8_t PIN_ENC_B    = 33;
const uint8_t PIN_BUTTON   = 25;
const uint8_t PIN_WARM_PWM = 18;  
const uint8_t PIN_COOL_PWM = 19;  

static const uint8_t ENCODER_MIN_POS   = 1;
static const uint8_t ENCODER_MAX_POS   = 10;
static const uint8_t ENCODER_START_POS = 3;

static bool lampOn = true;
static PwmLevels lastOnTarget = {100, 0};

Encoder enc(PIN_ENC_A,
            PIN_ENC_B,
            PIN_BUTTON,
            ENCODER_START_POS,
            ENCODER_MIN_POS,
            ENCODER_MAX_POS); 

GammaCurve gC(2.6f);
Composer   composer(
    0.5f,  // coolStart: desde x% de brillo empieza a entrar el frío
    0.75f,  // coolEnd:   a x% ya llegó a su mezcla máxima
    0.20f, // coolMaxShare: a lo sumo x% del budget se va al frío
    2.0f   // coolGamma: entra suave al principio, más decidido al final
);

//Solo canal warm
//Composer   composer(
//    1.0f,  
//    1.0f,  
//    0.0f, 
//    2.0f   
//);

PwmMapper pwmMapper(
    100,   // pwmWarmMin
    4095, // pwmWarmMax
    0,   // pwmCoolMin
    4095  // pwmCoolMax
);

Transition transition(PIN_WARM_PWM, PIN_COOL_PWM, 800);
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

    // Toggle ON/OFF con long click
    if (enc.wasClicked()) {
        lampOn = !lampOn;

        if (!lampOn) {
            Serial.println("Apagar.........");
            transition.setTarget({0, 0});
            //enc.freeze();
        } else {
            Serial.println("Encender........");
            transition.setTarget(lastOnTarget);
            enc.unFreeze();
        }
    }

    int pos = enc.getAbsolutePosition();
    static int lastPos = -1;

    // Solo aplico cambios del encoder si está encendida
    if (lampOn && pos != lastPos) {
        lastPos = pos;

        float normPos = enc.getNormalizedPosition();
        float brightness = gC.apply(normPos);
        LightMix mix = composer.compose(brightness);
        PwmLevels targetPwm = pwmMapper.toPwm(mix);

        // guardo último target “encendido”
        if (targetPwm.warm != 0 || targetPwm.cool != 0) {
            lastOnTarget = targetPwm;
        }

        transition.setTarget(targetPwm);
    }

    transition.update();

    if (enc.wasClicked()) {
        Serial.println("CLICK!");
    }

    delay(1);
}
