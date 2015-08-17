#include "main.h"
#include "machineeditor.h"
#include "ui_machineeditor.h"

#include <QDir>
#include <QFileInfo>
#include <QPushButton>

MachineEditor::MachineEditor(QWidget *parent, const QString &sMachineId) :
    QDialog(parent),
    ui(new Ui::MachineEditor),
    sMachineId(sMachineId)
{
    ui->setupUi(this);
    connect(ui->buttonBox->button(QDialogButtonBox::RestoreDefaults), SIGNAL(clicked()), SLOT(onRestoreDefaultsClicked()));

    const QDir &dir = GpxUiInfo::machineIniLocation();
    QString sFilename = sMachineId;
    sFilename.append(".ini");
    QFileInfo fi(dir.absoluteFilePath(sFilename));
    ie.setFilename(fi.absoluteFilePath());
    if (fi.exists())
        ie.read(NULL, NULL);
    else
        ie.clear();

    refreshFromMachineIni();
}

MachineEditor::~MachineEditor()
{
    delete ui;
}

// Machine definitions
#define MACHINE_ARRAY
#define MACHINE_ALIAS_ARRAY
#include "GPX/src/shared/std_machines.h"
#include "GPX/src/shared/classic_machines.h"

#define MACHINE_IS(m) strcasecmp(machine, m) == 0

Machine *gpx_find_machine(const char *machine)
{
    Machine **all_machines[] = {machines, wrong_machines, NULL};
    Machine ***ptr_all;
    Machine **ptr;

    // check the aliases
    MachineAlias **pma;
    for (pma = machine_aliases; *pma; pma++) {
        if (MACHINE_IS((*pma)->alias)) {
            machine = (*pma)->type;
            break;
        }
    }

    // check the machine list
    for (ptr_all = all_machines; *ptr_all != NULL; ptr_all++) {
        for (ptr = *ptr_all; *ptr; ptr++) {
            if(MACHINE_IS((*ptr)->type))
                return *ptr;
        }
    }

    // Machine not found
    return NULL;
}

static const Machine machineEmpty = {
    "", "Empty machine definition", 0, 0,
    {0, 0, 0, 0, 0, 0, 1}, // x axis
    {0, 0, 0, 0, 0, 0, 1}, // y axis
    {0, 0, 0, 0, 0, 0, 0}, // x axis
    {0, 0, 0, 0, 0, 1}, // a extruder
    {0, 0, 0, 0, 0, 1}, // b extruder
    1.75, // nominal filament diameter
    0.85, // nominal packing density
    0.4, // nozzle diameter
    {0, 0, 0}, // toolhead offsets
    {0.0070, 0.0040},  // JKN
    1,  // extruder count
    20, // timeout
    MACHINE_TYPE_NONE
};

void MachineEditor::refreshFromMachineIni()
{
    const Machine *pmachine = gpx_find_machine(sMachineId.toLatin1().constData());
    if (pmachine == NULL)
        pmachine = &machineEmpty;

    Section *psect = &ie.section("machine");
    ui->dsbNozzleDiameter->setValue(psect->value("nozzle_diameter", pmachine->nozzle_diameter));
    ui->sbExtruderCount->setValue(psect->intValue("extruder_count", pmachine->extruder_count));
    ui->sbTimeout->setValue(psect->intValue("timeout", pmachine->timeout));

    // X axis
    psect = &ie.section("x");
    ui->dsbXStepsPer->setValue(psect->value("steps_per_mm", pmachine->x.steps_per_mm));
    ui->dsbXMaxFeedrate->setValue(psect->intValue("max_feedrate", pmachine->x.max_feedrate));
    ui->dsbXHomeFeedrate->setValue(psect->intValue("home_feedrate", pmachine->x.home_feedrate));
    ui->cbXHome->setCurrentIndex(psect->intValue("endstop", pmachine->x.endstop));

    // Y axis
    psect = &ie.section("y");
    ui->dsbYStepsPer->setValue(psect->value("steps_per_mm", pmachine->y.steps_per_mm));
    ui->dsbYMaxFeedrate->setValue(psect->intValue("max_feedrate", pmachine->y.max_feedrate));
    ui->dsbYHomeFeedrate->setValue(psect->intValue("home_feedrate", pmachine->y.home_feedrate));
    ui->cbYHome->setCurrentIndex(psect->intValue("endstop", pmachine->x.endstop));

    // X axis
    psect = &ie.section("z");
    ui->dsbZStepsPer->setValue(psect->value("steps_per_mm", pmachine->z.steps_per_mm));
    ui->dsbZMaxFeedrate->setValue(psect->intValue("max_feedrate", pmachine->z.max_feedrate));
    ui->dsbZHomeFeedrate->setValue(psect->intValue("home_feedrate", pmachine->z.home_feedrate));
    ui->cbZHome->setCurrentIndex(psect->intValue("endstop", pmachine->x.endstop));

    // A axis
    psect = &ie.section("a");
    ui->dsbAStepsPer->setValue(psect->value("steps_per_mm", pmachine->a.steps_per_mm));
    ui->dsbAMaxFeedrate->setValue(psect->intValue("max_feedrate", pmachine->a.max_feedrate));
    ui->cboxHasHBP->setChecked(psect->intValue("has_heated_build_platform", pmachine->a.has_heated_build_platform));

    // B axis
    psect = &ie.section("b");
    ui->dsbBStepsPer->setValue(psect->value("steps_per_mm", pmachine->b.steps_per_mm));
    ui->dsbBMaxFeedrate->setValue(psect->intValue("max_feedrate", pmachine->b.max_feedrate));
}

