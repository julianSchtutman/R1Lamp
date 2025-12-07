#pragma once

#include <stdint.h>

struct LightMix {
    float warm;  // 0..1
    float cool;  // 0..1
};

class Composer {
public:
    // coolStart: brillo a partir del cual empieza a aparecer el frío (0..1)
    // coolEnd:   brillo donde el frío llega a su máximo share (0..1)
    // coolMaxShare: porción máxima del "budget" total que se va al frío (0..1)
    // coolGamma: controla cómo entra el frío (1 = lineal, >1 = más lento al principio)
    Composer(float coolStart    = 0.4f,
             float coolEnd      = 0.8f,
             float coolMaxShare = 0.35f,
             float coolGamma    = 1.6f);

    void  setCoolStart(float v);
    void  setCoolEnd(float v);
    void  setCoolMaxShare(float v);
    void  setCoolGamma(float v);

    float getCoolStart()    const;
    float getCoolEnd()      const;
    float getCoolMaxShare() const;
    float getCoolGamma()    const;

    // brightness: 0..1 (ya gamma-correcto)
    // devuelve cuánta porción de ese brillo va a warm / cool (también 0..1)
    LightMix compose(float brightness) const;

private:
    float _coolStart;     // umbral de aparición del frío
    float _coolEnd;       // umbral de saturación del frío
    float _coolMaxShare;  // fracción máxima del budget que va al frío
    float _coolGamma;     // forma de la curva de entrada del frío

    float clamp01(float v) const;
};
