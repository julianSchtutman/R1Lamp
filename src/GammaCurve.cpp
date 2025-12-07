#include "GammaCurve.h"
#include <math.h>

GammaCurve::GammaCurve(float gamma)
: _gamma(gamma)
{
}

void GammaCurve::setGamma(float gamma) {
    _gamma = gamma;
}

float GammaCurve::getGamma() const {
    return _gamma;
}

float GammaCurve::apply(float x) const {
    // Clamp de seguridad al rango [0, 1]
    if (x <= 0.0f) return 0.0f;
    if (x >= 1.0f) return 1.0f;

    // Gamma = 1 => curva lineal (atajo barato)
    if (_gamma == 1.0f) {
        return x;
    }

    // Curva gamma estándar
    // y = x^gamma
    return powf(x, _gamma);
}
