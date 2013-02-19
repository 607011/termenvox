// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#include "theremin.h"
#include "Stk.h"
#include <QtCore/QDebug>

using namespace stk;


int tickCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void* pData)
{
    (void)inputBuffer;
    (void)streamTime;
    (void)status;
    Theremin* instrument = reinterpret_cast<Theremin*>(pData);
    StkFloat* oSamples = reinterpret_cast<StkFloat*>(outputBuffer);
    for (unsigned int i = 0; i < nBufferFrames; ++i) {
        StkFloat loutput = instrument->tick();
        StkFloat routput = instrument->lastOut();
        // effects with mono input and mono output
        if (instrument->echoEffect()) {
            loutput = instrument->echo().tick(loutput);
            routput = loutput;
        }
        if (instrument->pitShiftEffect()) {
            loutput = instrument->pitShift().tick(loutput);
            routput = loutput;
        }
        if (instrument->lentPitShiftEffect()) {
            loutput = instrument->lentPitShift().tick(loutput);
            routput = loutput;
        }
        // effects with mono input and stereo output
        if (instrument->nRevEffect()) {
            loutput = instrument->nRev().tick(loutput);
            routput = instrument->nRev().lastOut(1);
        }
        if (instrument->jcRevEffect()) {
            loutput = instrument->jcRev().tick(loutput);
            routput = instrument->jcRev().lastOut(1);
        }
        if (instrument->prcRevEffect()) {
            loutput = instrument->prcRev().tick(loutput);
            routput = instrument->prcRev().lastOut(1);
        }
        if (instrument->chorusEffect()) {
            loutput = instrument->chorus().tick(loutput);
            routput = instrument->chorus().lastOut(1);
        }
        // effects with stereo input
        if (instrument->freeVerbEffect()) {
            loutput = instrument->freeVerb().tick(loutput, routput);
            routput = instrument->freeVerb().lastOut(1);
        }
        // filters with mono(!) input and mono(!) output
        if (instrument->lowPassFilter()) {
            loutput = instrument->lowPass().tick(loutput);
            routput = loutput;
        }
        if (instrument->highPassFilter()) {
            loutput = instrument->highPass().tick(loutput);
            routput = loutput;
        }
        *oSamples++ = loutput;
        *oSamples++ = routput;
    }
    return 0;
}


Theremin::Theremin(void)
    : mVolume(0.5)
    , mGlobalVolume(1.0)
    , mFrequency(440.0)
    , mEchoEffect(false)
    , mChorusEffect(false)
    , mEnvelopeEffect(false)
    , mPRCRevEffect(false)
    , mJCRevEffect(false)
    , mNRevEffect(false)
    , mFreeVerbEffect(false)
    , mPitShiftEffect(false)
    , mLentPitShiftEffect(false)
    , mLowPassFilter(false)
    , mHighPassFilter(false)
    , mIirFilter(false)
    , mInstrumentId(Silence)
    , mError("")
{
    Stk::setSampleRate(44100.0);
#ifdef QT_NO_DEBUG
    Stk::setRawwavePath( "./rawwaves/" );
#else
    Stk::setRawwavePath( "../termenvox/STK/rawwaves/" );
#endif
    mInstruments[Violin] = new stk::Bowed;
    mInstruments[Clarinet] = new stk::Clarinet;
    mInstruments[Flute] = new stk::Flute(80);
    mInstruments[PercFlute] = new stk::PercFlut;
    mInstruments[Wurley] = new stk::Wurley;
    mInstruments[HeavyMetalFM] = new stk::HevyMetl;
    mInstruments[BeeThree] = new stk::BeeThree;
    mInstruments[Rhodey] = new stk::Rhodey;
    mInstruments[FMVoices] = new stk::FMVoices;
    RtAudio::StreamParameters oparameters;
    oparameters.deviceId = mDAC.getDefaultOutputDevice();
    oparameters.nChannels = 2;
    RtAudioFormat format = (sizeof(StkFloat) == 8)? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;
    unsigned int bufferFrames = RT_BUFFER_SIZE;
    try {
        mDAC.openStream(&oparameters, NULL, format, (unsigned int)Stk::sampleRate(), &bufferFrames, &tickCallback, (void*)this);
    }
    catch (RtError& error) {
        mError = error;
        return;
    }
    try {
        mDAC.startStream();
    }
    catch (RtError& error) {
        mError = error;
        return;
    }
}


Theremin::~Theremin()
{
    stop();
    try {
        mDAC.closeStream();
    }
    catch (RtError&) { /* ... */ }
    for (int i = 0; i < LastInstrument; ++i)
        if (mInstruments[i] != NULL)
            delete mInstruments[i];
}


