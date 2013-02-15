// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#include "thereminwidget.h"
#include <QtCore/QDebug>
#include <QLinearGradient>
#include <QPainter>
#include <qmath.h>
#include <qglobal.h>

ThereminWidget::ThereminWidget(QWidget* parent)
    : QWidget(parent)
    , mShowToneScale(true)
    , mShowHzScale(true)
    , mScaling(Logarithmic)
    , mMinF(0.0)
    , mMaxF(4000.0)
    , mVolume(1.0)
    , mFrequency(440.0)
{
    setMouseTracking(true);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    setFrequencyRange(mMinF, mMaxF);

    static const qreal PitchRatio = qPow(2, 1./12); // 1.0594630943592952645618252949463;
    static const QString Chromatic[12] = { "C", "Cis", "D", "Dis/Es", "E", "F", "Fis/Ges", "G", "Gis/As", "A", "Ais/B", "B" };
    int n = 0;
    for (qreal f = 16.5; f < 20000.0; f *= 2) {
        qreal pitch = f;
        for (int i = 0; i < 12; ++i) {
            Pitch p = { pitch, QString("%1%2").arg(Chromatic[i]).arg(n) };
            mPitches.append(p);
            pitch *= PitchRatio;
        }
        ++n;
    }
}


qreal ThereminWidget::volumeToHeight(int y) const
{
    return 1e-2 * y * height();
}


template <typename T>
T square(T x) { return x*x; }


qreal ThereminWidget::frequency(int x) const
{
    qreal f;
    switch (mScaling)
    {
    case Linear:
        f = mMinF + qreal(x) / width() * mdF;
        break;
    case Logarithmic:
        f = exp(mLogMinF + (qreal(x) / width()) * mLogdF);
        break;
    case Quadratic:
        f = square(mSqrtMinF + (qreal(x) / width()) * mSqrtdF);
        break;
    }
    return f;
}


int ThereminWidget::frequencyToWidth(qreal f) const
{
    int x;
    switch (mScaling)
    {
    case Linear:
        x = int(width() / mdF * (f - mMinF));
        break;
    case Logarithmic:
        x = int(width() / mLogdF * (log(f) - mLogMinF));
        break;
    case Quadratic:
        x = int(width() / mSqrtdF * (sqrt(f) - mSqrtMinF));
        break;
    }
    return x;
}


void ThereminWidget::setFrequencyRange(qreal minF, qreal maxF)
{
    mMinF = minF;
    mMaxF = maxF;
    mdF = mMaxF - mMinF;
    mLogMinF = qFuzzyIsNull(mMinF)? 0.0 : qLn(mMinF);
    mLogMaxF = qFuzzyIsNull(mMaxF)? 0.0 : qLn(mMaxF);
    mLogdF = qFuzzyIsNull(mdF)? 0.0 : qLn(mdF);
    mSqrtMinF = qSqrt(mMinF);
    mSqrtMaxF = qSqrt(mMaxF);
    mSqrtdF = qSqrt(mdF);
    update();
}


void ThereminWidget::setScaling(ThereminWidget::Scaling scaling)
{
    mScaling = scaling;
    update();
}


void ThereminWidget::paintEvent(QPaintEvent*)
{
    static const qreal fTicks[] = { 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 200, 300, 400, 500, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 15000, 20000, -1 };
    QPainter p(this);
    // p.setRenderHint(QPainter::Antialiasing);
    QLinearGradient grad(QPointF(0, 0), QPointF(0, height()));
    grad.setColorAt(0.01, QColor(14, 88, 33));
    grad.setColorAt(0.99, QColor(1, 20, 2));
    grad.setSpread(QGradient::PadSpread);
    p.fillRect(rect(), QBrush(grad));
    p.setBrush(Qt::transparent);
    if (mShowToneScale) {
        p.setPen(QColor(190, 190, 190, 128));
        for (QVector<Pitch>::const_iterator i = mPitches.constBegin(); i != mPitches.constEnd(); ++i) {
            if (i->f >= mMinF && i->f <= mMaxF) {
                int x = frequencyToWidth(i->f);
                p.drawLine(x, 0, x, height());
                p.save();
                p.translate(x + 2, 2);
                p.rotate(90);
                p.drawText(0, 0, 40, 15, Qt::AlignLeft, QString("%1").arg(i->name));
                p.restore();
            }
        }}
    if (mShowHzScale) {
        p.setPen(QColor(14, 210, 33, 166));
        qreal f;
        for (int i = 0; (f = fTicks[i]) >= 0 && f <= mMaxF; ++i) {
            int x = frequencyToWidth(f);
            p.drawLine(x, 0, x, height());
            p.drawText(x+2, height()-20, 40, 15, Qt::AlignLeft | Qt::AlignVCenter, QString("%1").arg(f));
        }
    }
    p.setPen(QColor(14, 210, 33, 166));
    for (int i = 0; i < 100; i += 10) {
        int y = volumeToHeight(i);
        p.drawLine(0, y, width(), y);
    }
    p.setPen(Qt::lightGray);
    p.drawText(QPointF(5, 15), tr("%1 Hz").arg(mFrequency, 0, 'g', 5));
    p.drawText(QPointF(5, 30), tr("%1%").arg(mVolume*100, 0, 'g', 3));
    p.setPen(Qt::red);
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
    default:
        break;
    }
}


void ThereminWidget::mouseReleaseEvent(QMouseEvent* e)
{
    switch (e->button()) {
    case Qt::LeftButton:
        mTheremin.stop();
        break;
    default:
        break;
    }
}


void ThereminWidget::mouseMoveEvent(QMouseEvent* e)
{
    mFrequency = frequency(e->x());
    mVolume = qreal(height() - e->y()) / height();
    mTheremin.setFrequency(mFrequency);
    mTheremin.setVolume(mVolume);
    update();
}
