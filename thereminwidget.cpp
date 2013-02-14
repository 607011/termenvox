// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#include "thereminwidget.h"
#include <QtCore/QDebug>
#include <QLinearGradient>
#include <QPainter>
#include <qmath.h>

ThereminWidget::ThereminWidget(QWidget* parent)
    : QWidget(parent)
    , mLogarithmicScale(true)
    , mVolume(1.0)
    , mFrequency(440.0)
    , mMarker(QPoint(0, 0))
{
    setMouseTracking(true);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}


const qreal ThereminWidget::maxF = 4000;
const qreal ThereminWidget::logMaxF = log(maxF);
const qreal ThereminWidget::minF = 20;
const qreal ThereminWidget::logMinF = log(minF);
const qreal ThereminWidget::dF = maxF - minF;
const qreal ThereminWidget::logdF = logMaxF - logMinF;


qreal ThereminWidget::volumeToHeight(int y) const
{
   return 1e-2 * y * height();
}


qreal ThereminWidget::frequency(int x) const
{
    return mLogarithmicScale
            ? exp(logMinF + (qreal(x) / width()) * logdF)
            : minF + qreal(x) / width() * dF;
}


void ThereminWidget::paintEvent(QPaintEvent*)
{
    static const qreal fTicks[] = { 20, 30, 40, 50, 60, 70, 80, 90, 100, 200, 300, 400, 500, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 15000, 20000, -1 };
    QPainter p(this);
    QLinearGradient grad(QPointF(0, 0), QPointF(0, height()));
    grad.setColorAt(0.01, QColor(14, 88, 33));
    grad.setColorAt(0.99, QColor(1, 20, 2));
    grad.setSpread(QGradient::PadSpread);
    p.fillRect(rect(), QBrush(grad));
    p.setBrush(Qt::transparent);
    p.setPen(QColor(14, 210, 33, 166));
    int i = 0;
    qreal f;
    while ((f = fTicks[i++]) >= minF && f <= maxF) {
        int x = (mLogarithmicScale)
                ? int(width() / logdF * (log(f) - logMinF))
                : int(width() / dF * (f - minF));
        p.drawLine(x, 0, x, height());
        p.drawText(x+2, height()-20, 40, 15, Qt::AlignLeft| Qt::AlignVCenter, QString("%1").arg(f));
    }
    for (i = 0; i < 100; i += 10) {
        int y = volumeToHeight(i);
        p.drawLine(0, y, width(), y);
    }
    p.setPen(Qt::lightGray);
    p.drawText(QPointF(5, 15), tr("%1 Hz").arg(mFrequency, 0, 'g', 5));
    p.drawText(QPointF(5, 30), tr("%1%").arg(mVolume*100, 0, 'g', 3));
    p.setPen(Qt::red);
    if (!mMarker.isNull())
        p.drawEllipse(mMarker, 3, 3);
}


void ThereminWidget::wheelEvent(QWheelEvent* e)
{
    update();
}


void ThereminWidget::mousePressEvent(QMouseEvent* e)
{
    switch (e->button()) {
    case Qt::LeftButton:
        mTheremin.play();
        break;
    case Qt::RightButton:
        break;
    default:
        break;
    }
    update();
}


void ThereminWidget::mouseReleaseEvent(QMouseEvent* e)
{
    switch (e->button()) {
    case Qt::LeftButton:
        mTheremin.stop();
        break;
    case Qt::RightButton:
        break;
    default:
        break;
    }
    update();
}


void ThereminWidget::mouseMoveEvent(QMouseEvent* e)
{
    mFrequency = frequency(e->x());
    mTheremin.setFrequency(mFrequency);
    mVolume = qreal(height() - e->y()) / height();
    mTheremin.setVolume(mVolume);
    update();
}
