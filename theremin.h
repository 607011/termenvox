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
#include "Echo.h"
#include "OneZero.h"
#include "OnePole.h"


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

    void setFrequency(stk::StkFloat hertz);
    void setLowPassFrequency(stk::StkFloat);
    void setHighPassFrequency(stk::StkFloat);
    void setHighPassB0(stk::StkFloat);
    void setHighPassB1(stk::StkFloat);
    void setEcho(int);
    void setVolume(stk::StkFloat);
    void setGlobalVolume(stk::StkFloat);
    void chooseInstrument(Instrument instrumentId);
    stk::StkFloat tick(void);
    stk::Echo& echo(void) { return mEcho; }
    stk::OneZero& lowPass(void) { return mLowPass; }
    stk::OnePole& highPass(void) { return mHighPass; }
    bool echoEffect(void) const { return mEchoEffect; }
    bool lowPassFilter(void) { return mLowPassFilter; }
    bool highPassFilter(void) { return mHighPassFilter; }

    void play(void);
    void stop(void);


private:
    stk::StkFloat mVolume;
    stk::StkFloat mGlobalVolume;
    stk::StkFloat mFrequency;
    bool mEchoEffect;
    bool mLowPassFilter;
    bool mHighPassFilter;
    stk::Instrmnt* mInstruments[LastInstrument];
    Instrument mInstrumentId;
    stk::Echo mEcho;
    stk::OneZero mLowPass;
    stk::OnePole mHighPass;
    RtAudio mDAC;
};

#endif // __THEREMIN_H_
