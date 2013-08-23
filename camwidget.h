// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#ifndef __CAMWIDGET_H_
#define __CAMWIDGET_H_

#include "opencv.h"

#include <QWidget>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QTimerEvent>
#include <QTime>
#include <QScopedPointer>

class CamWidgetPrivate;

class CamWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CamWidget(QWidget* parent = NULL);
    ~CamWidget();
    void startCapture(void);
    void stopCapture(void);
    const OpenCV* cv(void) const;

protected:
    void paintEvent(QPaintEvent*);
    void timerEvent(QTimerEvent*);
    void resizeEvent(QResizeEvent*);

signals:

private:
    void calcDestRect();

private:
    QScopedPointer<CamWidgetPrivate> d_ptr;
    Q_DECLARE_PRIVATE(CamWidget)
    Q_DISABLE_COPY(CamWidget)

private:
    static const int MaxNumTimeSamples = 5;
};

#endif // __CAMWIDGET_H_
