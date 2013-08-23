// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#include "leaplistener.h"

#include <QtCore/QDebug>
#include <qmath.h>

void LeapListener::onInit(const Leap::Controller&) {
    emit initialized();
}


void LeapListener::onConnect(const Leap::Controller& controller) {
    qDebug() << "Connected Leap Motion devices";
    const Leap::DeviceList& devices = controller.devices();
    for (Leap::DeviceList::const_iterator d = devices.begin(); d != devices.end(); ++d) {
        const Leap::Device& device = *d;
        qDebug() << "  "
                 << "horizontalViewAngle =" << (device.horizontalViewAngle() / M_PI * 180) << "°"
                 << ","
                 << "verticalViewAngle =" << (device.verticalViewAngle() / M_PI * 180) << "°"
                 << ","
                 << "range =" << device.range() << "mm";
    }
    emit connected();
}


void LeapListener::onDisconnect(const Leap::Controller&) {
    emit disconnected();
}


void LeapListener::onExit(const Leap::Controller&) {
    emit exited();
}


void LeapListener::onFrame(const Leap::Controller& controller) {
    const Leap::Frame frame = controller.frame();
    if (frame.hands().count() > 0) {
        const Leap::Hand& hand1 = frame.hands()[0];
        if (frame.hands().count() > 1) {
            const Leap::Hand& hand2 = frame.hands()[1];
            if (frame.hands()[0].palmPosition().x < 0) {
                emit handsDetected(hand1, hand2);
            }
            else {
                emit handsDetected(hand2, hand1);
            }
        }
        else if (frame.hands()[0].palmPosition().x < 0) {
            emit handsDetected(hand1, Leap::Hand());
        }
        else {
            emit handsDetected(Leap::Hand(), hand1);
        }
    }
    else {
        emit handsDetected(Leap::Hand(), Leap::Hand());
    }
}
