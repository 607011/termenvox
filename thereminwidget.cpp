// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#include "thereminwidget.h"
#include <QtCore/QDebug>
#include <QLinearGradient>
#include <QMessageBox>
#include <QPainter>
#include <qmath.h>
#include <qglobal.h>

ThereminWidget::ThereminWidget(QWidget* parent)
    : QWidget(parent)
    , mShowToneScale(true)
    , mShowHzScale(true)
    , mShowLoudnessScale(true)
    , mShowToneInfo(true)
    , mHold(false)
    , mScaling(Logarithmic)
    , mMinF(0.0)
    , mMaxF(4000.0)
    , mVolume(1.0)
    , mFrequency(443.0)
{
    setMouseTracking(true);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    setFrequencyRange(mMinF, mMaxF);

    if (mTheremin.lastErrorType() != RtError::UNSPECIFIED)
        QMessageBox::critical(this, tr("STK Error"), tr("Error while initializing Theremin: %1. Can't play any sound.").arg(mTheremin.lastErrorMessage()));

    static const qreal PitchRatio = 1.0594630943592952645618252949463417007792043174941856; // qPow(2, 1./12);
    static const QString MusicalScale[12] = { "C", "Cis", "D", "Dis/Es", "E", "F", "Fis/Ges", "G", "Gis/As", "A", "Ais/B", "B" };
    int n = 0;
    for (qreal f = 16.5; f < 20000.0; f *= 2) {
        qreal pitch = f;
        for (int i = 0; i < 12; ++i) {
            ThereminWidget::Pitch p = { pitch, QString("%1%2").arg(MusicalScale[i]).arg(n) };
            mPitches.append(p);
            pitch *= PitchRatio;
        }
        ++n;
    }
}


void ThereminWidget::setScaling(ThereminWidget::Scaling scaling)
{
    mScaling = scaling;
    update();
}


void ThereminWidget::setFrequencyRange(qreal minF, qreal maxF)
{
    mMinF = minF;
    mMaxF = maxF;
    mdF = mMaxF - mMinF;
    mLogdF = qFuzzyIsNull(mdF)? 1.0 : qLn(mdF);
    update();
}


template <typename T>
T sqr(T x) { return x*x; }


qreal ThereminWidget::widthToFrequency(int x) const
{
    qreal f;
    switch (mScaling)
    {
    case Linear:
        f = mMinF + x * mdF / width();
        break;
    case Logarithmic:
        f = mMinF + exp(x * mLogdF / width());
        break;
    case Quadratic:
        f = mMinF + sqr(qreal(x) / width()) * mdF;
        break;
    case Cubic:
        f = mMinF + qPow(qreal(x) / width(), 3) * mdF;
        break;
    case Biquadratic:
        f = mMinF + qPow(qreal(x) / width(), 4) * mdF;
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
        x = int(width() * (f - mMinF) / mdF);
        break;
    case Logarithmic:
        x = int(width() * log(f - mMinF) / mLogdF);
        break;
    case Quadratic:
        x = int(width() * sqrt((f - mMinF) / mdF));
        break;
    case Cubic:
        x = int(width() * qPow((f - mMinF) / mdF, 1./3));
        break;
    case Biquadratic:
        x = int(width() * qPow((f - mMinF) / mdF, 1./4));
        break;
    }
    return x;
}


int ThereminWidget::volumeToHeight(qreal volume) const
{
    return height() - int(volume * height());
}


qreal ThereminWidget::heightToVolume(int h) const
{
    return qreal(height() - h) / height();
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
                const int x = frequencyToWidth(i->f);
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
            const int x = frequencyToWidth(f);
            p.drawLine(x, 0, x, height());
            p.drawText(x+2, height()-20, 40, 15, Qt::AlignLeft | Qt::AlignVCenter, QString("%1").arg(f));
        }
    }
    if (mShowLoudnessScale) {
        p.setPen(QColor(14, 210, 33, 166));
        for (qreal i = 0; i < 1; i += 0.1) {
            const int y = volumeToHeight(i);
            p.drawLine(0, y, width(), y);
        }
    }
    if (mShowToneInfo) {
        p.setPen(Qt::lightGray);
        p.drawText(QPointF(5, 15), tr("%1 Hz").arg(mFrequency, 0, 'g', 5));
        p.drawText(QPointF(5, 30), tr("%1%").arg(mVolume*100, 0, 'g', 3));
    }
    if (mHold) {
        p.setPen(Qt::red);
        p.drawEllipse(QPointF(frequencyToWidth(mFrequency), volumeToHeight(mVolume)), 3, 3);
    }
}


void ThereminWidget::wheelEvent(QWheelEvent* e)
{
    // ...
}


void ThereminWidget::mousePressEvent(QMouseEvent* e)
{
    switch (e->button()) {
    case Qt::LeftButton:
        mTheremin.play();
        mHold = false;
        update();
        break;
    case Qt::RightButton:
        mHold = true;
        mFrequency = widthToFrequency(e->x());
        mVolume = heightToVolume(e->y());
        mTheremin.setFrequency(mFrequency);
        mTheremin.setVolume(mVolume);
        mTheremin.play();
        update();
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
    if (!mHold) {
        mFrequency = widthToFrequency(e->x());
        mVolume = qreal(height() - e->y()) / height();
        mTheremin.setFrequency(mFrequency);
        mTheremin.setVolume(mVolume);
        update();
    }
}
