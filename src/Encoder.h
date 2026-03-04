#pragma once
#include <Arduino.h>

class Encoder {
public:
    Encoder(uint8_t pinA, uint8_t pinB, uint8_t pinBtn,
        int startPos = 10,
        int minPos = 1,
        int maxPos = 20);

    void begin();
    void update();

    int   getAbsolutePosition() const { return position; }          // 0..MAX_POS
    float getNormalizedPosition() const { return normalizedPosition; }
    bool  wasClicked();                                             // true 1 sola vez por click corto
    bool  wasLongClicked();                                         // true 1 sola vez por click largo
    bool  isPressed() const { return (buttonState == LOW); }
    void freeze();
    void unFreeze();

private:
    // Pines
    uint8_t pinA, pinB, pinBtn;

    // Encoder
    int ticksPerClick = 4;
    int32_t encoderTicks = 0;
    uint8_t lastEncoded = 0;
    int position = 0;                     // minpos..19
    float normalizedPosition = 0.0f;      // x..1
    bool freezed = false;
    
    //init positon config
    int minPos = 1;
    int maxPos = 20;
    int startPos = 10;

    // Botón
    bool     buttonState      = HIGH;
    bool     lastButtonState  = HIGH;
    uint32_t lastDebounceTime = 0;
    const uint32_t debounceMs = 20;
    
    // Click corto/largo (flags one-shot)
    bool     clickFlag        = false;    // click corto detectado
    bool     longClickFlag    = false;    // click largo detectado
    bool     longFired = false;

    // Long click timing
    uint32_t pressStartTimeMs = 0;
    const uint32_t longClickMs = 700;     // umbral (ajustable)

    void  updateNormalized();
};