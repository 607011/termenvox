// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#include <QSettings>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "theremin.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mThereminWidget(new ThereminWidget)
{
    QSettings::setDefaultFormat(QSettings::NativeFormat);

    ui->setupUi(this);
    ui->horizontalLayout->addWidget(mThereminWidget);

    QObject::connect(ui->instrumentComboBox, SIGNAL(currentIndexChanged(int)), SLOT(instrumentChanged(int)));
    QObject::connect(ui->volumeDial, SIGNAL(valueChanged(int)), SLOT(volumeChanged(int)));

    restoreAppSettings();
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::closeEvent(QCloseEvent* e)
{
    saveAppSettings();
}


void MainWindow::saveAppSettings(void)
{
    QSettings settings(Company, AppName);
    settings.setValue("MainWindow/geometry", saveGeometry());
    settings.setValue("MainWindow/windowState", saveState());
    settings.setValue("MainWindow/instrument", ui->instrumentComboBox->currentIndex());
    settings.setValue("MainWindow/volume", ui->volumeDial->value());
}


void MainWindow::restoreAppSettings(void)
{
    QSettings settings(Company, AppName);
    restoreGeometry(settings.value("MainWindow/geometry").toByteArray());
    restoreState(settings.value("MainWindow/windowState").toByteArray());
    ui->instrumentComboBox->setCurrentIndex(settings.value("MainWindow/instrument").toInt());
    ui->volumeDial->setValue(settings.value("MainWindow/volume").toInt());
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
