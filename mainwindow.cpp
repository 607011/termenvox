// Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

#include <QtCore/QDebug>
#include <QSettings>
#include <QMessageBox>
#include <QIntValidator>
#include <QStringList>
#include <QModelIndex>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "theremin.h"


QListWidgetItem* makeEffectListItem(const QString&, Theremin::Postprocessing);


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mThereminWidget(new ThereminWidget)
    , mTheremin(mThereminWidget->theremin())
    , mCamWidget(new CamWidget)
{
    QSettings::setDefaultFormat(QSettings::NativeFormat);

    ui->setupUi(this);
    setWindowTitle(tr("%1 %2").arg(AppName).arg(AppVersion));

    ui->horizontalLayoutMouse->addWidget(mThereminWidget);
    ui->horizontalLayoutCam->insertWidget(1, mCamWidget);

    ui->minFLineEdit->setValidator(new QIntValidator(ui->minFDial->minimum(), ui->minFDial->maximum()));
    ui->maxFLineEdit->setValidator(new QIntValidator(ui->maxFDial->minimum(), ui->maxFDial->maximum()));

    QObject::connect(ui->effectsListWidget, SIGNAL(itemClicked(QListWidgetItem*)), SLOT(effectsOrderChanged(void)));
    QObject::connect(ui->effectsListWidget->model(), SIGNAL(rowsMoved(const QModelIndex&, int, int, const QModelIndex&, int)), SLOT(effectsOrderChanged(void)));
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
    QObject::connect(ui->pitShiftDial, SIGNAL(valueChanged(int)), SLOT(pitShiftChanged(int)));
    QObject::connect(ui->lentPitShiftDial, SIGNAL(valueChanged(int)), SLOT(lentPitShiftChanged(int)));
    QObject::connect(ui->nRevDecayDial, SIGNAL(valueChanged(int)), SLOT(nRevChanged(int)));
    QObject::connect(ui->jcRevDecayDial, SIGNAL(valueChanged(int)), SLOT(jcRevChanged(int)));
    QObject::connect(ui->prcRevDecayDial, SIGNAL(valueChanged(int)), SLOT(prcRevChanged(int)));
    QObject::connect(ui->echoDial, SIGNAL(valueChanged(int)), SLOT(echoChanged(int)));
    QObject::connect(ui->onOffPushButton, SIGNAL(clicked()), SLOT(resetTheremin()));
    QObject::connect(ui->camOnOffPushButton, SIGNAL(clicked()), SLOT(startStopCapture()));
    QObject::connect(ui->actionHzScale, SIGNAL(toggled(bool)), mThereminWidget, SLOT(setShowHzScale(bool)));
    QObject::connect(ui->actionToneScale, SIGNAL(toggled(bool)), mThereminWidget, SLOT(setShowToneScale(bool)));
    QObject::connect(ui->actionVolumeScale, SIGNAL(toggled(bool)), mThereminWidget, SLOT(setShowLoudnessScale(bool)));
    QObject::connect(ui->actionResetSettings, SIGNAL(triggered()), SLOT(resetSettings()));
    QObject::connect(ui->actionExit, SIGNAL(triggered()), SLOT(close()));
    QObject::connect(ui->actionAbout, SIGNAL(triggered()), SLOT(about()));
    QObject::connect(ui->actionAboutQt, SIGNAL(triggered()), SLOT(aboutQt()));
    QObject::connect(mCamWidget->cv(), SIGNAL(objectsDetected()), SLOT(objectsDetected()));

    mEffects << makeEffectListItem(tr("Chorus Effect"), Theremin::ChorusEffect)
             << makeEffectListItem(tr("Echo Effect"), Theremin::EchoEffect)
             << makeEffectListItem(tr("Pitch Shift Effect"), Theremin::PitchShiftEffect)
             << makeEffectListItem(tr("Lent Pitch Shift Effect"), Theremin::LentPitchShiftEffect)
             << makeEffectListItem(tr("NRev Effect"), Theremin::NRevEffect)
             << makeEffectListItem(tr("JCRev Effect"), Theremin::JCRevEffect)
             << makeEffectListItem(tr("PRCRev Effect"), Theremin::PRCRevEffect)
             << makeEffectListItem(tr("FreeVerb Effect"), Theremin::FreeVerbEffect)
             << makeEffectListItem(tr("Low Pass Filter"), Theremin::LowPassFilter)
             << makeEffectListItem(tr("High Pass Filter"), Theremin::HighPassFilter);
    restoreAppSettings();

}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::closeEvent(QCloseEvent*)
{
    saveAppSettings();
}


