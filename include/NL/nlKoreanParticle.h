#ifndef NL_KOREAN_PARTICLE_H
#define NL_KOREAN_PARTICLE_H

// Selects the WITH/WITHOUT variant of a Korean localization particle.
enum KoreanParticle
{
    KOREAN_PARTICLE_WITH = 0,
    KOREAN_PARTICLE_WITHOUT = 1,
};

KoreanParticle GetKoreanParticle(const unsigned short* string);
KoreanParticle GetKoreanParticle(int number);

#endif // NL_KOREAN_PARTICLE_H
