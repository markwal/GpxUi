#include "main.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "rungpx.h"
#include "updates.h"
#include "machineeditor.h"
#include "build/version.h"

#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QSettings>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    heightCollapsed = 0;
    ui->setupUi(this);
    populateMachineType(*ui->comboMachineType);
    setAdvanced(false);
    heightCollapsed = sizeHint().height();
    setFixedHeight(heightCollapsed);

    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(on_about()));
    connect(ui->actionAbout_Qt, SIGNAL(triggered()), QApplication::instance(), SLOT(aboutQt()));

    on_btnReload_clicked();

    Updates::autoUpdate();
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::iniEditorParserCallback(void *user, QString &sSection, Line &line)
{
    MainWindow *pmw = (MainWindow *)user;

    // translate some synonyms to the canonical names for the index/retrieval
    if (sSection != pmw->sSectionCur) {
        sSection = sSection.toLower();
        if (sSection == "slicer")
            sSection = "printer";
        pmw->sSectionCur = sSection;
    }
    line.sName = line.sName.toLower();
    if (line.sName == "nominal_filament_diameter" || line.sName == "filament_diameter")
        line.sName = "slicer_filament_diameter";
    if (sSection == "machine" && line.sName == "slicer_filament_diameter")
        sSection = "printer";
    if (line.sName == "nozzle_temperature")
        line.sName = "active_temperature";
    line.sValue = line.sValue.toLower();
    return true;
}

void MainWindow::setAdvanced(bool isAdvanced)
{
    ui->widgetAdvanced->setHidden(!isAdvanced);
    ui->btnAdvancedToggle->setText(isAdvanced ? tr("Advanced <<") : tr("Advanced >>"));
    if (heightCollapsed)
        setFixedHeight(isAdvanced ? sizeHint().height() : heightCollapsed);
}

void MainWindow::on_btnAdvancedToggle_clicked()
{
    setAdvanced(ui->widgetAdvanced->isHidden());
}

void MainWindow::on_tbMachineEditor_clicked()
{
    MachineEditor me(this, ui->comboMachineType->currentData().toString());

    me.exec();
}

bool MainWindow::chooseInputFile()
{
    QSettings settings;

    // Attempt to set the current directory to the directory in the edit box.
    // If it is empty, or not a folder, attempt to use the most recent folder.
    // Otherwise, give up and let QFileDialog decide
    QString s = ui->editInput->text();
    QDir dir(QFileInfo(s).path());
    if (s.isEmpty() || !dir.exists()) {
        s = settings.value("mostRecentFolder", "").toString();
        if (!s.isEmpty())
            dir.setPath(s);
    }

    s = QFileDialog::getOpenFileName(this, tr("Choose Input Gcode File"), dir.cleanPath(dir.absolutePath()), tr("Gcode (*.gcode *.gco *.g)"));

    if (s.isEmpty())
        return false; // user cancelled

    // Save the most recent folder in settings
    settings.setValue("mostRecentFolder", QFileInfo(s).path());
    ui->editInput->setText(s);
    return true;
}

void MainWindow::on_tbtnInputGcode_clicked()
{
    chooseInputFile();
}

void MainWindow::on_btnTranslate_clicked()
{
    if (ui->editInput->text().isEmpty()) {
        if (!chooseInputFile())
            return;
    }

    QFileInfo fi(ui->editInput->text());
    while (!fi.exists()) {
        QMessageBox msgbox(this);
        msgbox.setText("Unable to find the input file.");
        msgbox.setInformativeText("Would you like to choose another?");
        msgbox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        int ret = msgbox.exec();
        if (ret != QMessageBox::Ok)
            return;
        if (!chooseInputFile())
            return;
        fi.setFile(ui->editInput->text());
    }

    QDir dir(ui->editInput->text());
    QString sDefaultFileName = dir.absoluteFilePath(fi.completeBaseName() + ".x3g");

    QString sFileName = QFileDialog::getSaveFileName(this, tr("Choose X3g Output File"),
        sDefaultFileName, tr("X3g (*.x3g *.s3g)"));
    if (sFileName.isEmpty())
        return; // user cancelled

    saveToIni();

    RunGpx rungpx;
    rungpx.Translate(ui->editInput->text(), sFileName);
}

