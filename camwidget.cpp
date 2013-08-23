// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#include <QtCore/QDebug>
#include <QtConcurrent/QtConcurrent>
#include <QPointF>
#include <QString>
#include <QPainter>
#include <QImage>
#include <QVector>
#include <QRect>
#include "camwidget.h"

class CamWidgetPrivate
{
public:
    CamWidgetPrivate()
        : timeSampleIndex(0)
        , fps(0.0)
        , cameraUpdateTimerId(0)
        , scale(1.0)
        , isProcessing(false)
    { /* ... */ }
    QVector<int> timeSamples;
    int timeSampleIndex;
    QTime t;
    qreal fps;
    OpenCV opencv;
    int cameraUpdateTimerId;
    QImage image;
    qreal windowAspectRatio;
    qreal frameAspectRatio;
    QRect destRect;
    qreal scale;
    bool isProcessing;
};




CamWidget::CamWidget(QWidget* parent)
    : QWidget(parent)
    , d_ptr(new CamWidgetPrivate)
{
    setMinimumSize(320, 240);
    setMaximumSize(1920, 1080);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}


CamWidget::~CamWidget()
{
    stopCapture();
}


const OpenCV* CamWidget::cv(void) const
{
    return &d_ptr->opencv;
}

void CamWidget::calcDestRect()
{
    Q_D(CamWidget);
    if (d->windowAspectRatio < d->frameAspectRatio) {
        const int h = int(width() / d->frameAspectRatio);
        d->destRect = QRect(0, (height()-h)/2, width(), h);
    }
    else {
        const int w = int(height() * d->frameAspectRatio);
        d->destRect = QRect((width()-w)/2, 0, w, height());
    }
}


void CamWidget::paintEvent(QPaintEvent*)
{
    Q_D(CamWidget);
    static const QColor colors[5] = { QColor(255, 120, 10), QColor(10, 255, 10), QColor(10, 10, 255), QColor(255, 120, 10), QColor(10, 255, 255) };
    d->scale = qMin(qreal(d->destRect.width()) / d->image.width(), qreal(d->destRect.height()) / d->image.height());
    QPainter painter(this);
    painter.fillRect(rect(), QBrush(QColor(50, 50, 50)));
    if (!d->image.isNull()) {
        painter.drawImage(d->destRect, d->image);
        painter.setBrush(Qt::transparent);
        painter.setPen(QColor(90, 90, 90, 150));
        painter.drawRect(d->destRect.x(), d->destRect.y(), d->destRect.width()-1, d->destRect.height()-1);
        painter.save();
        painter.setRenderHint(QPainter::Antialiasing);
        painter.translate(d->destRect.topLeft());
        painter.scale(d->scale, d->scale);
        const QVector<QRectF>& objects = d->opencv.detectedObjects();
        int n = 0;
        for (QVector<QRectF>::const_iterator i = objects.constBegin(); i != objects.constEnd(); ++i) {
            painter.setPen(QPen(colors[n % 5], 1.5));
            painter.drawEllipse(i->center(), 3, 3);
            painter.setPen(QPen(colors[n % 5], 0.5));
            painter.drawRect(*i);
            ++n;
        }
        painter.restore();
        painter.setPen(Qt::black);
        painter.drawText(QRectF(d->destRect.x()+5, d->destRect.y()+5, 50, 20), Qt::AlignRight, QString("%1 fps").arg(d->fps, 0, 'g', 2));
        d->t.restart();
    }
}


void CamWidget::resizeEvent(QResizeEvent* e)
{
    Q_D(CamWidget);
    d->windowAspectRatio = qreal(e->size().width()) / e->size().height();
    calcDestRect();
    update();
}


void CamWidget::timerEvent(QTimerEvent*)
{
    Q_D(CamWidget);
    if (d->opencv.isCapturing() && !d->isProcessing) {
        d->isProcessing = true;
        QFuture<void> imageFuture = QtConcurrent::run(&d->opencv, &OpenCV::process);
        imageFuture.waitForFinished();
        d->image = d->opencv.frame();
        const int elapsed = d->t.elapsed();
        if (elapsed > 0) {
            if (d->timeSampleIndex >= MaxNumTimeSamples)
                d->timeSampleIndex = 0;
            if (d->timeSamples.size() < MaxNumTimeSamples)
                d->timeSamples.push_back(elapsed);
            else
                d->timeSamples[d->timeSampleIndex] = elapsed;
            ++d->timeSampleIndex;
            int elapsedSum = 0;
            for (int i = 0; i < d->timeSamples.size(); ++i)
                elapsedSum += d->timeSamples[i];
            d->fps = 1000.0 / elapsedSum * d->timeSamples.size();
        }
        update();
        d->isProcessing = false;
    }
}


void CamWidget::startCapture()
{
    Q_D(CamWidget);
    static const int fps = 30;
    d->opencv.startCapture(640, 480, fps);
    int width, height;
    while (!d->opencv.getImageSize(width, height))
        /* wait */;
    d->frameAspectRatio = qreal(width) / height;
    calcDestRect();
    d->cameraUpdateTimerId = startTimer(1000 / fps);
}


void CamWidget::stopCapture()
{
    Q_D(CamWidget);
    if (d->cameraUpdateTimerId != 0) {
        killTimer(d->cameraUpdateTimerId);
        d->cameraUpdateTimerId = 0;
        d->opencv.stopCapture();
    }
}
