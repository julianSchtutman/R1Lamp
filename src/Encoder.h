#pragma once
#include <Arduino.h>

class Encoder {
public:
    Encoder(uint8_t pinA, uint8_t pinB, uint8_t pinBtn, int ticksPerClick = 4);

    void begin();
    void update();

    int  getPosition() const { return position; }  // 0..19
    bool wasClicked();                             // true 1 sola vez por click
    bool isPressed() const { return (buttonState == LOW); }

private:
    // Pines
    uint8_t pinA, pinB, pinBtn;

    // Encoder
    int ticksPerClick;
    int32_t encoderTicks = 0;
    int32_t lastLogicalPos = 0;
    uint8_t lastEncoded = 0;
    int position = 0;               // 0..19

    // Botón
    bool     buttonState      = HIGH;
    bool     lastButtonState  = HIGH;
    uint32_t lastDebounceTime = 0;
    const uint32_t debounceMs = 20;
    bool     clickFlag        = false; // almacena un click detectado
};
