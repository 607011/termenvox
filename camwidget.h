// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#ifndef __CAMWIDGET_H_
#define __CAMWIDGET_H_

#include "opencv.h"

#include <QWidget>
#include <QVector>
#include <QImage>
#include <QRect>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QTimerEvent>
#include <QTime>

class CamWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CamWidget(QWidget* parent = NULL);
    ~CamWidget();
    void startCapture(void);
    void stopCapture(void);
    const OpenCV* cv(void) const { return &mOpenCV; }

protected:
    void paintEvent(QPaintEvent*);
    void timerEvent(QTimerEvent*);
    void resizeEvent(QResizeEvent*);

signals:

private:
    void calcDestRect();

private:
    static const int MaxNumTimeSamples = 5;
    QVector<int> mTimeSamples;
    int mTimeSampleIndex;
    QTime mTime;
    qreal mFPS;
    OpenCV mOpenCV;
    int mCameraUpdateTimerId;
    QImage mImage;
    qreal mWindowAspectRatio;
    qreal mFrameAspectRatio;
    QRect mDestRect;
    qreal mScale;
};

#endif // __CAMWIDGET_H_
