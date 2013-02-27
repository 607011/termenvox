// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#include <QtCore/QDebug>
#include <qmath.h>
#include <cstdlib>
#include "opencv.h"
#include "util.h"

#if defined(DETECT_HANDS)
const char* OpenCV::HaarClassifierFile = "xml/haarcascade-hand-1.xml";
#elif defined(DETECT_PALMS)
const char* OpenCV::HaarClassifierFile = "xml/palm.xml";
#elif defined(DETECT_FISTS)
const char* OpenCV::HaarClassifierFile = "xml/fist.xml";
#elif defined(DETECT_FACES)
const char* OpenCV::HaarClassifierFile = "xml/haarcascade_frontalface_alt.xml";
#endif


OpenCV::OpenCV(QObject* parent)
    : QObject(parent)
    , mSize(cvSize(0, 0))
    , mCamera(NULL)
    , mImage(NULL)
    , mDownsizedImage(NULL)
    , mGrayImage(NULL)
    , mObjects(NULL)
    , mCascade(NULL)
    , mStorage(NULL)
    , mScale(1.1)
{
    mCascade = (CvHaarClassifierCascade*)cvLoad(HaarClassifierFile, 0, 0, 0);
}


OpenCV::~OpenCV()
{
    stopCapture();
    dealloc(mCascade, cvReleaseHaarClassifierCascade);
}


bool OpenCV::startCapture(int desiredWidth, int desiredHeight, int fps, int cam)
{
    stopCapture();
    mCamera = cvCreateCameraCapture(cam);
    while (!isCapturing())
        /* wait */;
    cvSetCaptureProperty(mCamera, CV_CAP_PROP_FRAME_WIDTH, desiredWidth);
    cvSetCaptureProperty(mCamera, CV_CAP_PROP_FRAME_HEIGHT, desiredHeight);
    cvSetCaptureProperty(mCamera, CV_CAP_PROP_FPS, fps);
    if (getImageSize(desiredWidth, desiredHeight) == false)
        return false;
    mFrame = QImage(desiredWidth, desiredHeight, QImage::Format_ARGB32_Premultiplied);
    mImage = cvQueryFrame(mCamera);
    mSize = cvSize(mImage->width/2, mImage->height/2);
    mDownsizedImage = cvCreateImage(mSize, 8, 3);
    mGrayImage = cvCreateImage(mSize, 8, 1);
    mStorage = cvCreateMemStorage();
    return true;
}


void OpenCV::stopCapture(void)
{
    dealloc(mCamera, cvReleaseCapture);
    dealloc(mDownsizedImage, cvReleaseImage);
    dealloc(mGrayImage, cvReleaseImage);
    dealloc(mStorage, cvReleaseMemStorage);
}


bool OpenCV::getImageSize(int& width, int& height) const
{
    if (!isCapturing() || mCascade == NULL)
        return false;
    const IplImage* cvimage = cvQueryFrame(mCamera);
    width = cvimage->width/2;
    height = cvimage->height/2;
    return true;
}


bool OpenCV::process(void)
{
    if (!isCapturing() || mCascade == NULL)
        return false;
    while ((mImage = cvQueryFrame(mCamera)) == NULL)
        /* wait */;
    cvResize(mImage, mDownsizedImage);
    cvCvtColor(mDownsizedImage, mGrayImage, CV_BGR2GRAY);
    cvClearMemStorage(mStorage);
#if defined(DETECT_HANDS)
    mObjects = cvHaarDetectObjects(mGrayImage, mCascade, mStorage, mScale, 2, CV_HAAR_DO_CANNY_PRUNING, cvSize(100, 100));
#elif defined(DETECT_PALMS)
    mObjects = cvHaarDetectObjects(mGrayImage, mCascade, mStorage, mScale, 2, CV_HAAR_DO_CANNY_PRUNING, cvSize(20, 20));
#elif defined(DETECT_FISTS)
    mObjects = cvHaarDetectObjects(mGrayImage, mCascade, mStorage, mScale, 2, CV_HAAR_DO_CANNY_PRUNING, cvSize(24, 24));
#elif defined(DETECT_FACES)
    mObjects = cvHaarDetectObjects(mGrayImage, mCascade, mStorage, mScale, 2, CV_HAAR_DO_CANNY_PRUNING, cvSize(30, 30));
#endif
    convertIplImageToQImage(mGrayImage, mFrame);
    if (mObjects != NULL && mObjects->total > 0)
        emit objectsDetected();
    return true;
}


QVector<QRectF> OpenCV::detectedObjects(void) const
{
    QVector<QRectF> objects(mObjects->total);
    const int N = (mObjects)? mObjects->total : 0;
    for (int i = 0; i < N; ++i) {
        const CvRect* const r = (CvRect*)cvGetSeqElem(mObjects, i);
        const int x = mSize.width - r->x - r->width; // mirror along y-axis
        const int y = r->y;
        objects[i] = QRectF(x, y, r->width, r->height);
    }
    return objects;
}


void OpenCV::convertIplImageToQImage(const IplImage* iplImg, QImage& image)
{
    const int w = iplImg->width;
    int y = iplImg->height;
    char* src = iplImg->imageData +  iplImg->widthStep * y;
    switch (iplImg->nChannels) {
    case 1:
    {
        while (y--) {
            QRgb* dst = (uint*)image.scanLine(y);
            const QRgb* const dstEnd = reinterpret_cast<QRgb*>(image.scanLine(y)) + w;
            while (dst < dstEnd) {
                *dst++ = qRgb(*src, *src, *src);
                --src;
            }
        }
        break;
    }
    case 3:
    {
        while (y--) {
            QRgb* dst = (uint*)image.scanLine(y);
            const QRgb* const dstEnd = reinterpret_cast<QRgb*>(image.scanLine(y)) + w;
            while (dst < dstEnd) {
                *dst++ = qRgb(*(src-1), *(src-2), *src);
                src -= 3;
            }
        }
        break;
    }
    default:
        break;
    }
}
