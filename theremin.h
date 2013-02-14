// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#ifndef __THEREMIN_H_
#define __THEREMIN_H_

#include "RtAudio.h"
#include "Instrmnt.h"
#include "Bowed.h"
#include "Clarinet.h"
#include "Flute.h"
#include "PercFlut.h"
#include "Wurley.h"
#include "HevyMetl.h"
#include "BeeThree.h"
#include "Rhodey.h"
#include "FMVoices.h"

class Theremin
{
public:
    Theremin(void);
    ~Theremin();

    enum Instrument {
        Silence = -1,
        Violin,
        Clarinet,
        Flute,
        PercFlute,
        Wurley,
        HeavyMetalFM,
        BeeThree,
        Rhodey,
        FMVoices,
        LastInstrument
    };

    void setFrequency(double hertz);
    void setVolume(double);
    void setGlobalVolume(double);
    void chooseInstrument(Instrument instrumentId);
    stk::StkFloat tick(void);

    void play(void);
    void stop(void);



private:
    double mVolume;
    double mGlobalVolume;
    double mFrequency;
    stk::Instrmnt* mInstruments[LastInstrument];
    Instrument mInstrumentId;
    RtAudio mDAC;
};

#endif // __THEREMIN_H_
