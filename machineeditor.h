#ifndef MACHINEEDITOR_H
#define MACHINEEDITOR_H

#include <QDialog>
#include "iniedit.h"

class QComboBox;

namespace Ui {
class MachineEditor;
}

class MachineEditor : public QDialog
{
    Q_OBJECT

public:
    explicit MachineEditor(QWidget *parent, const QString &sMachineId);
    ~MachineEditor();

private slots:
    void onRestoreDefaultsClicked();
    void on_sbExtruderCount_valueChanged(int arg1);

    void on_buttonBox_accepted();

private:
    Ui::MachineEditor *ui;
    QString sMachineId;
    IniEditor ie;
    void populateEndstop(QComboBox &cb);
    void refreshFromMachineIni();
    void saveToIni();
};

#endif // MACHINEEDITOR_H