void MainWindow::resetTheremin(void)
{
    mTheremin.reset();
}


void MainWindow::startStopCapture()
{
    if (ui->camOnOffPushButton->text() == tr("Start")) {
        mCamWidget->startCapture();
        ui->camOnOffPushButton->setText(tr("Stop"));
    }
    else {
        mCamWidget->stopCapture();
        ui->camOnOffPushButton->setText(tr("Start"));
    }
}


void MainWindow::objectsDetected(void)
{
    const QVector<QRectF> objects = mCamWidget->cv()->detectedObjects();
    const QSize& sz = mCamWidget->cv()->getImageSize();
    const int w2 = sz.width() / 2;
    QVector<QPointF> leftSide, rightSide;
    for (QVector<QRectF>::const_iterator i = objects.constBegin(); i != objects.constEnd(); ++i) {
        if (i->x() > w2)
            rightSide.push_back(i->center());
        else
            leftSide.push_back(i->center());
    }
    QPointF left, right;
    if (!leftSide.empty())
        left = leftSide.first();
    if (!rightSide.empty())
        right = rightSide.first() - QPointF(w2, 0);
    mThereminWidget->setVolume(left.y() / sz.height());
    mThereminWidget->setFrequency1(right.x() / w2);
    if (!left.isNull() && !right.isNull())
        mTheremin.play();
    else
        mTheremin.stop();
}


QListWidgetItem* makeEffectListItem(const QString& name, Theremin::Postprocessing id)
{
    QListWidgetItem* item = new QListWidgetItem(name);
    item->setData(Qt::UserRole, id);
    return item;
}


void MainWindow::restoreAppSettings(void)
{
    QSettings settings(Company, AppName);
    restoreGeometry(settings.value("MainWindow/geometry").toByteArray());
    restoreState(settings.value("MainWindow/windowState").toByteArray());
    ui->toolBox->setCurrentIndex(settings.value("MainWindow/toolbox/page", 0).toInt());
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
    ui->lowPassDial->setValue(settings.value("Filters/LowPass", -1).toInt());
    lowPassFreqChanged(ui->lowPassDial->value());
    ui->highPassDial->setValue(settings.value("Filters/HighPass", -1).toInt());
    highPassFreqChanged(ui->highPassDial->value());
    ui->echoDial->setValue(settings.value("Effects/Echo", -1).toInt());
    ui->chorusDepthDial->setValue(settings.value("Effects/Chorus/depth", 0).toInt());
    chorusDepthChanged(ui->chorusDepthDial->value());
    ui->chorusFreqDial->setValue(settings.value("Effects/Chorus/frequency", 0).toInt());
    chorusFreqChanged(ui->chorusFreqDial->value());
    ui->freeVerbDampingDial->setValue(settings.value("Effects/FreeVerb/damping", 0).toInt());
    freeVerbDampingChanged(ui->freeVerbDampingDial->value());
    ui->freeVerbRoomSizeDial->setValue(settings.value("Effects/FreeVerb/roomsize", 0).toInt());
    freeVerbRoomSizeChanged(ui->freeVerbRoomSizeDial->value());
    ui->jcRevDecayDial->setValue(settings.value("Effects/JCRev", 0).toInt());
    jcRevChanged(ui->jcRevDecayDial->value());
    ui->prcRevDecayDial->setValue(settings.value("Effects/PRCRev", 0).toInt());
    prcRevChanged(ui->prcRevDecayDial->value());
    ui->pitShiftDial->setValue(settings.value("Effects/PitShift", 0).toInt());
    pitShiftChanged(ui->pitShiftDial->value());
    ui->lentPitShiftDial->setValue(settings.value("Effects/LentPitShift", 0).toInt());
    pitShiftChanged(ui->lentPitShiftDial->value());

    // Restore effect order. If none given, set a default.
    QVector<Theremin::Postprocessing> effectId;
    QVector<bool> effectEnabled;
    QList<int> id0;
    id0 << 8 << 9 << 1 << 0 << 2 << 3 << 4 << 5 << 6 << 7;
    QStringList oid = settings.value("Effects/order/id").toStringList();
    for (QStringList::const_iterator i = oid.constBegin(); i != oid.constEnd(); ++i)
        effectId.push_back((Theremin::Postprocessing)i->toInt());
    for (QList<int>::const_iterator i = id0.constBegin(); i != id0.constEnd(); ++i)
        if (!effectId.contains((Theremin::Postprocessing)*i))
            effectId.push_back((Theremin::Postprocessing)*i);

    QStringList oen = settings.value("Effects/order/enabled").toStringList();
    for (int i = 0; i < effectId.size() - oen.size(); ++i)
        oen.append("false");
    for (QStringList::const_iterator i = oen.constBegin(); i != oen.constEnd(); ++i)
        effectEnabled.push_back(*i == "true");

    ui->effectsListWidget->blockSignals(true);
    QVector<Theremin::Postprocessing>::const_iterator itId = effectId.constBegin();
    QVector<bool>::const_iterator itEnabled = effectEnabled.constBegin();
    while (itId != effectId.constEnd() && itEnabled != effectEnabled.constEnd()) {
        const int id = *itId;
        if (0 <= id && id < Theremin::LastPostprocessor) {
            mEffects[id]->setCheckState(*itEnabled? Qt::Checked : Qt::Unchecked);
            ui->effectsListWidget->addItem(mEffects[id]);
        }
        ++itId;
        ++itEnabled;
    }
    ui->effectsListWidget->blockSignals(false);
    effectsOrderChanged();

}


