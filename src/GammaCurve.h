#pragma once

#include <stdint.h>

class GammaCurve {
public:
    // Constructor: por defecto gamma = 1.0 (curva lineal)
    explicit GammaCurve(float gamma = 1.0f);

    // Setters / getters por si querés tunear gamma en runtime
    void  setGamma(float gamma);
    float getGamma() const;

    // Aplica la curva gamma:
    // - input x debe venir normalizado en el rango [0.0, 1.0]
    // - devuelve también un valor normalizado [0.0, 1.0]
    float apply(float x) const;

private:
    float _gamma;
};