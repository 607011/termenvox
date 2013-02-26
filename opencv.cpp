// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

// most of the following code was ported from https://github.com/bengal/opencv-hand-detection/blob/master/hand.c

#include <QtCore/QDebug>
#include <qmath.h>
#include <cstdlib>
#include "OpenCV.h"
#include "util.h"


#define NUM_FINGERS	(5)
#define NUM_DEFECTS	(8)


OpenCV::OpenCV(QObject* parent)
    : QObject(parent)
    , mSize(cvSize(0, 0))
    , mCamera(NULL)
    , mImage(NULL)
    , mSmallImage(NULL)
    , mGrayImage(NULL)
    , mHands(NULL)
    , mCascade(NULL)
    , mStorage(NULL)
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
    mImage = cvQueryFrame(mCamera);
    mSize = cvGetSize(mImage);
    mGrayImage = cvCreateImage(mSize, 8, 1);
    mSmallImage = cvCreateImage(cvSize(mSize.width/2, mSize.height/2), 8, 1);
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
    if (!isCapturing())
        return false;
    const IplImage* cvimage = cvQueryFrame(mCamera);
    width = cvimage->width;
    height = cvimage->height;
    return true;
}


bool OpenCV::process(void)
{
    if (!isCapturing())
        return false;
    // filter & threshold
    mImage = cvQueryFrame(mCamera);
    // cvResize(mImage, mSmallImage);
    cvCvtColor(mImage, mGrayImage, CV_BGR2GRAY);
    cvClearMemStorage(mStorage);
    mHands = cvHaarDetectObjects(mGrayImage, mCascade, mStorage, 1.1, 2, 0, cvSize(30, 30));
    convertIplImageToQImage(mGrayImage, mFrame);
    return true;
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
