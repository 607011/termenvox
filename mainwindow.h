// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#ifndef __MAINWINDOW_H_
#define __MAINWINDOW_H_

#include <QMainWindow>
#include <QModelIndexList>
#include <QListWidgetItem>

#include "main.h"
#include "thereminwidget.h"


namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = NULL);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent*);

private slots:
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

private:
    void saveAppSettings(void);
    void restoreAppSettings(void);


private:
    Ui::MainWindow* ui;
    ThereminWidget* mThereminWidget;
};

#endif // __MAINWINDOW_H_
