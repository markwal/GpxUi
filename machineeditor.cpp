#include "machineeditor.h"
#include "ui_machineeditor.h"

MachineEditor::MachineEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MachineEditor)
{
    ui->setupUi(this);
}

MachineEditor::~MachineEditor()
{
    delete ui;
}
