// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#ifndef __CAMWIDGET_H_
#define __CAMWIDGET_H_

#include <QWidget>
#include <QImage>
#include <QRect>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QTimerEvent>

#include "OpenCV.h"

class CamWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CamWidget(QWidget* parent = NULL);
    ~CamWidget();
    void startCapture(void);
    void stopCapture(void);

protected:
    void paintEvent(QPaintEvent*);
    void timerEvent(QTimerEvent*);
    void resizeEvent(QResizeEvent*);

signals:

private:
    void calcDestRect();

private:
    OpenCV mOpenCV;
    int mCameraUpdateTimerId;
    QImage mImage;
    qreal mWindowAspectRatio;
    qreal mFrameAspectRatio;
    QRect mDestRect;
    qreal mScale;
};

#endif // __CAMWIDGET_H_
