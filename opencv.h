// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#ifndef __OPENCVSCENE_H_
#define __OPENCVSCENE_H_

#include <QThread>
#include <QImage>
#include <QVector>
#include <QRectF>
#include <QPoint>
#include <QSize>
#include <opencv/cv.h>
#include <opencv/highgui.h>

// #define DETECT_HANDS
// #define DETECT_FISTS
#define DETECT_PALMS
// #define DETECT_FACES

class OpenCV : public QObject
{
    Q_OBJECT

public:
    OpenCV(QObject* parent = NULL);
    ~OpenCV();

    typedef QVector<QPoint> Fingers;

signals:
    void objectsDetected(void);

public:
    bool startCapture(int width, int height, int fps, int cam = 0);
    void stopCapture(void);
    bool isCapturing() const { return (mCamera != NULL); }
    bool getImageSize(int& width, int& height) const;
    QSize getImageSize(void) const {
        if (mDownsizedImage == NULL)
            return QSize();
        return QSize(mDownsizedImage->width, mDownsizedImage->height);
    }
    bool process(void);
    inline const QImage& frame(void) { return mFrame; }

    QVector<QRectF> detectedObjects(void) const;

private:
    void convertIplImageToQImage(const IplImage* iplImg, QImage& image);
    static const char* HaarClassifierFile;

private:
    CvSize mSize;
    CvCapture* mCamera;
    QImage mFrame;
    IplImage* mImage;
    IplImage* mDownsizedImage;
    IplImage* mGrayImage;
    CvSeq* mObjects;
    CvHaarClassifierCascade* mCascade;
    CvMemStorage* mStorage;
    qreal mScale;
};

#endif // __OPENCVSCENE_H_
