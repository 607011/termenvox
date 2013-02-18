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
#include "Chorus.h"
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
    void setEcho(int);
    void setChorusDepth(stk::StkFloat);
    void setChorusFrequency(stk::StkFloat);
    void setVolume(stk::StkFloat);
    void setGlobalVolume(stk::StkFloat);
    void chooseInstrument(Instrument instrumentId);
    const char* lastErrorMessage(void) const { return mError.what(); }
    const RtError::Type lastErrorType(void) const { return mError.getType(); }
    stk::StkFloat tick(void);
    stk::StkFloat lastOut(void) const;
    stk::Echo& echo(void) { return mEcho; }
    stk::Chorus& chorus(void) { return mChorus; }
    stk::OnePole& lowPass(void) { return mLowPass; }
    stk::OnePole& highPass(void) { return mHighPass; }
    bool echoEffect(void) const { return mEchoEffect; }
    bool chorusEffect(void) const { return mChorusEffect; }
    bool lowPassFilter(void) const { return mLowPassFilter; }
    bool highPassFilter(void) const { return mHighPassFilter; }

    void play(void);
    void stop(void);


private:
    stk::StkFloat mVolume;
    stk::StkFloat mGlobalVolume;
    stk::StkFloat mFrequency;
    bool mEchoEffect;
    bool mChorusEffect;
    bool mLowPassFilter;
    bool mHighPassFilter;
    stk::Instrmnt* mInstruments[LastInstrument];
    Instrument mInstrumentId;
    stk::Echo mEcho;
    stk::Chorus mChorus;
    stk::OnePole mLowPass;
    stk::OnePole mHighPass;
    RtAudio mDAC;
    RtError mError;
};

#endif // __THEREMIN_H_
