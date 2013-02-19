// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#ifndef __THEREMIN_H_
#define __THEREMIN_H_

#include "RtAudio.h"
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

    stk::StkFloat tick(void);
    stk::StkFloat lastOut(void) const;

    const char* lastErrorMessage(void) const { return mError.what(); }
    const RtError::Type lastErrorType(void) const { return mError.getType(); }

    // effects
    inline stk::Chorus& chorus(void) { return mChorus; }
    inline bool chorusEffect(void) const { return mChorusEffect; }
    void setChorusDepth(stk::StkFloat);
    void setChorusFrequency(stk::StkFloat);

    inline stk::Echo& echo(void) { return mEcho; }
    inline bool echoEffect(void) const { return mEchoEffect; }
    void setEcho(int);

    inline stk::Envelope& envelope(void) { return mEnvelope; }
    inline bool envelopeEffect(void) const { return mEnvelopeEffect; }

    inline stk::FreeVerb& freeVerb(void) { return  mFreeVerb; }
    void setFreeVerbDamping(stk::StkFloat);
    void setFreeVerbRoomSize(stk::StkFloat);
    inline bool freeVerbEffect(void) const { return mFreeVerbEffect; }

    inline stk::JCRev& jcRev(void) { return mJCRev; }
    inline bool jcRevEffect(void) const { return mJCRevEffect; }

    inline stk::LentPitShift& lentPitShift(void) { return  mLentPitShift; }
    void setLentPitShift(stk::StkFloat);
    inline bool lentPitShiftEffect(void) const { return mLentPitShiftEffect; }

    inline stk::NRev& nRev(void) { return mNRev; }
    inline bool nRevEffect(void) const { return mNRevEffect; }

    inline stk::PitShift& pitShift(void) { return  mPitShift; }
    void setPitShift(stk::StkFloat);
    inline bool pitShiftEffect(void) const { return mPitShiftEffect; }

    inline stk::PRCRev& prcRev(void) { return mPRCRev; }
    inline bool prcRevEffect(void) const { return mPRCRevEffect; }

    // filters
    inline stk::OnePole& lowPass(void) { return mLowPass; }
    inline bool lowPassFilter(void) const { return mLowPassFilter; }
    void setLowPassFrequency(stk::StkFloat);

    inline stk::OnePole& highPass(void) { return mHighPass; }
    void setHighPassFrequency(stk::StkFloat);
    inline bool highPassFilter(void) const { return mHighPassFilter; }

    inline stk::Iir& iir(void) { return mIir; }
    inline bool iirFilter(void) const { return mIirFilter; }


private:
    stk::StkFloat mVolume;
    stk::StkFloat mGlobalVolume;
    stk::StkFloat mFrequency;
    bool mLowPassFilter;
    bool mHighPassFilter;
    bool mIirFilter;
    stk::Instrmnt* mInstruments[LastInstrument];
    Instrument mInstrumentId;
    // effects
    stk::Echo mEcho;
    stk::Chorus mChorus;
    stk::Envelope mEnvelope;
    stk::PRCRev mPRCRev;
    stk::JCRev mJCRev;
    stk::NRev mNRev;
    stk::FreeVerb mFreeVerb;
    stk::PitShift mPitShift;
    stk::LentPitShift mLentPitShift;
    bool mEchoEffect;
    bool mChorusEffect;
    bool mEnvelopeEffect;
    bool mPRCRevEffect;
    bool mJCRevEffect;
    bool mNRevEffect;
    bool mFreeVerbEffect;
    bool mPitShiftEffect;
    bool mLentPitShiftEffect;
    // filter
    stk::OnePole mLowPass;
    stk::OnePole mHighPass;
    stk::Iir mIir;
    RtAudio mDAC;
    RtError mError;
};

#endif // __THEREMIN_H_
