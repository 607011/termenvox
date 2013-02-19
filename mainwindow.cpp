// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#include <QtCore/QDebug>
#include <QSettings>
#include <QMessageBox>
#include <QIntValidator>
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

    ui->minFLineEdit->setValidator(new QIntValidator(ui->minFDial->minimum(), ui->minFDial->maximum()));
    ui->maxFLineEdit->setValidator(new QIntValidator(ui->maxFDial->minimum(), ui->maxFDial->maximum()));

    QObject::connect(ui->instrumentComboBox, SIGNAL(currentIndexChanged(int)), SLOT(instrumentChanged(int)));
    QObject::connect(ui->volumeDial, SIGNAL(valueChanged(int)), SLOT(volumeChanged(int)));
    QObject::connect(ui->scaleComboBox, SIGNAL(currentIndexChanged(int)), SLOT(scalingChanged(int)));
    QObject::connect(ui->minFDial, SIGNAL(valueChanged(int)), SLOT(minFrequencyChanged(int)));
    QObject::connect(ui->maxFDial, SIGNAL(valueChanged(int)), SLOT(maxFrequencyChanged(int)));
    QObject::connect(ui->minFLineEdit, SIGNAL(textChanged(QString)), SLOT(minFrequencyEntered(QString)));
    QObject::connect(ui->maxFLineEdit, SIGNAL(textChanged(QString)), SLOT(maxFrequencyEntered(QString)));
    QObject::connect(ui->lowPassDial, SIGNAL(valueChanged(int)), SLOT(lowPassFreqChanged(int)));
    QObject::connect(ui->highPassDial, SIGNAL(valueChanged(int)), SLOT(highPassFreqChanged(int)));
    QObject::connect(ui->chorusDepthDial, SIGNAL(valueChanged(int)), SLOT(chorusDepthChanged(int)));
    QObject::connect(ui->chorusFreqDial, SIGNAL(valueChanged(int)), SLOT(chorusFreqChanged(int)));
    QObject::connect(ui->freeVerbDampingDial, SIGNAL(valueChanged(int)), SLOT(freeVerbDampingChanged(int)));
    QObject::connect(ui->freeVerbRoomSizeDial, SIGNAL(valueChanged(int)), SLOT(freeVerbRoomSizeChanged(int)));
    QObject::connect(ui->echoDial, SIGNAL(valueChanged(int)), SLOT(echoChanged(int)));
    QObject::connect(ui->actionHzScale, SIGNAL(toggled(bool)), mThereminWidget, SLOT(setShowHzScale(bool)));
    QObject::connect(ui->actionToneScale, SIGNAL(toggled(bool)), mThereminWidget, SLOT(setShowToneScale(bool)));
    QObject::connect(ui->actionVolumeScale, SIGNAL(toggled(bool)), mThereminWidget, SLOT(setShowLoudnessScale(bool)));
    QObject::connect(ui->actionExit, SIGNAL(triggered()), SLOT(close()));
    QObject::connect(ui->actionAbout, SIGNAL(triggered()), SLOT(about()));
    QObject::connect(ui->actionAboutQt, SIGNAL(triggered()), SLOT(aboutQt()));

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
    settings.setValue("MainWindow/hzScale", ui->actionHzScale->isChecked());
    settings.setValue("MainWindow/toneScale", ui->actionToneScale->isChecked());
    settings.setValue("MainWindow/volumeScale", ui->actionVolumeScale->isChecked());
    settings.setValue("MainWindow/minF", ui->minFDial->value());
    settings.setValue("MainWindow/maxF", ui->maxFDial->value());
    settings.setValue("MainWindow/lowPass", ui->lowPassDial->value());
    settings.setValue("MainWindow/highPass", ui->highPassDial->value());
    settings.setValue("MainWindow/echo", ui->echoDial->value());
    settings.setValue("MainWindow/chorusDepth", ui->chorusDepthDial->value());
    settings.setValue("MainWindow/chorusFreq", ui->chorusFreqDial->value());
}


