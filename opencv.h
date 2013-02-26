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

    QPainterPath hands(void) const {
        QPainterPath path;
        const int N = mHands? mHands->total : 0;
        for( int i = 0; i < N; ++i) {
            const CvRect* const r = (CvRect*)cvGetSeqElem(mHands, i);
            path.addRect(mSize.width - r->x - r->width, r->y, r->width, r->height);
        }
        return path;
    }

private:
    void convertIplImageToQImage(const IplImage* iplImg, QImage& image);

private:
    CvSize mSize;
    CvCapture* mCamera;
    QImage mFrame;
    IplImage* mImage;
    IplImage* mSmallImage;
    IplImage* mGrayImage;
    CvSeq* mHands;
    CvHaarClassifierCascade* mCascade;
    CvMemStorage* mStorage;
    QWaitCondition mCond;
    QMutex mMutex;
};

#endif // __OPENCVSCENE_H_
