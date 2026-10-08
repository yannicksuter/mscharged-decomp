#ifndef GAME_AUDIO_AUDIO_STREAM_HEADER_H
#define GAME_AUDIO_AUDIO_STREAM_HEADER_H

struct AudioStreamHeader
{
    unsigned long num_samples;
    unsigned long num_adpcm_nibbles;
    unsigned long sample_rate;
    unsigned short loop_flag;
    unsigned short format;
    unsigned long sa;
    unsigned long ea;
    unsigned long ca;
    unsigned short coef[16];
    unsigned short gain;
    unsigned short ps;
    unsigned short yn1;
    unsigned short yn2;
    unsigned short lps;
    unsigned short lyn1;
    unsigned short lyn2;
    unsigned short pad[11];
};

#endif // GAME_AUDIO_AUDIO_STREAM_HEADER_H
