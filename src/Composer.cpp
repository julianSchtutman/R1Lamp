#include "Composer.h"
#include <math.h>

Composer::Composer(float coolStart,
                   float coolEnd,
                   float coolMaxShare,
                   float coolGamma)
: _coolStart(coolStart)
, _coolEnd(coolEnd)
, _coolMaxShare(coolMaxShare)
, _coolGamma(coolGamma)
{
}

void Composer::setCoolStart(float v) {
    _coolStart = clamp01(v);
}

void Composer::setCoolEnd(float v) {
    _coolEnd = clamp01(v);
}

void Composer::setCoolMaxShare(float v) {
    // no tiene sentido que sea <0 o >1
    if (v < 0.0f) v = 0.0f;
    if (v > 1.0f) v = 1.0f;
    _coolMaxShare = v;
}

void Composer::setCoolGamma(float v) {
    if (v < 0.1f) v = 0.1f;  // evitar cosas raras
    _coolGamma = v;
}

float Composer::getCoolStart() const {
    return _coolStart;
}

float Composer::getCoolEnd() const {
    return _coolEnd;
}

float Composer::getCoolMaxShare() const {
    return _coolMaxShare;
}

float Composer::getCoolGamma() const {
    return _coolGamma;
}

LightMix Composer::compose(float brightness) const {
    LightMix mix{0.0f, 0.0f};

    // Clamp de seguridad
    brightness = clamp01(brightness);
    if (brightness <= 0.0f) {
        return mix;
    }

    // Caso 1: solo cálido (por debajo de coolStart)
    if (brightness <= _coolStart) {
        mix.warm = brightness;
        mix.cool = 0.0f;
        return mix;
    }

    // Por si coolEnd <= coolStart (config rara),
    // tratamos todo como "ya saturado"
    float coolRatio = 0.0f;

    if (_coolEnd <= _coolStart) {
        // frío directamente en su máximo share
        coolRatio = _coolMaxShare;
    } else {
        // brightness en [coolStart, coolEnd] -> t en [0,1]
        float t = (brightness - _coolStart) / (_coolEnd - _coolStart);
        t = clamp01(t);

        // Curva de entrada del frío (ease-in controlado por _coolGamma)
        float shaped = powf(t, _coolGamma);  // 0..1

        // En t=1 llegamos a coolMaxShare
        coolRatio = _coolMaxShare * shaped;
    }

    // Warm usa el resto del "budget"
    float warmRatio = 1.0f - coolRatio;

    // Distribuimos el "budget" de brillo:
    // warm + cool = brightness (invariante)
    mix.warm = brightness * warmRatio;
    mix.cool = brightness * coolRatio;

    return mix;
}

float Composer::clamp01(float v) const {
    if (v < 0.0f) return 0.0f;
    if (v > 1.0f) return 1.0f;
    return v;
}
