// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

// most of the following code was ported from https://github.com/bengal/opencv-hand-detection/blob/master/hand.c

#include <cstdlib>
#include "OpenCV.h"
#include <QtCore/QDebug>


#define NUM_FINGERS	(5)
#define NUM_DEFECTS	(8)

template <typename T>
static void dealloc(T& p) {
    if (p)
        free(p);
    p = NULL;
}


OpenCV::OpenCV(void)
    : mCamera(NULL)
    , mFingers(NULL)
    , mDefects(NULL)
{
    // ...
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


bool OpenCV::stopCapture(void)
{
    if (mCamera == NULL)
        return false;
    cvReleaseImage(&mThresholdImage);
    cvReleaseImage(&mTempImage1);
    cvReleaseImage(&mTempImage3);
    cvReleaseMemStorage(&mContourStorage);
    cvReleaseMemStorage(&mHullStorage);
    cvReleaseMemStorage(&mDefectsStorage);
    cvReleaseMemStorage(&mTempStorage);
    cvReleaseCapture(&mCamera);
    dealloc(mFingers);
    dealloc(mDefects);
    return true;
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


const QImage& OpenCV::getImage(void)
{
    if (!isCapturing())
        return mFrame;
    // filter & threshold
    mImage = cvQueryFrame(mCamera);
    cvSmooth(mImage, mTempImage3, CV_GAUSSIAN, 11, 11, 0, 0);
    cvSmooth(mTempImage3, mTempImage3, CV_MEDIAN, 11, 11, 0, 0);
    cvCvtColor(mTempImage3, mTempImage3, CV_BGR2HSV);
    cvInRangeS(mTempImage3, cvScalar(0, 0, 160, 0), cvScalar(255, 400, 300, 255), mThresholdImage);
    cvMorphologyEx(mThresholdImage, mThresholdImage, NULL, mKernel, CV_MOP_OPEN, 1);
    cvSmooth(mThresholdImage, mThresholdImage, CV_GAUSSIAN, 3, 3, 0, 0);

    // find contour
    double area, max_area = 0.0;
    CvSeq *contours, *tmp, *contour = NULL;
    cvCopy(mThresholdImage, mTempImage1, NULL);
    cvFindContours(mTempImage1, mTempStorage, &contours, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));
    for (tmp = contours; tmp; tmp = tmp->h_next) {
        area = fabs(cvContourArea(tmp, CV_WHOLE_SEQ, 0));
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
    CvSeq* defects;
    CvConvexityDefect* defectArray;
    int i;
    int x = 0, y = 0;
    float dist = 0;
    mHull = NULL;
    if (!mContour)
        return mFrame;
    mHull = cvConvexHull2(mContour, mHullStorage, CV_CLOCKWISE, 0);
    if (mHull) {
        /* Get convexity defects of contour w.r.t. the convex hull */
        defects = cvConvexityDefects(mContour, mHull, mDefectsStorage);
        if (defects && defects->total) {
            defectArray = (CvConvexityDefect*)calloc(defects->total, sizeof(CvConvexityDefect));
            cvCvtSeqToArray(defects, defectArray, CV_WHOLE_SEQ);
            /* Average depth points to get hand center */
            for (i = 0; i < defects->total && i < NUM_DEFECTS; i++) {
                x += defectArray[i].depth_point->x;
                y += defectArray[i].depth_point->y;
                mDefects[i] = cvPoint(defectArray[i].depth_point->x, defectArray[i].depth_point->y);
            }
            x /= defects->total;
            y /= defects->total;
            mNumDefects = defects->total;
            mHandCenter = cvPoint(x, y);
            /* Compute hand radius as mean of distances of defects' depth point to hand center */
            for (i = 0; i < defects->total; i++) {
                int d = (x - defectArray[i].depth_point->x) *
                        (x - defectArray[i].depth_point->x) +
                        (y - defectArray[i].depth_point->y) *
                        (y - defectArray[i].depth_point->y);
                dist += sqrt((float)d);
            }
            mHandRadius = int(dist / defects->total);
            free(defectArray);
        }
    }

    // find fingers
    int n;
    CvPoint max_point;
    int dist1 = 0, dist2 = 0;
    int finger_distance[NUM_FINGERS + 1];

    mNumFingers = 0;
    if (!mContour || !mHull)
        return mFrame;

    n = mContour->total;
    CvPoint* points = (CvPoint*)calloc(n, sizeof(CvPoint));
    cvCvtSeqToArray(mContour, points, CV_WHOLE_SEQ);

    /* Fingers are detected as points where the distance to the center is a local maximum */
    for (i = 0; i < n; ++i) {
        int dist;
        int cx = mHandCenter.x;
        int cy = mHandCenter.y;
        dist = (cx - points[i].x) * (cx - points[i].x) + (cy - points[i].y) * (cy - points[i].y);
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

    convertIplImageToQImage(mThresholdImage, mFrame);
    return mFrame;
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
