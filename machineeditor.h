#ifndef MACHINEEDITOR_H
#define MACHINEEDITOR_H

#include <QDialog>

namespace Ui {
class MachineEditor;
}

class MachineEditor : public QDialog
{
    Q_OBJECT

public:
    explicit MachineEditor(QWidget *parent = 0);
    ~MachineEditor();

private:
    Ui::MachineEditor *ui;
};

#endif // MACHINEEDITOR_H
