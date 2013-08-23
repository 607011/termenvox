// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#ifndef __LEAPLISTENER_H_
#define __LEAPLISTENER_H_

#include <QObject>
#include <Leap.h>

class LeapListener : public QObject, public Leap::Listener
{
    Q_OBJECT
public:
    void onInit(const Leap::Controller&);
    void onConnect(const Leap::Controller&);
    void onDisconnect(const Leap::Controller&);
    void onExit(const Leap::Controller&);
    void onFrame(const Leap::Controller&);

signals:
    void handsDetected(const Leap::Hand& left, const Leap::Hand& right);
    void initialized(void);
    void connected(void);
    void disconnected(void);
    void exited(void);
};

Q_DECLARE_METATYPE(Leap::Hand)


#endif // __LEAPLISTENER_H_
