// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#ifndef __OPENCVSCENE_H_
#define __OPENCVSCENE_H_

#include <QThread>
#include <QImage>
#include <QVector>
#include <QPoint>
#include <QPainterPath>
#include <QWaitCondition>
#include <QMutex>
#include <opencv/cv.h>
#include <opencv/highgui.h>

class OpenCV : public QObject
{
    Q_OBJECT

public:
    OpenCV(QObject* parent = NULL);
    ~OpenCV();

    typedef QVector<QPoint> Fingers;

public:
    bool startCapture(int width, int height, int fps, int cam = 0);
    void stopCapture(void);
    bool isCapturing() const { return (mCamera != NULL); }
    bool getImageSize(int& width, int& height) const;
    bool process(void);
    inline const QImage& frame(void) { return mFrame; }

    QPainterPath faces(void) const;

private:
    void convertIplImageToQImage(const IplImage* iplImg, QImage& image);

private:
    CvSize mSize;
    CvCapture* mCamera;
    QImage mFrame;
    IplImage* mImage;
    IplImage* mGrayImage;
    CvSeq* mHands;
    CvHaarClassifierCascade* mCascade;
    CvMemStorage* mStorage;
    qreal mScale;
};

#endif // __OPENCVSCENE_H_
