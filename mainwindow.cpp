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
    QObject::connect(ui->scaleComboBox, SIGNAL(currentIndexChanged(int)), SLOT(scalingChanged(int)));
    QObject::connect(ui->minFDial, SIGNAL(valueChanged(int)), SLOT(minFrequencyChanged(int)));
    QObject::connect(ui->maxFDial, SIGNAL(valueChanged(int)), SLOT(maxFrequencyChanged(int)));
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
    settings.setValue("MainWindow/scaling", ui->scaleComboBox->currentIndex());
    settings.setValue("MainWindow/minF", ui->minFDial->value());
    settings.setValue("MainWindow/maxF", ui->maxFDial->value());
}


void MainWindow::restoreAppSettings(void)
{
    QSettings settings(Company, AppName);
    restoreGeometry(settings.value("MainWindow/geometry").toByteArray());
    restoreState(settings.value("MainWindow/windowState").toByteArray());
    ui->instrumentComboBox->setCurrentIndex(settings.value("MainWindow/instrument").toInt());
    ui->volumeDial->setValue(settings.value("MainWindow/volume").toInt());
    ui->scaleComboBox->setCurrentIndex(settings.value("MainWindow/scaling").toInt());
    ui->minFDial->setValue(settings.value("MainWindow/minF").toInt());
    ui->maxFDial->setValue(settings.value("MainWindow/maxF").toInt());
}


void MainWindow::instrumentChanged(int idx)
{
    mThereminWidget->theremin().chooseInstrument((Theremin::Instrument)idx);
}


void MainWindow::volumeChanged(int volume)
{
    mThereminWidget->theremin().setGlobalVolume(qreal(volume) / (ui->volumeDial->maximum() - ui->volumeDial->minimum()));
    ui->volumeLabel->setText(QString("%1").arg(volume/10));
}


void MainWindow::scalingChanged(int scaling)
{
    mThereminWidget->setScaling((ThereminWidget::Scaling)scaling);
}


void MainWindow::minFrequencyChanged(int freq)
{
    ui->minFLabel->setText(QString("%1").arg(freq));
    mThereminWidget->setFrequencyRange(freq, ui->maxFDial->value());
}


void MainWindow::maxFrequencyChanged(int freq)
{
    ui->maxFLabel->setText(QString("%1").arg(freq));
    mThereminWidget->setFrequencyRange(ui->minFDial->value(), freq);
}
