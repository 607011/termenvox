// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#ifndef __OPENCVSCENE_H_
#define __OPENCVSCENE_H_

#include <QThread>
#include <QImage>
#include <QVector>
#include <QPoint>
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

    QPoint handCenter(void) const { return QPoint(mHandCenter.x, mHandCenter.y); }
    Fingers fingers(void) const
    {
        Fingers tips;
        CvPoint* const cvFingers = mFingers;
        const CvPoint* const cvFingersEnd = cvFingers + mNumFingers;
        for (CvPoint* cvFingers = mFingers; cvFingers < cvFingersEnd; ++cvFingers) {
            if (isCapturing()) {
                tips.append(QPoint(cvFingers->x, cvFingers->y));
                ++cvFingers;
            }
        }
        return tips;
    }
    int numFingers(void) const { return mNumFingers; }
    int handRadius(void) const { return mHandRadius; }

protected:
    void run(void);

private:
    void convertIplImageToQImage(const IplImage* iplImg, QImage& image);

private:
    CvCapture* mCamera;
    QImage mFrame;
    IplImage* mImage;
    IplImage* mThresholdImage;
    IplImage* mTempImage1;
    IplImage* mTempImage3;
    CvSeq* mContour;
    CvSeq* mHull;
    CvPoint mHandCenter;
    CvPoint* mFingers;
    CvPoint* mDefects;
    CvMemStorage* mHullStorage;
    CvMemStorage* mContourStorage;
    CvMemStorage* mTempStorage;
    CvMemStorage* mDefectsStorage;
    IplConvKernel* mKernel;
    int mNumFingers;
    int mHandRadius;
    int mNumDefects;
    bool mAbort;
    QWaitCondition mCond;
    QMutex mMutex;
};

#endif // __OPENCVSCENE_H_
