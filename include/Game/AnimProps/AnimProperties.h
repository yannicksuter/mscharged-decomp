#ifndef ANIM_PROPERTIES_H
#define ANIM_PROPERTIES_H

struct AnimProperties
{
    const char* name;
    const char* animation;
    unsigned int flagsA;
    float blend;
    union
    {
        unsigned int flagsB;
        struct
        {
            bool mirror;
            unsigned char mPadding11[3];
        };
    };
    int ballRotationMode;
    unsigned int flagsD;
    unsigned int flagsE;
};

#endif
