#include "PwmMapper.h"

PwmMapper::PwmMapper(uint16_t pwmWarmMin,
                     uint16_t pwmWarmMax,
                     uint16_t pwmCoolMin,
                     uint16_t pwmCoolMax)
: _pwmWarmMin(pwmWarmMin)
, _pwmWarmMax(pwmWarmMax)
, _pwmCoolMin(pwmCoolMin)
, _pwmCoolMax(pwmCoolMax)
{
}

void PwmMapper::setWarmRange(uint16_t minVal, uint16_t maxVal) {
    _pwmWarmMin = minVal;
    _pwmWarmMax = maxVal;
}

void PwmMapper::setCoolRange(uint16_t minVal, uint16_t maxVal) {
    _pwmCoolMin = minVal;
    _pwmCoolMax = maxVal;
}

PwmLevels PwmMapper::toPwm(const LightMix& mix) const {
    PwmLevels out;

    // Cada canal se mapea en forma independiente
    out.warm = mapChannel(mix.warm, _pwmWarmMin, _pwmWarmMax);
    out.cool = mapChannel(mix.cool, _pwmCoolMin, _pwmCoolMax);

    return out;
}

float PwmMapper::clamp01(float v) const {
    if (v < 0.0f) return 0.0f;
    if (v > 1.0f) return 1.0f;
    return v;
}

uint16_t PwmMapper::mapChannel(float v, uint16_t minVal, uint16_t maxVal) const {
    // v viene en 0..1
    v = clamp01(v);

    // Si la orden es "0", apagamos de verdad el canal
    if (v <= 0.0f) {
        return 0;
    }

    // Rango efectivo
    if (maxVal <= minVal) {
        // Config rara, devolvemos todo o nada
        return maxVal;
    }

    uint16_t span = maxVal - minVal;
    uint16_t pwm  = minVal + (uint16_t)(v * span);

    return pwm;
}
