#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "iniedit.h"

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class QComboBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setAdvanced(bool isAdvanced);

private slots:
    void on_btnAdvancedToggle_clicked();
    void on_tbtnInputGcode_clicked();
    void on_tbMachineEditor_clicked();
    void on_btnTranslate_clicked();
    void on_about();
    void refreshFromIni(IniEditor &ie);
    void populateMachineType(QComboBox &combo);

private:
    Ui::MainWindow *ui;
    int heightCollapsed;
    IniEditor ie;
    QString sSectionCur;

    static bool iniEditorParserCallback(void *user, QString &sSection, Line &line);
};

#endif // MAINWINDOW_H
