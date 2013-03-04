// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#include "leaplistener.h"


void LeapListener::onInit(const Leap::Controller&) {
    emit initialized();
}


void LeapListener::onConnect(const Leap::Controller&) {
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
