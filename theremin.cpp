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
    , mFilterEnabled(true)
    , mPole(0.9)
{
    Stk::setSampleRate(44100.0);
    mBaseSine.setFrequency(300000);
    mBaseSine.addPhase(0);
    mFilter.setPole(mPole);
    RtAudio::StreamParameters parameters;
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


stk::StkFloat Theremin::tick(void)
{
    return (mFilterEnabled)
            ? mFilter.tick(mVolume * mSine.tick() * mBaseSine.tick())
            : mVolume * mSine.tick() * mBaseSine.tick();
}


void Theremin::setFrequency(double hertz)
{
    mSine.setFrequency(hertz);
}


void Theremin::setBaseFrequency(double hertz)
{
    mBaseSine.setFrequency(hertz);
}


void Theremin::setFilterGain(double amplitude)
{
    mFilter.setGain(1e-3*amplitude);
}


void Theremin::setVolume(double volume)
{
    mVolume = volume;
}


void Theremin::setFilterEnabled(bool enabled)
{
    mFilterEnabled = enabled;
}


void Theremin::setPole(double z)
{
    mFilter.setPole(z);
}