void Theremin::play(void)
{
    switch (mInstrumentId) {
    case Violin:
        reinterpret_cast<stk::Bowed*>(mInstruments[Violin])->startBowing(1.0, 40000);
        break;
    case Clarinet:
        reinterpret_cast<stk::Clarinet*>(mInstruments[Clarinet])->noteOn(mFrequency, 1.0);
        reinterpret_cast<stk::Clarinet*>(mInstruments[Clarinet])->startBlowing(1.0, 40000);
        break;
    case Flute:
        mInstruments[Flute]->noteOn(mFrequency, 1.0);
        reinterpret_cast<stk::Flute*>(mInstruments[Flute])->startBlowing(1.0, 0);
        break;
    case PercFlute:
        mInstruments[PercFlute]->noteOn(mFrequency, 1.0);
        break;
    case Wurley:
        mInstruments[Wurley]->noteOn(mFrequency, 1.0);
        break;
    case HeavyMetalFM:
        mInstruments[HeavyMetalFM]->noteOn(mFrequency, 1.0);
        break;
    case BeeThree:
        mInstruments[BeeThree]->noteOn(mFrequency, 1.0);
        break;
    case Rhodey:
        mInstruments[Rhodey]->noteOn(mFrequency, 1.0);
        break;
    case FMVoices:
        mInstruments[FMVoices]->noteOn(mFrequency, 1.0);
        break;
    default:
        break;
    }
}


void Theremin::stop(void)
{
    switch (mInstrumentId) {
    case Violin:
        reinterpret_cast<stk::Bowed*>(mInstruments[Violin])->stopBowing(40000);
        break;
    case Clarinet:
        reinterpret_cast<stk::Clarinet*>(mInstruments[Clarinet])->stopBlowing(40000);
        break;
    case Flute:
        reinterpret_cast<stk::Flute*>(mInstruments[Flute])->stopBlowing(40000);
        break;
    case PercFlute:
        mInstruments[PercFlute]->noteOff(1.0);
        break;
    case Wurley:
        mInstruments[Wurley]->noteOff(1.0);
        break;
    case HeavyMetalFM:
        mInstruments[HeavyMetalFM]->noteOff(1.0);
        break;
    case BeeThree:
        mInstruments[BeeThree]->noteOff(1.0);
        break;
    case Rhodey:
        mInstruments[Rhodey]->noteOff(1.0);
        break;
    case FMVoices:
        mInstruments[FMVoices]->noteOff(1.0);
        break;
    default:
        break;
    }
}


StkFloat Theremin::tick(void)
{
    return (mInstrumentId != Silence)? mGlobalVolume * mVolume * mInstruments[mInstrumentId]->tick() : 0;
}



stk::StkFloat Theremin::lastOut(void) const
{
    return (mInstrumentId != Silence)? mGlobalVolume * mVolume * mInstruments[mInstrumentId]->lastOut() : 0;
}


void Theremin::setFrequency(stk::StkFloat hertz)
{
    mFrequency = hertz;
    if (mInstrumentId != Silence)
        mInstruments[mInstrumentId]->setFrequency(mFrequency);
}


void Theremin::setVolume(stk::StkFloat volume)
{
    mVolume = volume;
}


void Theremin::setGlobalVolume(stk::StkFloat volume)
{
    mGlobalVolume = volume;
}


void Theremin::setLowPassFrequency(stk::StkFloat pole)
{
    mLowPassFilter = (pole > 0);
    mLowPass.setPole(pole);
}


void Theremin::setHighPassFrequency(stk::StkFloat pole)
{
    mHighPassFilter = (pole > 0);
    mHighPass.setPole(-pole);
}


void Theremin::setEcho(int delay)
{
    mEchoEffect = (delay > 0);
    mEcho.setDelay(delay);
}


void Theremin::setFreeVerbDamping(StkFloat damping)
{
    mFreeVerbEffect = (damping > 0);
    mFreeVerb.setDamping(damping);
}


void Theremin::setFreeVerbRoomSize(StkFloat roomsize)
{
    mFreeVerbEffect = (roomsize > 0);
    mFreeVerb.setRoomSize(roomsize);
}


void Theremin::setPitShift(StkFloat shift)
{
    mPitShiftEffect = (shift > 0);
    mPitShift.setShift(0.3 + 1.4 * shift);
}


void Theremin::setLentPitShift(StkFloat shift)
{
    mLentPitShiftEffect = (shift > 0);
    mLentPitShift.setShift(0.3 + 1.4 * shift);
}


void Theremin::setNRevDecay(StkFloat decay)
{
    mNRevEffect = (decay > 0);
    mNRev.setT60(5.0 * decay);
}


void Theremin::setJCRevDecay(StkFloat decay)
{
    mJCRevEffect = (decay > 0);
    mJCRev.setT60(5.0 * decay);
}


void Theremin::setPRCRevDecay(StkFloat decay)
{
    mPRCRevEffect = (decay > 0);
    mPRCRev.setT60(5.0 * decay);
}


void Theremin::setChorusDepth(stk::StkFloat depth)
{
    mChorusEffect = (depth > 0);
    mChorus.setModDepth(depth);
}


void Theremin::setChorusFrequency(stk::StkFloat freq)
{
    mChorusEffect = (freq > 0);
    mChorus.setModFrequency(freq);
}


void Theremin::chooseInstrument(Instrument instrumentId)
{
    mInstrumentId = instrumentId;
    switch (instrumentId)
    {
    case Violin:
    {
        stk::Bowed* const violin = reinterpret_cast<stk::Bowed*>(mInstruments[Violin]);
        violin->setVibrato(0.0);
        break;
    }
    case Flute:
    {
        stk::Flute* const flute = reinterpret_cast<stk::Flute*>(mInstruments[Flute]);
        flute->setJetReflection(0.5);
        flute->setEndReflection(-0.5);
        break;
    }
    default:
        break;
    }
}
