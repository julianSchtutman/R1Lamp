#pragma once

#include <stdint.h>
#include "PwmMapper.h"   // por el struct PwmLevels

class Transition {
public:
    // durationMs: duración total de la transición (por ejemplo 150 ms)
    Transition(uint8_t pinWarm,
               uint8_t pinCool,
               uint16_t durationMs = 150);

    // Llamar una vez en setup
    void begin();

    // Arranca (o reinicia) una transición hacia 'target'.
    // Se puede llamar aunque haya una transición en curso:
    // toma como inicio el nivel actual en ese momento.
    void setTarget(const PwmLevels& target);

    // Llamar en cada loop(). No bloquea.
    void update();

    // Devuelve el nivel PWM actual (último escrito a los pines)
    PwmLevels current() const { return _current; }

    // Cambiar duración en runtime (opcional)
    void setDuration(uint16_t durationMs) { _durationMs = durationMs; }
    uint16_t getDuration() const { return _durationMs; }

private:
    uint8_t  _pinWarm;
    uint8_t  _pinCool;
    uint16_t _durationMs;
    uint16_t _freq;
    uint8_t  _bits;
    uint8_t _chWarm;
    uint8_t _chCool;

    bool      _active;      // ¿hay transición en curso?
    uint32_t  _startTime;   // millis() al inicio de la transición
    PwmLevels _start;       // desde dónde arrancamos
    PwmLevels _target;      // hacia dónde vamos
    PwmLevels _current;     // valor actual aplicado
    bool _isDimming;  // true si estamos bajando brillo


    // Función de interpolación (por ahora lineal; se puede cambiar a cuadrática)
    float ease(float t) const;

    // Interpola entre start y target según t en [0,1]
    PwmLevels interpolate(float t) const;

    void writePwm(const PwmLevels& v) const;
};
