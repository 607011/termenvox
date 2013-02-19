// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#ifndef __THEREMIN_H_
#define __THEREMIN_H_

#include "RtAudio.h"
#include "Mutex.h"
// wave forms
#include "Bowed.h"
#include "Clarinet.h"
#include "Flute.h"
// instruments
#include "Instrmnt.h"
#include "PercFlut.h"
#include "Wurley.h"
#include "HevyMetl.h"
#include "BeeThree.h"
#include "Rhodey.h"
#include "FMVoices.h"
// effect
#include "Echo.h"
#include "Envelope.h"
#include "PRCRev.h"
#include "JCRev.h"
#include "NRev.h"
#include "FreeVerb.h"
#include "Echo.h"
#include "PitShift.h"
#include "LentPitShift.h"
// filters
#include "Iir.h"
#include "Chorus.h"
#include "OnePole.h"

#include <vector>

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
    void setVolume(stk::StkFloat);
    void setGlobalVolume(stk::StkFloat);

    void chooseInstrument(Instrument instrumentId);
    void play(void);
    void stop(void);

    inline void lock(void) { mMutex.lock(); }
    inline void unlock(void) { mMutex.unlock(); }

    stk::StkFloat tick(void);
    stk::StkFloat lastOut(void) const;

    const char* lastErrorMessage(void) const { return mError.what(); }
    const RtError::Type lastErrorType(void) const { return mError.getType(); }

    // effects
    inline stk::Chorus& chorus(void) { return mChorus; }
    void setChorusDepth(stk::StkFloat);
    void setChorusFrequency(stk::StkFloat);

    inline stk::Echo& echo(void) { return mEcho; }
    void setEcho(int);

    inline stk::Envelope& envelope(void) { return mEnvelope; }

    inline stk::FreeVerb& freeVerb(void) { return  mFreeVerb; }
    void setFreeVerbDamping(stk::StkFloat);
    void setFreeVerbRoomSize(stk::StkFloat);

    inline stk::JCRev& jcRev(void) { return mJCRev; }
    void setJCRevDecay(stk::StkFloat);

    inline stk::LentPitShift& lentPitShift(void) { return  mLentPitShift; }
    void setLentPitShift(stk::StkFloat);

    inline stk::NRev& nRev(void) { return mNRev; }
    void setNRevDecay(stk::StkFloat);

    inline stk::PitShift& pitShift(void) { return  mPitShift; }
    void setPitShift(stk::StkFloat);

    inline stk::PRCRev& prcRev(void) { return mPRCRev; }
    void setPRCRevDecay(stk::StkFloat);

    // filters
    inline stk::OnePole& lowPass(void) { return mLowPass; }
    void setLowPassFrequency(stk::StkFloat);

    inline stk::OnePole& highPass(void) { return mHighPass; }
    void setHighPassFrequency(stk::StkFloat);

    enum Postprocessing {
        ChorusEffect,
        EchoEffect,
        PitchShiftEffect,
        LentPitchShiftEffect,
        NRevEffect,
        JCRevEffect,
        PRCRevEffect,
        FreeVerbEffect,
        LowPassFilter,
        HighPassFilter,
        LastPostprocessor
    };

    struct Effect {
        Theremin::Postprocessing id;
        bool enabled;
    };

    void setEffects(std::vector<Effect>& effects)
    {
        lock();
        mEffects = effects;
        unlock();
    }
    const std::vector<Effect>& effects(void) const { return mEffects; }

private:
    stk::StkFloat mVolume;
    stk::StkFloat mGlobalVolume;
    stk::StkFloat mFrequency;
    stk::Instrmnt* mInstruments[LastInstrument];
    Instrument mInstrumentId;
    // effects
    stk::Chorus mChorus;
    stk::Echo mEcho;
    stk::Envelope mEnvelope;
    stk::FreeVerb mFreeVerb;
    stk::JCRev mJCRev;
    stk::LentPitShift mLentPitShift;
    stk::NRev mNRev;
    stk::PitShift mPitShift;
    stk::PRCRev mPRCRev;
    // filter
    stk::OnePole mLowPass;
    stk::OnePole mHighPass;
    stk::Mutex mMutex;
    RtAudio mDAC;
    RtError mError;
    std::vector<Theremin::Effect> mEffects;
};

#endif // __THEREMIN_H_
