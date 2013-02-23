// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#include <QtCore/QDebug>
#include <QPainter>
#include <QImage>
#include "camwidget.h"

CamWidget::CamWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(640, 480);
    setMaximumSize(1920, 1080);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    startCapture();
}

CamWidget::~CamWidget()
{
    stopCapture();
}


void CamWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    if (mImage.isNull())
        return;
    painter.drawImage(rect(), mImage);
    painter.setBrush(Qt::transparent);
    painter.setPen(Qt::red);
    painter.setRenderHint(QPainter::Antialiasing);
    OpenCV::Fingers& fingers = mOpenCV.fingers();
    for (OpenCV::Fingers::const_iterator i = fingers.constBegin(); i != fingers.constEnd(); ++i)
        painter.drawEllipse(*i, 9, 9);
    painter.setPen(Qt::green);
    painter.drawEllipse(mOpenCV.handCenter(), mOpenCV.handRadius(), mOpenCV.handRadius());
}


void CamWidget::timerEvent(QTimerEvent*)
{
    if (mOpenCV.isCapturing()) {
        mImage = mOpenCV.getImage();
        update();
    }
}

void CamWidget::startCapture()
{
    const int fps = 30;
    mOpenCV.startCapture(640, 480, fps);
    mCameraUpdateTimerId = startTimer(1000/fps);
}


void CamWidget::stopCapture()
{
    mOpenCV.stopCapture();
    killTimer(mCameraUpdateTimerId);
}
