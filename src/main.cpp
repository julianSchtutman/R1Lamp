#include <Arduino.h>
#include <math.h>
#include "Encoder.h"   // tu clase

// =========================
//  HARDWARE
// =========================
const uint8_t PIN_ENC_A   = D5;
const uint8_t PIN_ENC_B   = D6;
const uint8_t PIN_LED_PWM = D2;   // salida al MOSFET

// Instancia de tu encoder
Encoder encoder(PIN_ENC_A, PIN_ENC_B,0,4);

// =========================
//  CONFIG BRILLO / GAMMA
// =========================

// Cantidad de niveles lógicos que devuelve tu encoder:
// si va de 0 a 19 -> BRIGHTNESS_STEPS = 19
const int16_t BRIGHTNESS_STEPS = 19;

const uint16_t PWM_MAX = 1023;

// 👇 Estos dos son los que vas a ir tocando entre reflasheos
const float    GAMMA   = 2.6f;   // probá 1.8, 2.0, 2.4, 2.6...
const uint16_t PWM_MIN = 70;     // probá 0, 20, 40, 60...

// =========================
//  RAMPA ENTRE PASOS
// =========================

// Duración total de la rampa por cada “click” de encoder
const uint16_t FADE_TIME_MS = 200;   // 100–200 ms suele sentirse bien
// Cantidad de pasos intermedios dentro de esa rampa
const uint8_t  FADE_STEPS   = 50;

uint16_t currentPwm = 0;  // último PWM aplicado
int16_t  lastPos    = -1; // última posición de encoder usada

// =========================
//  FUNCIONES
// =========================

// Rampa bloqueante de 'from' -> 'to' en FADE_TIME_MS
void fadePwm(uint16_t from, uint16_t to) {
  if (from == to) {
    analogWrite(PIN_LED_PWM, to);
    return;
  }

  int16_t diff = (int16_t)to - (int16_t)from;
  uint16_t stepDelay = (FADE_STEPS > 0) ? (FADE_TIME_MS / FADE_STEPS) : 0;

  for (uint8_t i = 1; i <= FADE_STEPS; i++) {
    uint16_t v = from + (diff * i) / FADE_STEPS;
    analogWrite(PIN_LED_PWM, v);
    // Bloqueante a propósito para el prototipo
    delay(stepDelay);
    // Para que el ESP8266 no se cuelgue:
    yield();
  }
}

// Convierte posición de encoder -> PWM objetivo
uint16_t computePwmFromPosition(int16_t pos) {
  // Clamp de seguridad
  if (pos < 0) pos = 0;
  if (pos > BRIGHTNESS_STEPS) pos = BRIGHTNESS_STEPS;

  // Nivel 0 = apagado completo
  if (pos == 0) {
    return 0;
  }

  // Normalizamos a 0..1
  float b = (float)pos / (float)BRIGHTNESS_STEPS;

  // Curva gamma
  float out = powf(b, GAMMA);  // 0..1

  // Map a PWM respetando PWM_MIN
  uint16_t span = PWM_MAX - PWM_MIN;
  uint16_t pwm  = PWM_MIN + (uint16_t)(out * span);

  return pwm;
}

// =========================
//  SETUP / LOOP
// =========================

void setup() {
  Serial.begin(115200);
  delay(200);
  encoder.begin();

  pinMode(PIN_LED_PWM, OUTPUT);
  analogWriteRange(1023);

  Serial.println(F("=== Test R1: gamma + rampa por paso de encoder ==="));
  Serial.print(F("BRIGHTNESS_STEPS = ")); Serial.println(BRIGHTNESS_STEPS);
  Serial.print(F("GAMMA            = ")); Serial.println(GAMMA, 2);
  Serial.print(F("PWM_MIN          = ")); Serial.println(PWM_MIN);
  Serial.print(F("FADE_TIME_MS     = ")); Serial.println(FADE_TIME_MS);
  Serial.print(F("FADE_STEPS       = ")); Serial.println(FADE_STEPS);
}

void loop() {
  // Si tu clase necesita update por polling, dejá esto.
  // Si usa interrupciones y no hace falta, podés comentarlo.
  encoder.update();

  // ⚠️ Cambiá este método al que realmente tengas:
  // por ejemplo getLogicalPosition(), getPos(), etc.
  int16_t pos = encoder.getPosition();

  // Si no cambió la posición, no hacemos nada
  if (pos == lastPos) {
    return;
  }

  // Nueva posición -> nuevo PWM objetivo
  uint16_t pwmTarget = computePwmFromPosition(pos);

  Serial.print(F("pos="));
  Serial.print(pos);
  Serial.print(F("  from="));
  Serial.print(currentPwm);
  Serial.print(F("  to="));
  Serial.println(pwmTarget);

  // Rampa entre el PWM actual y el nuevo
  fadePwm(currentPwm, pwmTarget);
  currentPwm = pwmTarget;
  lastPos    = pos;
}