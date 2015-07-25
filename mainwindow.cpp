#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "rungpx.h"

#include <QFileDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    heightCollapsed = 0;
    ui->setupUi(this);
    setAdvanced(false);
    resize(sizeHint());
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
    ui->editInput->setText(QFileDialog::getOpenFileName());
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
