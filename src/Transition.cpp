#include "Transition.h"
#include <Arduino.h>  // millis(), analogWrite

Transition::Transition(uint8_t pinWarm,
                       uint8_t pinCool,
                       uint16_t durationMs)
: _pinWarm(pinWarm)
, _pinCool(pinCool)
, _durationMs(durationMs)
, _active(false)
, _startTime(0)
{
    _start   = {0, 0};
    _target  = {0, 0};
    _current = {0, 0};
}

void Transition::begin() {
    pinMode(_pinWarm, OUTPUT);
    pinMode(_pinCool, OUTPUT);
    analogWrite(_pinWarm, 0);
    analogWrite(_pinCool, 0);
}

void Transition::setTarget(const PwmLevels& target) {
    // Brillo total actual y objetivo (approx)
    uint32_t currentSum = (uint32_t)_current.warm + (uint32_t)_current.cool;
    uint32_t targetSum  = (uint32_t)target.warm + (uint32_t)target.cool;

    _isDimming = (targetSum < currentSum);

    _start     = _current;
    _target    = target;
    _startTime = millis();
    _active    = true;

    if (_durationMs == 0) {
        _current = _target;
        writePwm(_current);
        _active = false;
    }
}


void Transition::update() {
    if (!_active) {
        // Por las dudas, aseguramos que los pines reflejen _current
        writePwm(_current);
        return;
    }

    uint32_t now = millis();
    uint32_t elapsed = now - _startTime;

    if (elapsed >= _durationMs) {
        // Llegamos al final de la transición
        _current = _target;
        writePwm(_current);
        _active = false;
        return;
    }

    // t va de 0 a 1
    float t = (float)elapsed / (float)_durationMs;
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    float e = ease(t);                // aplicamos la función de easing
    _current = interpolate(e);        // interpolamos entre start y target    
    writePwm(_current);
}

float Transition::ease(float t) const {
    if (_isDimming) {
        // Bajando: que frene MUCHO al final (más suave al llegar a lo oscuro)

        // Combinación de ease-in + smoothstep, por ejemplo:
        float s = t * t * (3.0f - 2.0f * t);  // smoothstep base
        return s * s;  // lo “cuadramos” para que se pegue más al 0
    } else {
        // Subiendo: smoothstep normal
        return t * t * (3.0f - 2.0f * t);
    }
}

PwmLevels Transition::interpolate(float t) const {
    PwmLevels v;

    int16_t dw = (int16_t)_target.warm - (int16_t)_start.warm;
    int16_t dc = (int16_t)_target.cool - (int16_t)_start.cool;

    v.warm = _start.warm + (int16_t)(dw * t);
    v.cool = _start.cool + (int16_t)(dc * t);

    return v;
}

void Transition::writePwm(const PwmLevels& v) const {
    analogWrite(_pinWarm, v.warm);
    analogWrite(_pinCool, v.cool);
}
