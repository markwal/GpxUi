#ifndef RUNGPX_H
#define RUNGPX_H

#include <QDialog>
#include <QTextCursor>

class QProcess;
class QTimer;

namespace Ui {
class RunGpx;
}

class RunGpx : public QDialog
{
    Q_OBJECT

public:
    explicit RunGpx(QWidget *parent = 0);
    ~RunGpx();

    void Translate(const QString& sInputName, const QString& sOutputName);

public slots:
    void ReadStdOut();
    void DelayReadStdOut();
    void Finished(int ec);

private:
    Ui::RunGpx *ui;
    QTextCursor tc;
    QProcess *pprocess;
    QTimer *ptimer;
};

#endif // RUNGPX_H
