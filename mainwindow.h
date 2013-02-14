// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#ifndef __MAINWINDOW_H_
#define __MAINWINDOW_H_

#include <QMainWindow>
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
    void volumeChanged(int);
    void scalingChanged(int);
    void minFrequencyChanged(int);
    void maxFrequencyChanged(int);

private:
    void saveAppSettings(void);
    void restoreAppSettings(void);


private:
    Ui::MainWindow* ui;
    ThereminWidget* mThereminWidget;
};

#endif // __MAINWINDOW_H_
