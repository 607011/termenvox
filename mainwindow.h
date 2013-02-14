// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#ifndef __MAINWINDOW_H_
#define __MAINWINDOW_H_

#include <QMainWindow>
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

private slots:
    void instrumentChanged(int);
    void volumeChanged(int);

private:
    Ui::MainWindow* ui;
    ThereminWidget* mThereminWidget;
};

#endif // __MAINWINDOW_H_
