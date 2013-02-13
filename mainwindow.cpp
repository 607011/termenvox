// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "thereminwidget.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->horizontalLayout->addWidget(new ThereminWidget);
}


MainWindow::~MainWindow()
{
    delete ui;
}
