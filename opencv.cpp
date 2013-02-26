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
    , mCamera(NULL)
    , mImage(NULL)
    , mThresholdImage(NULL)
    , mTempImage1(NULL)
    , mTempImage3(NULL)
    , mHull(NULL)
    , mContour(NULL)
    , mFingers(NULL)
    , mDefects(NULL)
    , mHullStorage(NULL)
    , mContourStorage(NULL)
    , mTempStorage(NULL)
    , mDefectsStorage(NULL)
    , mKernel(NULL)
    , mNumFingers(0)
    , mAbort(false)
{
    // ...
}


OpenCV::~OpenCV()
{
    stopCapture();
}


void OpenCV::run(void)
{
    mCond.wait(&mMutex);
    while (!mAbort) {

    }
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
    mThresholdImage = cvCreateImage(cvGetSize(mImage), 8, 1);
    mTempImage1 = cvCreateImage(cvGetSize(mImage), 8, 1);
    mTempImage3 = cvCreateImage(cvGetSize(mImage), 8, 3);
    mKernel = cvCreateStructuringElementEx(9, 9, 4, 4, CV_SHAPE_RECT, NULL);
    mContourStorage = cvCreateMemStorage(0);
    mHullStorage = cvCreateMemStorage(0);
    mDefectsStorage = cvCreateMemStorage(0);
    mTempStorage = cvCreateMemStorage(0);
    mFingers = (CvPoint*)calloc(NUM_FINGERS + 1, sizeof(CvPoint));
    mDefects = (CvPoint*)calloc(NUM_DEFECTS, sizeof(CvPoint));
    return true;
}


void OpenCV::stopCapture(void)
{
    dealloc(mCamera, cvReleaseCapture);
    dealloc(mThresholdImage, cvReleaseImage);
    dealloc(mTempImage1, cvReleaseImage);
    dealloc(mTempImage3, cvReleaseImage);
    dealloc(mContourStorage, cvReleaseMemStorage);
    dealloc(mHullStorage, cvReleaseMemStorage);
    dealloc(mDefectsStorage, cvReleaseMemStorage);
    dealloc(mTempStorage, cvReleaseMemStorage);
    dealloc(mKernel, cvReleaseStructuringElement);
    dealloc(mFingers);
    dealloc(mDefects);
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
#if 0
    cvSmooth(mImage, mTempImage3, CV_GAUSSIAN, 11, 11, 0, 0);
    cvSmooth(mTempImage3, mTempImage3, CV_MEDIAN, 11, 11, 0, 0);
    cvCvtColor(mTempImage3, mTempImage3, CV_BGR2HSV);
    cvInRangeS(mTempImage3, cvScalar(0, 0, 160, 0), cvScalar(255, 400, 300, 255), mThresholdImage);
    cvMorphologyEx(mThresholdImage, mThresholdImage, NULL, mKernel, CV_MOP_OPEN, 1);
    cvSmooth(mThresholdImage, mThresholdImage, CV_GAUSSIAN, 3, 3, 0, 0);

    // find contour
    cvCopy(mThresholdImage, mTempImage1, NULL);
    CvSeq *contours;
    cvFindContours(mTempImage1, mTempStorage, &contours, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));
    float max_area = 0.0f;
    CvSeq* contour = NULL;
    for (CvSeq* tmp = contours; tmp; tmp = tmp->h_next) {
        float area = fabs(cvContourArea(tmp, CV_WHOLE_SEQ, 0));
        if (area > max_area) {
            max_area = area;
            contour = tmp;
        }
    }
    if (contour) {
        contour = cvApproxPoly(contour, sizeof(CvContour), mContourStorage, CV_POLY_APPROX_DP, 2, 1);
        mContour = contour;
    }

    // find convex hull
    mHull = NULL;
    if (!mContour)
        return false;
    mHull = cvConvexHull2(mContour, mHullStorage, CV_CLOCKWISE, 0);
    if (mHull) {
        /* Get convexity defects of contour w.r.t. the convex hull */
        CvSeq* defects = cvConvexityDefects(mContour, mHull, mDefectsStorage);
        if (defects && defects->total) {
            float dist = 0;
            int x = 0, y = 0;
            CvConvexityDefect* defectArray = (CvConvexityDefect*)calloc(defects->total, sizeof(CvConvexityDefect));
            cvCvtSeqToArray(defects, defectArray, CV_WHOLE_SEQ);
            // Average depth points to get hand center
            for (int i = 0; i < defects->total && i < NUM_DEFECTS; i++) {
                x += defectArray[i].depth_point->x;
                y += defectArray[i].depth_point->y;
                mDefects[i] = cvPoint(defectArray[i].depth_point->x, defectArray[i].depth_point->y);
            }
            x /= defects->total;
            y /= defects->total;
            mNumDefects = defects->total;
            mHandCenter = cvPoint(x, y);
            // Compute hand radius as mean of distances of defects' depth point to hand center
            for (int i = 0; i < defects->total; i++) {
                const int d = (x - defectArray[i].depth_point->x) *
                              (x - defectArray[i].depth_point->x) +
                              (y - defectArray[i].depth_point->y) *
                              (y - defectArray[i].depth_point->y);
                dist += qSqrt((float)d);
            }
            mHandRadius = int(dist / defects->total);
            free(defectArray);
        }
    }

    // find fingers
    CvPoint max_point;
    int dist1 = 0, dist2 = 0;
    int finger_distance[NUM_FINGERS + 1];
    mNumFingers = 0;
    if (!mContour || !mHull)
        return false;

    int n = mContour->total;
    CvPoint* points = (CvPoint*)calloc(n, sizeof(CvPoint));
    cvCvtSeqToArray(mContour, points, CV_WHOLE_SEQ);

    // Fingers are detected as points where the distance to the center is a local maximum
    for (int i = 0; i < n; ++i) {
        const int cx = mHandCenter.x;
        const int cy = mHandCenter.y;
        const int dist = (cx - points[i].x) * (cx - points[i].x) + (cy - points[i].y) * (cy - points[i].y);
        if (dist < dist1 && dist1 > dist2 && max_point.x != 0 && max_point.y < cvGetSize(mImage).height - 10) {
            finger_distance[mNumFingers] = dist;
            mFingers[mNumFingers++] = max_point;
            if (mNumFingers >= NUM_FINGERS + 1)
                break;
        }
        dist2 = dist1;
        dist1 = dist;
        max_point = points[i];
    }
    free(points);

#endif
    convertIplImageToQImage(mImage, mFrame);
    return true;
}


void OpenCV::convertIplImageToQImage(const IplImage* iplImg, QImage& image)
{
    const int w = iplImg->width;
    char* src = iplImg->imageData +  iplImg->widthStep * y;
    switch (iplImg->nChannels) {
    case 1:
    {
        int y = iplImg->height;
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
        int y = iplImg->height;
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
