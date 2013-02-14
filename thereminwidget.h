// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#ifndef __THEREMINWIDGET_H_
#define __THEREMINWIDGET_H_

#include <QWidget>
#include <QPaintEvent>
#include <QMouseEvent>
#include "theremin.h"


class ThereminWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ThereminWidget(QWidget* parent = NULL);
    QSize sizeHint(void) const { return QSize(640, 480); }
    QSize minimumSizeHint(void) const { return QSize(320, 240); }

protected:
    void paintEvent(QPaintEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void wheelEvent(QWheelEvent*);

signals:

public slots:

private:
    qreal frequency(int x) const;
    qreal volume(int y) const;
    bool mLogarithmicScale;

    static const qreal maxF;
    static const qreal logMaxF;
    static const qreal minF;
    static const qreal logMinF;
    static const qreal dF;
    static const qreal logdF;

    Theremin mTheremin;

    qreal mVolume;
    qreal mFrequency;
    QPoint mMarker;
};

#endif // __THEREMINWIDGET_H_