void MainWindow::populateMachineType(QComboBox &combo)
{
    int imtMax = combo.maxCount();
    combo.clear();
    combo.insertItem(imtMax, "Cupcake Gen3 XYZ, Mk5/6 + Gen4 Extruder", "c3");
    combo.insertItem(imtMax, "Cupcake Gen4 XYZ, Mk5/6 + Gen4 Extruder", "c4");
    combo.insertItem(imtMax, "Cupcake Pololu XYZ, Mk5/6 + Gen4 Extruder", "cp4");
    combo.insertItem(imtMax, "Cupcake Pololu XYZ, Mk5/6 + Pololu Extruder", "cpp");
    combo.insertItem(imtMax, "Core-XY with HBP - single extruder", "cxy");
    combo.insertItem(imtMax, "Core-XY with HBP - single extruder, slow Z", "cxysz");
    combo.insertItem(imtMax, "Clone R1 Single with HBP", "cr1");
    combo.insertItem(imtMax, "Clone R1 Dual with HBP", "cr1d");
    combo.insertItem(imtMax, "FlashForge Creator Pro", "fcp");
    combo.insertItem(imtMax, "Replicator 1 - single extruder", "r1");
    combo.insertItem(imtMax, "Replicator 1 - dual extruder", "r1d");
    combo.insertItem(imtMax, "Replicator 2", "r2");
    combo.insertItem(imtMax, "Replicator 2 with HBP", "r2h");
    combo.insertItem(imtMax, "Replicator 2X", "r2x");
    combo.insertItem(imtMax, "ZYYX - single extruder", "z");
    combo.insertItem(imtMax, "ZYYX - dual extruder", "zd");
}

void MainWindow::on_btnReload_clicked()
{
    sSectionCur = QString();
    const QDir &dir = GpxUiInfo::iniLocation();
    QString sFilename = INIFILENAME_PREFIX "gpx.ini";
    QFileInfo fi(dir.absoluteFilePath(sFilename));
    ie.setFilename(fi.absoluteFilePath());
    if (fi.exists())
        ie.read(iniEditorParserCallback, this);
    else
        ie.clear();

    refreshFromIni();
}

void MainWindow::on_btnSave_clicked()
{
    saveToIni();
}

void MainWindow::refreshFromIni()
{
    // main
    Section *psect = &ie.section("printer");
    int imt = ui->comboMachineType->findData(psect->value("machine_type", "r2"));
    if (imt != -1)
        ui->comboMachineType->setCurrentIndex(imt);
    ui->comboGcodeFlavor->setCurrentIndex(psect->value("gcode_flavor", "reprap").compare("makerbot", Qt::CaseInsensitive) == 0);

    // advanced
    ui->cboxProgress->setChecked(psect->value("build_progress", "1").toInt());
    ui->cboxDitto->setChecked(psect->value("ditto_printing", "0").toInt());
    ui->cboxRecalc->setChecked(psect->value("recalculate_5d", "0").toInt());
    ui->dsbNominal->setValue(psect->value("slicer_filament_diameter", "0").toDouble());
    ui->sbHBP->setValue(psect->value("build_platform_temperature", "0").toInt());

    psect = &ie.section("left");
    ui->dsbLeftActual->setValue(psect->value("actual_filament_diameter", "0").toDouble());
    ui->dsbLeftDensity->setValue(psect->value("packing_density", "0").toDouble());
    ui->sbLeftTemp->setValue(psect->value("active_temperature", "0").toInt());
    ui->sbLeftStandby->setValue(psect->value("standby_temperature", "0").toInt());

    psect = &ie.section("right");
    ui->dsbRightActual->setValue(psect->value("actual_filament_diameter", "0").toDouble());
    ui->dsbRightDensity->setValue(psect->value("packing_density", "0").toDouble());
    ui->sbRightTemp->setValue(psect->value("active_temperature", "0").toInt());
    ui->sbRightStandby->setValue(psect->value("standby_temperature", "0").toInt());
}

