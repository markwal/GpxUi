#ifndef UPDATES_H
#define UPDATES_H

#include <QDialog>

namespace Ui {
class Updates;
}

class QProcess;

class Updates : public QDialog
{
    Q_OBJECT

public:
    explicit Updates(QWidget *parent = 0);
    ~Updates();
    static void autoUpdate();

private slots:
    void on_buttonBox_accepted();
    void on_pbUpdateNow_clicked();
    void readStdout();
    void finished(int ec);

private:
    Ui::Updates *ui;
    QProcess *pprocess;
    bool fUpdating;

    void checkForUpdates();
    typedef enum {Check, Update, UpdateDetached} UpdateAction;
    static bool runUpdateExe(QProcess *, UpdateAction ua);
    void doAction(UpdateAction);
};

#endif // UPDATES_H
