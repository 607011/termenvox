// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#ifndef __MAINWINDOW_H_
#define __MAINWINDOW_H_

#include <QMainWindow>
#include <QListWidgetItem>
#include <QVector>
#include <Leap.h>
#include "main.h"


namespace Ui {
class MainWindow;
}

class MainWindowPrivate;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = NULL);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent*);

private slots:
    void resetTheremin(void);
    void startStopCapture(void);
    void startStopLeapMotionSensor(void);
    void objectsDetected(void);
    void setHands(const Leap::Hand &left, const Leap::Hand &right);
    void xMaxChanged(int);
    void yMaxChanged(int);
    void instrumentChanged(int);
    void effectsOrderChanged(void);
    void resetSettings(void);
    void volumeChanged(int);
    void scalingChanged(int);
    void minFrequencyChanged(int);
    void maxFrequencyChanged(int);
    void minFrequencyEntered(const QString&);
    void maxFrequencyEntered(const QString&);
    void lowPassFreqChanged(int);
    void highPassFreqChanged(int);
    void echoChanged(int);
    void chorusDepthChanged(int);
    void chorusFreqChanged(int freq);
    void freeVerbDampingChanged(int);
    void freeVerbRoomSizeChanged(int);
    void pitShiftChanged(int);
    void lentPitShiftChanged(int);
    void nRevChanged(int);
    void jcRevChanged(int);
    void prcRevChanged(int);
    void about(void);
    void aboutQt(void);

private: // methods
    void saveAppSettings(void);
    void restoreAppSettings(void);

private: // variables
    Ui::MainWindow* ui;

    QScopedPointer<MainWindowPrivate> d_ptr;
    Q_DECLARE_PRIVATE(MainWindow)
    Q_DISABLE_COPY(MainWindow)
};


#endif // __MAINWINDOW_H_
