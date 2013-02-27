// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

// most of the following code was ported from https://github.com/bengal/opencv-hand-detection/blob/master/hand.c

#include <QtCore/QDebug>
#include <qmath.h>
#include <cstdlib>
#include "OpenCV.h"
#include "util.h"

const char* OpenCV::HaarClassifierFile = "xml/haarcascade_frontalface_alt.xml";
// const char* OpenCV::HaarClassifierFile = "xml/haarcascade_frontalface_alt2.xml";
// const char* OpenCV::HaarClassifierFile = "xml/haarcascade_frontalface_alt_tree.xml";


OpenCV::OpenCV(QObject* parent)
    : QObject(parent)
    , mSize(cvSize(0, 0))
    , mCamera(NULL)
    , mImage(NULL)
    , mDownsizedImage(NULL)
    , mGrayImage(NULL)
    , mHands(NULL)
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
    mFrame = QImage(desiredWidth, desiredHeight, QImage::Format_ARGB32);
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
        ;
    cvResize(mImage, mDownsizedImage);
    cvCvtColor(mDownsizedImage, mGrayImage, CV_BGR2GRAY);
    cvClearMemStorage(mStorage);
    mHands = cvHaarDetectObjects(mGrayImage, mCascade, mStorage, mScale, 2, CV_HAAR_DO_CANNY_PRUNING, cvSize(30, 30));
    convertIplImageToQImage(mGrayImage, mFrame);
    return true;
}


QPainterPath OpenCV::faces(void) const
{
    QPainterPath path;
    const int N = (mHands)? mHands->total : 0;
    for (int i = 0; i < N; ++i) {
        const CvRect* const r = (CvRect*)cvGetSeqElem(mHands, i);
        const int x = mSize.width - r->x - r->width; // mirror along y-axis
        const int y = r->y;
        path.addRect(x, y, r->width, r->height);
        path.addEllipse(QPointF(x + 0.5 * r->width, y + 0.5 * r->height), 1.5, 1.5);
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
