// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.


#include "leapwidget.h"
#include <QtCore/QDebug>

using namespace Leap;

class LeapWidgetPrivate {
public:
    LeapWidgetPrivate(void)
        : halfWidth(0.0)
        , maxOffsetX(100.0)
        , maxOffsetY(100.0)
    { /* ... */ }
    ~LeapWidgetPrivate() {}

    Leap::Controller controller;
    LeapListener listener;
    Leap::Hand leftHand;
    Leap::Hand rightHand;
    qreal halfWidth;
    qreal maxOffsetX;
    qreal maxOffsetY;
};



LeapWidget::LeapWidget(QWidget* parent)
    : QWidget(parent)
    , d_ptr(new LeapWidgetPrivate)
{
    setMinimumSize(320, 240);
    setMaximumSize(1920, 1080);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    qRegisterMetaType<Leap::Hand>();
    QObject::connect(&d_ptr->listener, SIGNAL(handsDetected(Leap::Hand, Leap::Hand)), SLOT(setHands(Leap::Hand, Leap::Hand)));
    QObject::connect(&d_ptr->listener, SIGNAL(handsDetected(Leap::Hand, Leap::Hand)), SIGNAL(handsDetected(Leap::Hand, Leap::Hand)));
}


LeapWidget::~LeapWidget()
{
    stopSensor();
}


QPointF LeapWidget::posToPoint(const Leap::Vector &pos) const
{
    return QPointF(d_ptr->halfWidth * (1 + pos.x / d_ptr->maxOffsetX), height() * (1 - pos.y / d_ptr->maxOffsetY));
}


void LeapWidget::paintFingers(QPainter &p, const FingerList &fingers)
{
    if (!fingers.empty()) {
        Leap::Vector avgPos;
        p.setPen(QPen(QColor(255, 140, 0), 2));
        for (int i = 0; i < fingers.count(); ++i) {
            const Leap::Vector& tip = fingers[i].tipPosition();
            p.drawEllipse(posToPoint(tip), 4, 4);
            avgPos += fingers[i].tipPosition();
        }
        avgPos /= (float)fingers.count();
        p.setPen(QPen(QColor(230, 41, 173), 2));
        p.drawEllipse(posToPoint(avgPos), 6, 6);
    }
}

void LeapWidget::paintEvent(QPaintEvent*)
{
    Q_D(LeapWidget);
    QPainter p(this);
    if (!d->controller.isConnected()) {
        p.fillRect(rect(), QBrush(QColor(30, 20, 20)));
        return;
    }
    p.fillRect(rect(), QBrush(QColor(230, 230, 230)));
    p.setPen(Qt::black);
    p.drawText(QRect(5,  5, 60, 15), Qt::AlignLeft | Qt::AlignTop, QString("x: %1").arg(d->leftHand.palmPosition().x, 0, 'g', 4));
    p.drawText(QRect(5, 20, 60, 15), Qt::AlignLeft | Qt::AlignTop, QString("y: %1").arg(d->leftHand.palmPosition().y, 0, 'g', 4));
    p.drawText(QRect(width()-65,  5, 60, 15), Qt::AlignLeft | Qt::AlignTop, QString("x: %1").arg(d->rightHand.palmPosition().x, 0, 'g', 4));
    p.drawText(QRect(width()-65, 20, 60, 15), Qt::AlignLeft | Qt::AlignTop, QString("y: %1").arg(d->rightHand.palmPosition().y, 0, 'g', 4));
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(Qt::transparent);
    p.setPen(QPen(QColor(204, 29, 52), 4));
    p.drawEllipse(posToPoint(d->leftHand.palmPosition()), 13, 13);
    p.setPen(QPen(QColor(29, 204, 43), 4));
    p.drawEllipse(posToPoint(d->rightHand.palmPosition()), 13, 13);
    paintFingers(p, d->leftHand.fingers());
    paintFingers(p, d->rightHand.fingers());
}


void LeapWidget::resizeEvent(QResizeEvent*)
{
    Q_D(LeapWidget);
    d->halfWidth = qreal(width()) / 2;
    update();
}


void LeapWidget::closeEvent(QCloseEvent *)
{
    // ...
}


void LeapWidget::startSensor()
{
    Q_D(LeapWidget);
    d->controller.addListener(d->listener);
}


void LeapWidget::stopSensor()
{
    Q_D(LeapWidget);
    d->controller.removeListener(d->listener);
}


void LeapWidget::setHands(const Hand &left, const Hand &right)
{
    Q_D(LeapWidget);
    d->leftHand = left;
    d->rightHand = right;
    update();
}


void LeapWidget::setMaxOffsetX(int x)
{
    Q_D(LeapWidget);
    d->maxOffsetX = qreal(x);
    update();
}


void LeapWidget::setMaxOffsetY(int y)
{
    Q_D(LeapWidget);
    d->maxOffsetY = qreal(y);
    update();
}