void MainWindow::saveAppSettings(void)
{
    QSettings settings(Company, AppName);
    settings.setValue("MainWindow/geometry", saveGeometry());
    settings.setValue("MainWindow/windowState", saveState());
    settings.setValue("MainWindow/toolbox/page", ui->toolBox->currentIndex());
    settings.setValue("MainWindow/instrument", ui->instrumentComboBox->currentIndex());
    settings.setValue("MainWindow/volume", ui->volumeDial->value());
    settings.setValue("MainWindow/scaling", ui->scaleComboBox->currentIndex());
    settings.setValue("MainWindow/hzScale", ui->actionHzScale->isChecked());
    settings.setValue("MainWindow/toneScale", ui->actionToneScale->isChecked());
    settings.setValue("MainWindow/volumeScale", ui->actionVolumeScale->isChecked());
    settings.setValue("MainWindow/minF", ui->minFDial->value());
    settings.setValue("MainWindow/maxF", ui->maxFDial->value());
    settings.setValue("Filters/LowPass", ui->lowPassDial->value());
    settings.setValue("Filters/HighPass", ui->highPassDial->value());
    settings.setValue("Effects/Echo", ui->echoDial->value());
    settings.setValue("Effects/Chorus/depth", ui->chorusDepthDial->value());
    settings.setValue("Effects/Chorus/frequency", ui->chorusFreqDial->value());
    settings.setValue("Effects/FreeVerb/damping", ui->freeVerbDampingDial->value());
    settings.setValue("Effects/FreeVerb/roomsize", ui->freeVerbRoomSizeDial->value());
    settings.setValue("Effects/JCRev", ui->jcRevDecayDial->value());
    settings.setValue("Effects/PRCRev", ui->prcRevDecayDial->value());
    settings.setValue("Effects/PitShift", ui->pitShiftDial->value());
    settings.setValue("Effects/LentPitShift", ui->lentPitShiftDial->value());

    // save effect order
    QList<QVariant> effectId;
    QList<QVariant> effectEnabled;
    for (int i = 0; i < ui->effectsListWidget->count(); ++i) {
        const QListWidgetItem* const item = ui->effectsListWidget->item(i);
        int id = item->data(Qt::UserRole).toInt();
        bool enabled = item->checkState() == Qt::Checked;
        effectId.append(id);
        effectEnabled.append(enabled);
    }
    settings.setValue("Effects/order/id", effectId);
    settings.setValue("Effects/order/enabled", effectEnabled);
}


