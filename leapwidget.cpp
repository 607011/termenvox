// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.


#include "leapwidget.h"
#include <QPainter>
#include <QtCore/QDebug>

using namespace Leap;


LeapWidget::LeapWidget(QWidget* parent)
    : QWidget(parent)
    , mController(NULL)
    , mHalfWidth(0.0)
    , mMaxOffsetX(100.0)
    , mMaxOffsetY(100.0)
{
    setMinimumSize(320, 240);
    setMaximumSize(1920, 1080);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    qRegisterMetaType<Leap::Hand>();
    QObject::connect(&mListener, SIGNAL(handsDetected(Leap::Hand, Leap::Hand)), SLOT(setHands(Leap::Hand, Leap::Hand)));
    QObject::connect(&mListener, SIGNAL(handsDetected(Leap::Hand, Leap::Hand)), SIGNAL(handsDetected(Leap::Hand, Leap::Hand)));
}


LeapWidget::~LeapWidget()
{
    stopSensor();
}


QPointF LeapWidget::posToPoint(const Leap::Vector& pos) const
{
    return QPointF(mHalfWidth + mHalfWidth * pos.x / mMaxOffsetX, height() - height() * (qreal(pos.y) / mMaxOffsetY));
}


void LeapWidget::paintFingers(QPainter& p, const FingerList& fingers)
{
    if (!fingers.empty()) {
        Leap::Vector avgPos;
        p.setPen(QColor(255, 140, 0));
        for (int i = 0; i < fingers.count(); ++i) {
            const Leap::Vector& tip = fingers[i].tipPosition();
            p.drawEllipse(posToPoint(tip), 1.5, 1.5);
            avgPos += fingers[i].tipPosition();
        }
        avgPos /= (float)fingers.count();
        p.setPen(Qt::yellow);
        p.drawEllipse(posToPoint(avgPos), 2.5, 2.5);
    }
}


void LeapWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    if (mController == NULL) {
        p.fillRect(rect(), QBrush(QColor(30, 20, 20)));
        return;
    }
    p.fillRect(rect(), QBrush(QColor(40, 50, 40)));
    p.setPen(Qt::gray);
    p.drawText(QRect(5,  5, 60, 15), Qt::AlignLeft | Qt::AlignTop, QString("x: %1").arg(mL.palmPosition().x, 0, 'g', 4));
    p.drawText(QRect(5, 20, 60, 15), Qt::AlignLeft | Qt::AlignTop, QString("y: %1").arg(mL.palmPosition().y, 0, 'g', 4));
    p.drawText(QRect(width()-65,  5, 60, 15), Qt::AlignLeft | Qt::AlignTop, QString("x: %1").arg(mR.palmPosition().x, 0, 'g', 4));
    p.drawText(QRect(width()-65, 20, 60, 15), Qt::AlignLeft | Qt::AlignTop, QString("y: %1").arg(mR.palmPosition().y, 0, 'g', 4));
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(Qt::transparent);
    p.setPen(Qt::red);
    p.drawEllipse(posToPoint(mL.palmPosition()), 7, 7);
    p.setPen(Qt::green);
    p.drawEllipse(posToPoint(mR.palmPosition()), 7, 7);
    paintFingers(p, mL.fingers());
    paintFingers(p, mR.fingers());
}


void LeapWidget::resizeEvent(QResizeEvent*)
{
    mHalfWidth = qreal(width()) / 2;
    update();
}


void LeapWidget::closeEvent(QCloseEvent *)
{
    // ...
}


void LeapWidget::startSensor()
{
    mController = new Leap::Controller(mListener);
}


void LeapWidget::stopSensor()
{
    if (mController == NULL)
        return;
    mController->removeListener(mListener);
    delete mController;
    mController = NULL;
}


void LeapWidget::setHands(const Hand &left, const Hand &right)
{
    mL = left;
    mR = right;
    update();
}


void LeapWidget::setMaxOffsetX(int x)
{
    mMaxOffsetX = qreal(x);
    update();
}


void LeapWidget::setMaxOffsetY(int y)
{
    mMaxOffsetY = qreal(y);
    update();
}
