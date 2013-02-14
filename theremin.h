// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#ifndef __THEREMIN_H_
#define __THEREMIN_H_

#include "RtAudio.h"
#include "Bowed.h"
#include "SineWave.h"

class Theremin
{
public:
    Theremin(void);
    ~Theremin();
    void setFrequency(double hertz);
    void setVolume(double);
    stk::StkFloat tick(void);

    void play(void);
    void stop(void);

    enum Instrument {
        Violin
    };

private:
    double mVolume;
    Instrument mInstrumentId;
    stk::Bowed mInstrument;
    RtAudio mDAC;
};

#endif // __THEREMIN_H_