void MainWindow::instrumentChanged(int idx)
{
    mTheremin.chooseInstrument((Theremin::Instrument)idx);
}


void MainWindow::effectsOrderChanged(void)
{
    QVector<Theremin::Effect> effects;
    for (int i = 0; i < ui->effectsListWidget->count(); ++i) {
        const QListWidgetItem* const item = ui->effectsListWidget->item(i);
        Theremin::Effect effect = {
            (Theremin::Postprocessing)item->data(Qt::UserRole).toInt(),
            item->checkState() == Qt::Checked
        };
        effects.push_back(effect);
    }
    mTheremin.setEffects(effects.toStdVector());
}


void MainWindow::resetSettings(void)
{
    ui->instrumentComboBox->setCurrentIndex(Theremin::BeeThree);
    ui->volumeDial->setValue(ui->volumeDial->maximum()/2);
    ui->lowPassDial->setValue(0);
    ui->highPassDial->setValue(0);
    ui->minFDial->setValue(10);
    ui->maxFDial->setValue(4000);
    ui->echoDial->setValue(0);
    ui->chorusDepthDial->setValue(0);
    ui->chorusFreqDial->setValue(0);
    ui->freeVerbDampingDial->setValue(0);
    ui->freeVerbRoomSizeDial->setValue(0);
    ui->nRevDecayDial->setValue(0);
    ui->jcRevDecayDial->setValue(0);
    ui->prcRevDecayDial->setValue(0);
    ui->pitShiftDial->setValue(0);
    ui->lentPitShiftDial->setValue(0);
    QMessageBox::information(this, tr("Settings reset"), tr("All settings have been set to their respective default values."));
}


void MainWindow::volumeChanged(int volume)
{
    qreal v = qreal(volume) / (ui->volumeDial->maximum() - ui->volumeDial->minimum());
    mTheremin.setGlobalVolume(v);
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
    mTheremin.setLowPassFrequency(qreal(pole) / ui->lowPassDial->maximum());
}


void MainWindow::highPassFreqChanged(int pole)
{
    mTheremin.setHighPassFrequency(qreal(pole) / ui->highPassDial->maximum());
}


void MainWindow::echoChanged(int delay)
{
    mTheremin.setEcho(delay);
}


void MainWindow::chorusDepthChanged(int depth)
{
    mTheremin.setChorusDepth(qreal(depth) / ui->chorusDepthDial->maximum());
}


void MainWindow::chorusFreqChanged(int freq)
{
    mTheremin.setChorusFrequency(freq);
}


void MainWindow::freeVerbDampingChanged(int damping)
{
    mTheremin.setFreeVerbDamping(qreal(damping) / ui->freeVerbDampingDial->maximum());
}


void MainWindow::freeVerbRoomSizeChanged(int size)
{
    mTheremin.setFreeVerbRoomSize(qreal(size) / ui->freeVerbRoomSizeDial->maximum());
}


void MainWindow::pitShiftChanged(int shift)
{
    mTheremin.setPitShift(qreal(shift) / ui->pitShiftDial->maximum());
}


void MainWindow::lentPitShiftChanged(int shift)
{
    mTheremin.setLentPitShift(qreal(shift) / ui->lentPitShiftDial->maximum());
}


void MainWindow::nRevChanged(int decay)
{
    mTheremin.setNRevDecay(qreal(decay) / ui->nRevDecayDial->maximum());
}


void MainWindow::jcRevChanged(int decay)
{
    mTheremin.setJCRevDecay(qreal(decay) / ui->jcRevDecayDial->maximum());
}


void MainWindow::prcRevChanged(int decay)
{
    mTheremin.setPRCRevDecay(qreal(decay) / ui->prcRevDecayDial->maximum());
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
