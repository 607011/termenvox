// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "theremin.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mThereminWidget(new ThereminWidget)
{
    ui->setupUi(this);
    ui->horizontalLayout->addWidget(mThereminWidget);

    QObject::connect(ui->instrumentComboBox, SIGNAL(currentIndexChanged(int)), SLOT(instrumentChanged(int)));
    QObject::connect(ui->volumeDial, SIGNAL(valueChanged(int)), SLOT(volumeChanged(int)));

    ui->instrumentComboBox->setCurrentIndex((int)Theremin::HeavyMetalFM);
    ui->volumeDial->setValue(500);
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::instrumentChanged(int idx)
{
    mThereminWidget->theremin().chooseInstrument((Theremin::Instrument)idx);
}


void MainWindow::volumeChanged(int volume)
{
    mThereminWidget->theremin().setGlobalVolume(qreal(volume) / (ui->volumeDial->maximum() - ui->volumeDial->minimum()));
    ui->volumeLCDNumber->display(volume/10);
}
