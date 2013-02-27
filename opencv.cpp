// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

// most of the following code was ported from https://github.com/bengal/opencv-hand-detection/blob/master/hand.c

#include <QtCore/QDebug>
#include <qmath.h>
#include <cstdlib>
#include "OpenCV.h"
#include "util.h"


OpenCV::OpenCV(QObject* parent)
    : QObject(parent)
    , mSize(cvSize(0, 0))
    , mCamera(NULL)
    , mImage(NULL)
    , mGrayImage(NULL)
    , mHands(NULL)
    , mCascade(NULL)
    , mStorage(NULL)
    , mScale(1.1)
{
    mCascade = (CvHaarClassifierCascade*)cvLoad("xml/haarcascade_frontalface_alt.xml", 0, 0, 0);
}


OpenCV::~OpenCV()
{
    stopCapture();
}


bool OpenCV::startCapture(int desiredWidth, int desiredHeight, int fps, int cam)
{
    stopCapture();
    mCamera = cvCreateCameraCapture(cam);
    while (!isCapturing())
        /* wait ... */;
    cvSetCaptureProperty(mCamera, CV_CAP_PROP_FRAME_WIDTH, desiredWidth);
    cvSetCaptureProperty(mCamera, CV_CAP_PROP_FRAME_HEIGHT, desiredHeight);
    cvSetCaptureProperty(mCamera, CV_CAP_PROP_FPS, fps);
    if (getImageSize(desiredWidth, desiredHeight) == false)
        return false;
    mFrame = QImage(desiredWidth, desiredHeight, QImage::Format_ARGB32);
    while ((mImage = cvQueryFrame(mCamera)) == NULL)
        ;
    mSize = cvGetSize(mImage);
    mGrayImage = cvCreateImage(mSize, 8, 1);
    mStorage = cvCreateMemStorage();
    return true;
}


void OpenCV::stopCapture(void)
{
    dealloc(mCamera, cvReleaseCapture);
    dealloc(mGrayImage, cvReleaseImage);
    dealloc(mStorage, cvReleaseMemStorage);
}


bool OpenCV::getImageSize(int& width, int& height) const
{
    if (!isCapturing() || mCascade == NULL)
        return false;
    const IplImage* cvimage = cvQueryFrame(mCamera);
    width = cvimage->width;
    height = cvimage->height;
    return true;
}


bool OpenCV::process(void)
{
    if (!isCapturing() || mCascade == NULL)
        return false;
    mImage = cvQueryFrame(mCamera);
    cvCvtColor(mImage, mGrayImage, CV_BGR2GRAY);
    cvClearMemStorage(mStorage);
    mHands = cvHaarDetectObjects(mGrayImage, mCascade, mStorage, mScale, 2, CV_HAAR_SCALE_IMAGE, cvSize(30, 30));
    convertIplImageToQImage(mImage, mFrame);
    return true;
}


QPainterPath OpenCV::hands(void) const
{
    QPainterPath path;
    const int N = (mHands)? mHands->total : 0;
    for (int i = 0; i < N; ++i) {
        const CvRect* const r = (CvRect*)cvGetSeqElem(mHands, i);
        path.addRect(mSize.width - r->x - r->width, r->y, r->width, r->height);
    }
    return path;
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
