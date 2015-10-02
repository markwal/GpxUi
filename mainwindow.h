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
    void setInputName(QString s);

private slots:
    void on_btnAdvancedToggle_clicked();
    void on_tbtnInputGcode_clicked();
    void on_tbMachineEditor_clicked();
    void on_btnTranslate_clicked();
    void on_about();
    void on_btnDefaults_clicked();
    void on_btnReload_clicked();
    void on_btnSave_clicked();
    void on_action_Updates_triggered();

private:
    Ui::MainWindow *ui;
    int heightCollapsed;
    IniEditor ie;
    QString sSectionCur;

    bool chooseInputFile();
    void refreshFromIni();
    void saveToIni();
    void populateMachineType(QComboBox &combo);
    static bool iniEditorParserCallback(void *user, QString &sSection, Line &line);
};

#endif // MAINWINDOW_H
