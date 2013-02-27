// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#include <QtCore/QDebug>
#include <QtConcurrent/QtConcurrent>
#include <QRectF>
#include <QString>
#include <QPainter>
#include <QImage>
#include "camwidget.h"

CamWidget::CamWidget(QWidget* parent)
    : QWidget(parent)
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
    mScale = qMin(qreal(mDestRect.width()) / mImage.width(), qreal(mDestRect.height()) / mImage.height());
    QPainter painter(this);
    if (mImage.isNull()) {
        painter.fillRect(rect(), QBrush(QColor(50, 50, 50)));
    }
    else {
        painter.drawImage(mDestRect, mImage);
        painter.setBrush(Qt::transparent);
        painter.setPen(QColor(90, 90, 90, 150));
        painter.drawRect(mDestRect.x(), mDestRect.y(), mDestRect.width()-1, mDestRect.height()-1);
        painter.setPen(Qt::black);
        painter.drawText(QRectF(5, 5, 50, 20), QString("%1 fps").arg(mFPS, 0, 'g', 2));
        mTime.restart();
        painter.setPen(Qt::red);
        painter.scale(mScale, mScale);
        painter.translate(mDestRect.topLeft());
        painter.drawPath(mOpenCV.faces());
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
        if (elapsed > 0)
            mFPS = 1000.0 / elapsed;
        update();
    }
}


void CamWidget::startCapture()
{
    const int fps = 30;
    mOpenCV.startCapture(640, 480, fps);
    int width, height;
    while (!mOpenCV.getImageSize(width, height))
        /* wait */;
    mFrameAspectRatio = qreal(width) / height;
    calcDestRect();
    mCameraUpdateTimerId = startTimer(1000/fps);
}


void CamWidget::stopCapture()
{
    killTimer(mCameraUpdateTimerId);
    mOpenCV.stopCapture();
}