void MachineEditor::saveToIni()
{
    const Machine *pmachine = gpx_find_machine(sMachineId.toLatin1().constData());
    if (pmachine == NULL)
        pmachine = &machineEmpty;

    Section *psect = &ie.section("machine");
    psect->setValue("nozzle_diameter", ui->dsbNozzleDiameter->value(), pmachine->nozzle_diameter);
    psect->setValue("extruder_count", ui->sbExtruderCount->value(), pmachine->extruder_count);
    psect->setValue("timeout", ui->sbTimeout->value(), pmachine->timeout);

    // X axis
    psect = &ie.section("x");
    psect->setValue("steps_per_mm", ui->dsbXStepsPer->value(), pmachine->x.steps_per_mm, 6);
    psect->setValue("max_feedrate", ui->dsbXMaxFeedrate->value(), pmachine->x.max_feedrate, 0);
    psect->setValue("home_feedrate", ui->dsbXHomeFeedrate->value(), pmachine->x.home_feedrate, 0);
    psect->setValue("endstop", ui->cbXHome->currentIndex(), pmachine->x.endstop);

    // Y axis
    psect = &ie.section("y");
    psect->setValue("steps_per_mm", ui->dsbYStepsPer->value(), pmachine->y.steps_per_mm, 6);
    psect->setValue("max_feedrate", ui->dsbYMaxFeedrate->value(), pmachine->y.max_feedrate, 0);
    psect->setValue("home_feedrate", ui->dsbYHomeFeedrate->value(), pmachine->y.home_feedrate, 0);
    psect->setValue("endstop", ui->cbYHome->currentIndex(), pmachine->x.endstop);

    // X axis
    psect = &ie.section("z");
    psect->setValue("steps_per_mm", ui->dsbZStepsPer->value(), pmachine->z.steps_per_mm, 6);
    psect->setValue("max_feedrate", ui->dsbZMaxFeedrate->value(), pmachine->z.max_feedrate, 0);
    psect->setValue("home_feedrate", ui->dsbZHomeFeedrate->value(), pmachine->z.home_feedrate, 0);
    psect->setValue("endstop", ui->cbZHome->currentIndex(), pmachine->x.endstop);

    // A axis
    psect = &ie.section("a");
    psect->setValue("steps_per_mm", ui->dsbAStepsPer->value(), pmachine->a.steps_per_mm, 31);
    psect->setValue("max_feedrate", ui->dsbAMaxFeedrate->value(), pmachine->a.max_feedrate, 0);
    psect->setValue("has_heated_build_platform", ui->cboxHasHBP->isChecked(), pmachine->a.has_heated_build_platform);

    // B axis
    psect = &ie.section("b");
    psect->setValue("steps_per_mm", ui->dsbBStepsPer->value(), pmachine->b.steps_per_mm, 31);
    psect->setValue("max_feedrate", ui->dsbBMaxFeedrate->value(), pmachine->b.max_feedrate, 0);

    ie.write();
}

void MachineEditor::onRestoreDefaultsClicked()
{
    const Machine *pmachine = gpx_find_machine(sMachineId.toLatin1().constData());
    if (pmachine == NULL)
        pmachine = &machineEmpty;

    ui->dsbNozzleDiameter->setValue(pmachine->nozzle_diameter);
    ui->sbExtruderCount->setValue(pmachine->extruder_count);
    ui->sbTimeout->setValue(pmachine->timeout);

    // X axis
    ui->dsbXStepsPer->setValue(pmachine->x.steps_per_mm);
    ui->dsbXMaxFeedrate->setValue(pmachine->x.max_feedrate);
    ui->dsbXHomeFeedrate->setValue(pmachine->x.home_feedrate);
    ui->cbXHome->setCurrentIndex(pmachine->x.endstop);

    // Y axis
    ui->dsbYStepsPer->setValue(pmachine->y.steps_per_mm);
    ui->dsbYMaxFeedrate->setValue(pmachine->y.max_feedrate);
    ui->dsbYHomeFeedrate->setValue(pmachine->y.home_feedrate);
    ui->cbYHome->setCurrentIndex(pmachine->x.endstop);

    // X axis
    ui->dsbZStepsPer->setValue(pmachine->z.steps_per_mm);
    ui->dsbZMaxFeedrate->setValue(pmachine->z.max_feedrate);
    ui->dsbZHomeFeedrate->setValue(pmachine->z.home_feedrate);
    ui->cbZHome->setCurrentIndex(pmachine->x.endstop);

    // A axis
    ui->dsbAStepsPer->setValue(pmachine->a.steps_per_mm);
    ui->dsbAMaxFeedrate->setValue(pmachine->a.max_feedrate);
    ui->cboxHasHBP->setChecked(pmachine->a.has_heated_build_platform);

    // B axis
    ui->dsbBStepsPer->setValue(pmachine->b.steps_per_mm);
    ui->dsbBMaxFeedrate->setValue(pmachine->b.max_feedrate);
}

void MachineEditor::on_sbExtruderCount_valueChanged(int cextruders)
{
   if (cextruders >= 2)
       ui->widgetB->show();
   else
       ui->widgetB->hide();
}

void MachineEditor::on_buttonBox_accepted()
{
    saveToIni();
}
