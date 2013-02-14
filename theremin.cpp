// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#include "theremin.h"

using namespace stk;


int _tick(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void* pData)
{
    (void)(inputBuffer);
    (void)(streamTime);
    (void)(status);
    Theremin* instrument = reinterpret_cast<Theremin*>(pData);
    StkFloat* samples = reinterpret_cast<StkFloat*>(outputBuffer);
    for (unsigned int i = 0; i < nBufferFrames; ++i)
        *samples++ = instrument->tick();
    return 0;
}


Theremin::Theremin(void)
    : mVolume(0.5)
    , mInstrumentId(Violin)
{
    Stk::setSampleRate(44100.0);
    RtAudio::StreamParameters parameters;
    switch (mInstrumentId) {
    case Violin:
        mInstrument.setVibrato(0.0);
        break;
    default:
        break;
    }
    parameters.deviceId = mDAC.getDefaultOutputDevice();
    parameters.nChannels = 1;
    RtAudioFormat format = (sizeof(StkFloat) == 8)? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;
    unsigned int bufferFrames = RT_BUFFER_SIZE;
    try {
        mDAC.openStream(&parameters, NULL, format, (unsigned int)Stk::sampleRate(), &bufferFrames, &_tick, (void*)this);
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
    try {
        mDAC.closeStream();
    }
    catch (RtError& error) {
        error.printMessage();
    }
}


void Theremin::play(void)
{
    switch (mInstrumentId) {
    case Violin:
        mInstrument.startBowing(0.5, 40000);
        break;
    default:
        break;
    }
}

void Theremin::stop(void)
{
    switch (mInstrumentId) {
    case Violin:
        mInstrument.stopBowing(40000);
        break;
    default:
        break;
    }
}


stk::StkFloat Theremin::tick(void)
{
    return mVolume * mInstrument.tick();
}


void Theremin::setFrequency(double hertz)
{
    mInstrument.setFrequency(hertz);
}


void Theremin::setVolume(double volume)
{
    mVolume = volume;
}
