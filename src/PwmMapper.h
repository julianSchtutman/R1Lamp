#pragma once

#include <stdint.h>
#include "Composer.h"  // por LightMix
                       // (o solo declarás struct LightMix si preferís)

struct PwmLevels {
    uint16_t warm;
    uint16_t cool;
};

class PwmMapper {
public:
    // Constructor:
    //  - pwmWarmMin / pwmWarmMax: rango físico del canal cálido
    //  - pwmCoolMin / pwmCoolMax: rango físico del canal frío
    //
    // Ejemplo típico en ESP8266:
    //  pwmWarmMin = 40, pwmWarmMax = 1023
    //  pwmCoolMin = 40, pwmCoolMax = 1023
    PwmMapper(uint16_t pwmWarmMin,
              uint16_t pwmWarmMax,
              uint16_t pwmCoolMin,
              uint16_t pwmCoolMax);

    void setWarmRange(uint16_t minVal, uint16_t maxVal);
    void setCoolRange(uint16_t minVal, uint16_t maxVal);

    // Convierte la mezcla normalizada (0..1) en niveles PWM para cada canal.
    // - mix.warm y mix.cool vienen en 0..1 desde Composer.
    // - Devuelve PWM en el rango configurado.
    PwmLevels toPwm(const LightMix& mix) const;

private:
    uint16_t _pwmWarmMin;
    uint16_t _pwmWarmMax;
    uint16_t _pwmCoolMin;
    uint16_t _pwmCoolMax;

    float clamp01(float v) const;
    uint16_t mapChannel(float v, uint16_t minVal, uint16_t maxVal) const;
};
