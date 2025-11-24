#include <Arduino.h>

// Pines del encoder
const int pinCLK = D5;   // A
const int pinDT  = D6;   // B
const int pinSW  = D7;   // Switch (botón)

// Estado previo
int lastCLK = HIGH;
int encoderValue = 0;

unsigned long lastButtonTime = 0;

void setup() {
    Serial.begin(115200);

    pinMode(pinCLK, INPUT_PULLUP);
    pinMode(pinDT,  INPUT_PULLUP);
    pinMode(pinSW,  INPUT_PULLUP);

    lastCLK = digitalRead(pinCLK);

    Serial.println("Encoder listo!");
}

void loop() {

    // --------- Lectura de giro ----------
    int currentCLK = digitalRead(pinCLK);

    if (currentCLK != lastCLK) {               // hubo cambio en A
        if (digitalRead(pinDT) != currentCLK)  // determina la dirección
            encoderValue++;
        else
            encoderValue--;

        Serial.print("Encoder: ");
        Serial.println(encoderValue);
    }

    lastCLK = currentCLK;


    // --------- Lectura del switch ----------
    if (digitalRead(pinSW) == LOW) {
        if (millis() - lastButtonTime > 250) { // debounce
            Serial.println("Click!");
        }
        lastButtonTime = millis();
    }
}
