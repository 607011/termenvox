// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#include <QtCore/QDebug>
#include <QtConcurrent/QtConcurrent>
#include <QPointF>
#include <QString>
#include <QPainter>
#include <QImage>
#include "camwidget.h"

CamWidget::CamWidget(QWidget* parent)
    : QWidget(parent)
    , mTimeSampleIndex(0)
    , mFPS(0.0)
    , mScale(1.0)
{
    setMinimumSize(320, 240);
    setMaximumSize(1920, 1080);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}


CamWidget::~CamWidget()
{
    stopCapture();
}


void CamWidget::calcDestRect()
{
    if (mWindowAspectRatio < mFrameAspectRatio) {
        const int h = int(width() / mFrameAspectRatio);
        mDestRect = QRect(0, (height()-h)/2, width(), h);
    }
    else {
        const int w = int(height() * mFrameAspectRatio);
        mDestRect = QRect((width()-w)/2, 0, w, height());
    }
}


void CamWidget::paintEvent(QPaintEvent*)
{
    static const QColor colors[5] = { QColor(255, 120, 10), QColor(10, 255, 10), QColor(10, 10, 255), QColor(255, 120, 10), QColor(10, 255, 255) };
    mScale = qMin(qreal(mDestRect.width()) / mImage.width(), qreal(mDestRect.height()) / mImage.height());
    QPainter painter(this);
    painter.fillRect(rect(), QBrush(QColor(50, 50, 50)));
    if (!mImage.isNull()) {
        painter.drawImage(mDestRect, mImage);
        painter.setBrush(Qt::transparent);
        painter.setPen(QColor(90, 90, 90, 150));
        painter.drawRect(mDestRect.x(), mDestRect.y(), mDestRect.width()-1, mDestRect.height()-1);
        painter.save();
        painter.setRenderHint(QPainter::Antialiasing);
        painter.translate(mDestRect.topLeft());
        painter.scale(mScale, mScale);
        const QVector<QRectF>& objects = mOpenCV.detectedObjects();
        int n = 0;
        for (QVector<QRectF>::const_iterator i = objects.constBegin(); i != objects.constEnd(); ++i) {
            painter.setPen(QPen(colors[n % 5], 1.5));
            painter.drawEllipse(i->center(), 3, 3);
            painter.setPen(QPen(colors[n % 5], 0.5));
            painter.drawRect(*i);
            ++n;
        }
        painter.restore();
        painter.setPen(Qt::black);
        painter.drawText(QRectF(mDestRect.x()+5, mDestRect.y()+5, 50, 20), QString("%1 fps").arg(mFPS, 0, 'g', 2));
        mTime.restart();
    }
}


void CamWidget::resizeEvent(QResizeEvent* e)
{
    mWindowAspectRatio = qreal(e->size().width()) / e->size().height();
    calcDestRect();
    update();
}


void CamWidget::timerEvent(QTimerEvent*)
{
    if (mOpenCV.isCapturing()) {
        QFuture<void> imageFuture = QtConcurrent::run(&mOpenCV, &OpenCV::process);
        imageFuture.waitForFinished();
        mImage = mOpenCV.frame();
        const int elapsed = mTime.elapsed();
        if (elapsed > 0) {
            if (mTimeSampleIndex > MaxNumTimeSamples)
                mTimeSampleIndex = 0;
            if (mTimeSamples.size() < MaxNumTimeSamples)
                mTimeSamples.push_back(elapsed);
            else
                mTimeSamples[mTimeSampleIndex] = elapsed;
            ++mTimeSampleIndex;
            int elapsedSum = 0;
            for (int i = 0; i < mTimeSamples.size(); ++i)
                elapsedSum += mTimeSamples[i];
            mFPS = 1000.0 / elapsedSum * mTimeSamples.size();
        }
        update();
    }
}


void CamWidget::startCapture()
{
    static const int fps = 30;
    mOpenCV.startCapture(640, 480, fps);
    int width, height;
    while (!mOpenCV.getImageSize(width, height))
        /* wait */;
    mFrameAspectRatio = qreal(width) / height;
    calcDestRect();
    mCameraUpdateTimerId = startTimer(1000 / fps);
}


void CamWidget::stopCapture()
{
    killTimer(mCameraUpdateTimerId);
    mOpenCV.stopCapture();
}
