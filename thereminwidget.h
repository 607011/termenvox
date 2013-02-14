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
    QSize sizeHint(void) const { return QSize(640, 200); }
    QSize minimumSizeHint(void) const { return QSize(320, 100); }

    inline Theremin& theremin(void) { return mTheremin; }

    enum Scaling {
        Linear,
        Logarithmic,
        Quadratic
    };

    void setFrequencyRange(qreal, qreal);
    void setScaling(Scaling);

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
    qreal volumeToHeight(int y) const;

private:
    Scaling mScaling;

    qreal mMinF;
    qreal mMaxF;

    qreal mLogMaxF;
    qreal mSqrtMaxF;
    qreal mLogMinF;
    qreal mSqrtMinF;
    qreal mdF;
    qreal mLogdF;
    qreal mSqrtdF;

    Theremin mTheremin;

    qreal mVolume;
    qreal mFrequency;
    QPoint mMarker;
};

#endif // __THEREMINWIDGET_H_
