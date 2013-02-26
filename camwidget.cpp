// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#include <QtCore/QDebug>
#include <QtConcurrent/QtConcurrent>
#include <QPainter>
#include <QImage>
#include "camwidget.h"

CamWidget::CamWidget(QWidget* parent)
    : QWidget(parent)
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
    QPainter painter(this);
    if (mImage.isNull()) {
        painter.fillRect(rect(), QBrush(QColor(50, 50, 50)));
    }
    else {
        painter.drawImage(mDestRect, mImage);
        painter.setBrush(Qt::transparent);
        painter.setPen(Qt::red);
        painter.setRenderHint(QPainter::Antialiasing);
        const OpenCV::Fingers& fingers = mOpenCV.fingers();
        for (OpenCV::Fingers::const_iterator i = fingers.constBegin(); i != fingers.constEnd(); ++i)
            painter.drawEllipse(*i, 5, 5);
        painter.setPen(Qt::green);
        painter.drawEllipse(mOpenCV.handCenter(), mOpenCV.handRadius(), mOpenCV.handRadius());
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
        update();
    }
}


void CamWidget::startCapture()
{
    const int fps = 30;
    mOpenCV.startCapture(640, 480, fps);
    int width, height;
    while (!mOpenCV.getImageSize(width, height))
        /* loop */;
    mFrameAspectRatio = qreal(width) / height;
    calcDestRect();
    mCameraUpdateTimerId = startTimer(1000/fps);
}


void CamWidget::stopCapture()
{
    mOpenCV.stopCapture();
    killTimer(mCameraUpdateTimerId);
}