void MainWindow::saveToIni()
{
    // main
    Section *psect = &ie.section("printer");
    psect->setValue("machine_type", ui->comboMachineType->currentData().toString());
    psect->setValue("gcode_flavor", ui->comboGcodeFlavor->currentIndex() ? "makerbot" : "reprap", "reprap");

    // advanced
    psect->setValue("build_progress", ui->cboxProgress->isChecked(), 1);
    psect->setValue("ditto_printing", ui->cboxDitto->isChecked(), 0);
    psect->setValue("recalculate_5d", ui->cboxRecalc->isChecked(), 0);
    psect->setValue("slicer_filament_diameter", ui->dsbNominal->value(), 0.0);
    psect->setValue("build_platform_temperature", ui->sbHBP->value(), 0);

    psect = &ie.section("left");
    psect->setValue("actual_filament_diameter", ui->dsbLeftActual->value(), 0.0);
    psect->setValue("packing_density", ui->dsbLeftDensity->value(), 0.0);
    psect->setValue("active_temperature", ui->sbLeftTemp->value(), 0);
    psect->setValue("standby_temperature", ui->sbLeftStandby->value(), 0);

    psect = &ie.section("right");
    psect->setValue("actual_filament_diameter", ui->dsbRightActual->value(), 0.0);
    psect->setValue("packing_density", ui->dsbRightDensity->value(), 0.0);
    psect->setValue("active_temperature", ui->sbRightTemp->value(), 0);
    psect->setValue("standby_temperature", ui->sbRightStandby->value(), 0);

    ie.write(true);
}

void MainWindow::on_btnDefaults_clicked()
{
    // main
    int imt = ui->comboMachineType->findData("r2");
    if (imt != -1)
        ui->comboMachineType->setCurrentIndex(imt);
    ui->comboGcodeFlavor->setCurrentIndex(0);

    // advanced
    ui->cboxProgress->setChecked(true);
    ui->cboxDitto->setChecked(false);
    ui->cboxRecalc->setChecked(false);
    ui->dsbNominal->setValue(0.0);
    ui->sbHBP->setValue(0);

    ui->dsbLeftActual->setValue(0.0);
    ui->dsbLeftDensity->setValue(0.0);
    ui->sbLeftTemp->setValue(0);
    ui->sbLeftStandby->setValue(0);

    ui->dsbRightActual->setValue(0.0);
    ui->dsbRightDensity->setValue(0.0);
    ui->sbRightTemp->setValue(0);
    ui->sbRightStandby->setValue(0);
}

void MainWindow::on_about()
{
    QMessageBox::about(this, "About GpxUi",
    "GpxUi v" GPXUI_VERSION "\n\n"

    "GpxUi is a graphical user interface wrapped around GPX, a command line utility\n\n"

    "GPX is a post process utility for converting gcode output from 3D slicing "
    "software like Cura, KISSlicer, S3DCreator and Slic3r to x3g files for standalone "
    "3D printing on Makerbot Cupcake, ThingOMatic, and Replicator 1/2/2x printers "
    "- with support for both stock and SailFish firmwares\n\n"

    "The sources for this version are found at " GPXUI_ORIGIN "\n\n"

    "GpxUi (GUI) Copyright (c) 2015 Mark Walker, All rights reserved.\n"
    "GPX Copyright (c) 2013 WHPThomas, All rights reserved.\n"
    "Additional changes Copyright (c) 2014, 2015 DNewman, All rights reserved.\n"
    "Portions copyright (c) 2009, Ben Hoyt, All rights reserved.\n"
    "Squirrel.Windows (c) 2012 Github, Inc.\n\n"

    "This program is free software; you can redistribute it and/or modify "
    "it under the terms of the GNU General Public License as published by "
    "the Free Software Foundation; either version 2 of the License, or "
    "(at your option) any later version.\n\n"

    "This program is distributed in the hope that it will be useful, "
    "but WITHOUT ANY WARRANTY; without even the implied warranty of "
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
    "GNU General Public License for more details.\n\n"

    "Please see additional license information in the documentation distributed with this program.\n"
    );
}

void MainWindow::on_action_Updates_triggered()
{
    Updates updates(this);
    updates.exec();
}
