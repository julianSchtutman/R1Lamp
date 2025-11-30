#include <Arduino.h>

// Encoder por polling (SIN interrupciones) para ESP8266
// - Se leen A y B en cada loop
// - Decodificador de cuadratura completo
// - Botón con debounce por millis()

const uint8_t PIN_ENC_A   = D5;   // Canal A del encoder
const uint8_t PIN_ENC_B   = D6;   // Canal B del encoder
const uint8_t PIN_BUTTON  = D7;   // Pulsador del encoder

int32_t encoderTicks = 0;          // Contador crudo
uint8_t lastEncoded  = 0;

// Para posición lógica (1 por clic)
int32_t lastLogicalPos = 0;
const int TICKS_PER_CLICK = 4;     // Probá cambiarlo si tu encoder es distinto

// ==== BOTÓN ====
bool     buttonState      = HIGH;
bool     lastButtonState  = HIGH;
uint32_t lastDebounceTime = 0;
const uint32_t debounceMs = 20;

void setup() {
  Serial.begin(115200);
  delay(200);

  pinMode(PIN_ENC_A,  INPUT_PULLUP);
  pinMode(PIN_ENC_B,  INPUT_PULLUP);
  pinMode(PIN_BUTTON, INPUT_PULLUP);

  uint8_t a = digitalRead(PIN_ENC_A);
  uint8_t b = digitalRead(PIN_ENC_B);
  lastEncoded = (a << 1) | b;

  Serial.println("Encoder por polling listo.");
}

void loop() {
  // ========= 1) ENCODER =========
  uint8_t a = digitalRead(PIN_ENC_A);
  uint8_t b = digitalRead(PIN_ENC_B);
  uint8_t encoded = (a << 1) | b;

  if (encoded != lastEncoded) {
    uint8_t sum = (lastEncoded << 2) | encoded;

    if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
      encoderTicks++;
    } else if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
      encoderTicks--;
    }

    lastEncoded = encoded;

    // Posición lógica: 1 unidad por "clic"
    int32_t logicalPos = encoderTicks / TICKS_PER_CLICK;

    if (logicalPos != lastLogicalPos) {
      int32_t deltaLogical = logicalPos - lastLogicalPos;
      lastLogicalPos = logicalPos;

      Serial.print("Pos: ");
      Serial.print(logicalPos);
      Serial.print("   Delta: ");
      Serial.println(deltaLogical);
    }
  }

  // ========= 2) BOTÓN =========
  int reading = digitalRead(PIN_BUTTON);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceMs) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        Serial.println("CLICK!");
      }
    }
  }

  lastButtonState = reading;

  // ========= 3) OTROS =========
  delay(1);
}

