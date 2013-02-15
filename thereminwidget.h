// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#ifndef __THEREMINWIDGET_H_
#define __THEREMINWIDGET_H_

#include <QWidget>
#include <QBrush>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QVector>
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

    struct Pitch {
        qreal f;
        QString name;
    };

protected:
    void paintEvent(QPaintEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void wheelEvent(QWheelEvent*);

signals:

public slots:
    void setShowHzScale(bool enabled) { mShowHzScale = enabled; update(); }
    void setShowToneScale(bool enabled) { mShowToneScale = enabled; update(); }
    void setShowLoudnessScale(bool enabled) { mShowLoudnessScale = enabled; update(); }

private:
    qreal frequency(qreal x) const;
    qreal volumeToHeight(int y) const;
    int frequencyToWidth(qreal f) const;

private:
    bool mShowHzScale;
    bool mShowToneScale;
    bool mShowLoudnessScale;
    bool mShowToneInfo;

    Scaling mScaling;

    qreal mMinF;
    qreal mMaxF;
    qreal mdF;
    qreal mLogdF;
    qreal mSqrtdF;

    Theremin mTheremin;

    qreal mVolume;
    qreal mFrequency;

    QVector<Pitch> mPitches;
};

#endif // __THEREMINWIDGET_H_
