// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#include "theremin.h"

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
        StkFloat output = instrument->tick();
        if (instrument->echoEffect())
            output = instrument->echo().tick(output);
        if (instrument->lowPassFilter())
            output = instrument->lowPass().tick(output);
        if (instrument->highPassFilter())
            output = instrument->highPass().tick(output);
        *oSamples++ = output;
    }
    return 0;
}


Theremin::Theremin(void)
    : mVolume(0.5)
    , mGlobalVolume(1.0)
    , mFrequency(440.0)
    , mEchoEffect(false)
    , mLowPassFilter(false)
    , mHighPassFilter(false)
    , mInstrumentId(Silence)
{
    Stk::setSampleRate(44100.0);
    Stk::setRawwavePath( "../termenvox/STK/rawwaves/" );
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
    oparameters.nChannels = 1;
    RtAudioFormat format = (sizeof(StkFloat) == 8)? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;
    unsigned int bufferFrames = RT_BUFFER_SIZE;
    try {
        mDAC.openStream(&oparameters, NULL, format, (unsigned int)Stk::sampleRate(), &bufferFrames, &tickCallback, (void*)this);
    }
    catch (RtError& error) {
        error.printMessage();
    }
    try {
        mDAC.startStream();
    }
    catch (RtError& error) {
        error.printMessage();
    }
}


Theremin::~Theremin()
{
    stop();
    try {
        mDAC.closeStream();
    }
    catch (RtError& error) {
        error.printMessage();
    }
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


void Theremin::setLowPassFrequency(stk::StkFloat freq)
{
    mLowPassFilter = (freq > 0);
    mLowPass.setZero(freq/4000.);
}


void Theremin::setHighPassFrequency(stk::StkFloat freq)
{
    mHighPassFilter = (freq > 0);
    mHighPass.setPole(-freq/4000.);
}


void Theremin::setHighPassB0(stk::StkFloat freq)
{
    mHighPass.setB0(freq/4000.);
}


void Theremin::setHighPassB1(stk::StkFloat freq)
{
    mHighPass.setA1(freq/4000.);
}


void Theremin::setEcho(int delay)
{
    mEchoEffect = (delay > 0);
    mEcho.setDelay(delay);
}


void Theremin::chooseInstrument(Instrument instrumentId)
{
    mInstrumentId = instrumentId;
    switch (instrumentId)
    {
    case Violin:
        reinterpret_cast<stk::Bowed*>(mInstruments[Violin])->setVibrato(0.0);
        break;
    case Flute:
        reinterpret_cast<stk::Flute*>(mInstruments[Flute])->setJetReflection(0.5);
        reinterpret_cast<stk::Flute*>(mInstruments[Flute])->setEndReflection(-0.5);
        break;
    default:
        break;
    }
}

