// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#ifndef __THEREMIN_H_
#define __THEREMIN_H_

#include "RtAudio.h"
#include "SineWave.h"
#include "OnePole.h"


class Theremin
{
public:
    Theremin(void);
    ~Theremin();
    void setFrequency(double hertz);
    void setBaseFrequency(double hertz);
    void setFilterGain(double amplitude);
    void setVolume(double);
    void setPole(double z);
    void setFilterEnabled(bool);
    stk::StkFloat tick(void);
    bool filterEnabled(void) const { return mFilterEnabled; }

private:
    double mVolume;
    bool mFilterEnabled;
    double mPole;
    stk::SineWave mBaseSine;
    stk::SineWave mSine;
    stk::OnePole mFilter;
    RtAudio mDAC;
};

#endif // __THEREMIN_H_
