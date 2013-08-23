// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#ifndef __LEAPWIDGET_H_
#define __LEAPWIDGET_H_

#include <QWidget>
#include <QPainter>
#include <QPointF>
#include <QScopedPointer>
#include "leaplistener.h"

class LeapWidgetPrivate;

class LeapWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LeapWidget(QWidget* parent = NULL);
    ~LeapWidget();

protected:
    void paintEvent(QPaintEvent*);
    void resizeEvent(QResizeEvent*);
    void closeEvent(QCloseEvent*);

signals:
    void handsDetected(const Leap::Hand& left, const Leap::Hand& right);

public slots:
    void startSensor(void);
    void stopSensor(void);
    void setHands(const Leap::Hand& left, const Leap::Hand& right);
    void setMaxOffsetX(int);
    void setMaxOffsetY(int);

private:
    QScopedPointer<LeapWidgetPrivate> d_ptr;
    Q_DECLARE_PRIVATE(LeapWidget)
    Q_DISABLE_COPY(LeapWidget)

private:
    QPointF posToPoint(const Leap::Vector&) const;
    void paintFingers(QPainter&, const Leap::FingerList&);
};

#endif // __LEAPWIDGET_H_