void MainWindow::restoreAppSettings(void)
{
    QSettings settings(Company, AppName);
    restoreGeometry(settings.value("MainWindow/geometry").toByteArray());
    restoreState(settings.value("MainWindow/windowState").toByteArray());
    ui->instrumentComboBox->setCurrentIndex(settings.value("MainWindow/instrument", Theremin::BeeThree).toInt());
    ui->volumeDial->setValue(settings.value("MainWindow/volume", 1000).toInt());
    ui->scaleComboBox->setCurrentIndex(settings.value("MainWindow/scaling", ThereminWidget::Logarithmic).toInt());
    ui->actionHzScale->setChecked(settings.value("MainWindow/hzScale", false).toBool());
    mThereminWidget->setShowHzScale(ui->actionHzScale->isChecked());
    ui->actionToneScale->setChecked(settings.value("MainWindow/toneScale", true).toBool());
    mThereminWidget->setShowToneScale(ui->actionToneScale->isChecked());
    ui->actionVolumeScale->setChecked(settings.value("MainWindow/volumeScale", true).toBool());
    mThereminWidget->setShowLoudnessScale(ui->actionVolumeScale->isChecked());
    ui->minFDial->setValue(settings.value("MainWindow/minF", 10).toInt());
    ui->maxFDial->setValue(settings.value("MainWindow/maxF", 4000).toInt());
    ui->lowPassDial->setValue(settings.value("MainWindow/lowPass", -1).toInt());
    lowPassFreqChanged(ui->lowPassDial->value());
    ui->highPassDial->setValue(settings.value("MainWindow/highPass", -1).toInt());
    highPassFreqChanged(ui->highPassDial->value());
    ui->echoDial->setValue(settings.value("MainWindow/echo", -1).toInt());
    ui->chorusDepthDial->setValue(settings.value("MainWindow/chorusDepth", 0).toInt());
    ui->chorusFreqDial->setValue(settings.value("MainWindow/chorusFreq", 0).toInt());
}


void MainWindow::instrumentChanged(int idx)
{
    mThereminWidget->theremin().chooseInstrument((Theremin::Instrument)idx);
}


void MainWindow::volumeChanged(int volume)
{
    qreal v = qreal(volume) / (ui->volumeDial->maximum() - ui->volumeDial->minimum());
    mThereminWidget->theremin().setGlobalVolume(v);
}


void MainWindow::scalingChanged(int scaling)
{
    mThereminWidget->setScaling((ThereminWidget::Scaling)scaling);
}


void MainWindow::minFrequencyChanged(int freq)
{
    ui->minFLineEdit->setText(QString("%1").arg(freq));
    mThereminWidget->setFrequencyRange(freq, ui->maxFDial->value());
}


void MainWindow::maxFrequencyChanged(int freq)
{
    ui->maxFLineEdit->setText(QString("%1").arg(freq));
    mThereminWidget->setFrequencyRange(ui->minFDial->value(), freq);
}


void MainWindow::minFrequencyEntered(const QString& text)
{
    ui->minFDial->setValue(text.toInt());
}


void MainWindow::maxFrequencyEntered(const QString& text)
{
    ui->maxFDial->setValue(text.toInt());
}


void MainWindow::lowPassFreqChanged(int pole)
{
    mThereminWidget->theremin().setLowPassFrequency(qreal(pole) / ui->lowPassDial->maximum());
}


void MainWindow::highPassFreqChanged(int pole)
{
    mThereminWidget->theremin().setHighPassFrequency(qreal(pole) / ui->highPassDial->maximum());
}


void MainWindow::echoChanged(int delay)
{
    mThereminWidget->theremin().setEcho(delay);
}


void MainWindow::chorusDepthChanged(int depth)
{
    mThereminWidget->theremin().setChorusDepth(qreal(depth) / ui->chorusDepthDial->maximum());
}


void MainWindow::chorusFreqChanged(int freq)
{
    mThereminWidget->theremin().setChorusFrequency(freq);
}


void MainWindow::freeVerbDampingChanged(int damping)
{
    mThereminWidget->theremin().setFreeVerbDamping(qreal(damping) / ui->freeVerbDampingDial->maximum());
}


void MainWindow::freeVerbRoomSizeChanged(int size)
{
    mThereminWidget->theremin().setFreeVerbRoomSize(qreal(size) / ui->freeVerbRoomSizeDial->maximum());
}


void MainWindow::about(void)
{
    QMessageBox::about(this, tr("About %1 %2%3").arg(AppName).arg(AppVersionNoDebug).arg(AppMinorVersion),
                       tr("<p><b>%1</b> simulates the electrical instrument invented by Lev Termen. "
                          "See <a href=\"%2\" title=\"%1 project homepage\">%2</a> for more info.</p>"
                          "<p>Copyright &copy; 2013 %3 &lt;%4&gt;, Heise Zeitschriften Verlag.</p>"
                          "<p>This program is free software: you can redistribute it and/or modify "
                          "it under the terms of the GNU General Public License as published by "
                          "the Free Software Foundation, either version 3 of the License, or "
                          "(at your option) any later version.</p>"
                          "<p>This program is distributed in the hope that it will be useful, "
                          "but WITHOUT ANY WARRANTY; without even the implied warranty of "
                          "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the "
                          "GNU General Public License for more details.</p>"
                          "You should have received a copy of the GNU General Public License "
                          "along with this program. "
                          "If not, see <a href=\"http://www.gnu.org/licenses/gpl-3.0\">http://www.gnu.org/licenses</a>.</p>")
                       .arg(AppName).arg(AppUrl).arg(AppAuthor).arg(AppAuthorMail));
}


void MainWindow::aboutQt(void)
{
    QMessageBox::aboutQt(this);
}
