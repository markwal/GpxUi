#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "rungpx.h"
#include "build/version.h"

#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QSettings>

#ifdef Q_OS_WIN
#define INIFILENAME_PREFIX ""
#else
#define INIFILENAME_PREFIX "."
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    heightCollapsed = 0;
    ui->setupUi(this);
    populateMachineType(*ui->comboMachineType);
    setAdvanced(false);
    resize(sizeHint());

    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(on_about()));

    QString sFilename = INIFILENAME_PREFIX "gpxui.ini";
    QFileInfo fi(sFilename);
    if (fi.exists())
        ie.read(sFilename);
    else
        ie.clear();

    refreshFromIni(ie);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setAdvanced(bool isAdvanced)
{
    ui->widgetAdvanced->setHidden(!isAdvanced);
    ui->btnAdvancedToggle->setText(isAdvanced ? tr("Advanced <<") : tr("Advanced >>"));
}

void MainWindow::on_btnAdvancedToggle_clicked()
{
    setAdvanced(ui->widgetAdvanced->isHidden());
}

void MainWindow::on_tbtnInputGcode_clicked()
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

    s = QFileDialog::getOpenFileName(this, tr("Choose Gcode File"), dir.cleanPath(dir.absolutePath()), tr("Gcode (*.gcode *.gco *.g)"));

    // Save the most recent folder in settings
    if (!s.isEmpty()) {
        settings.setValue("mostRecentFolder", QFileInfo(s).path());
        ui->editInput->setText(s);
    }
}

void MainWindow::on_btnTranslate_clicked()
{
    QFileInfo fi(ui->editInput->text());
    QDir dir(ui->editInput->text());
    QString sDefaultFileName = dir.absoluteFilePath(fi.completeBaseName() + ".x3g");

    QString sFileName = QFileDialog::getSaveFileName(this, tr("X3g Output File"),
        sDefaultFileName, tr("X3g (*.x3g *.s3g)"));
    if (sFileName.isEmpty())
        return; // user cancelled

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

void MainWindow::refreshFromIni(IniEditor &ie)
{
    // main
    Section sect = ie.section("printer");
    int imt = ui->comboMachineType->findData(sect.getValue("machine_type", "r2"));
    if (imt != -1)
        ui->comboMachineType->setCurrentIndex(imt);
    QString s = sect.getValue("reprap_flavor", "reprap");
    ui->comboGcodeFlavor->setCurrentIndex(s.compare("makerbot", Qt::CaseInsensitive) == 0);

    // advanced
    ui->cboxProgress->setChecked(sect.getValue("build_progress", "1").toInt());
    ui->cboxDitto->setChecked(sect.getValue("ditto_printing", "0").toInt());
    ui->cboxRecalc->setChecked(sect.getValue("recalculate_5d", "0").toInt());
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
    "Portions copyright (c) 2009, Ben Hoyt, All rights reserved.\n\n"

    "This program is free software; you can redistribute it and/or modify "
    "it under the terms of the GNU General Public License as published by "
    "the Free Software Foundation; either version 2 of the License, or "
    "(at your option) any later version.\n\n"

    "This program is distributed in the hope that it will be useful, "
    "but WITHOUT ANY WARRANTY; without even the implied warranty of "
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
    "GNU General Public License for more details.\n\n"

    "Please see additional license information in included documentation in"
    "the same folder as this program."
    );
}
