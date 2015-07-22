#ifndef RUNGPX_H
#define RUNGPX_H

#include <QDialog>

class QProcess;

namespace Ui {
class RunGpx;
}

class LogModel;

class RunGpx : public QDialog
{
    Q_OBJECT

public:
    explicit RunGpx(QWidget *parent = 0);
    ~RunGpx();

    void Translate(const QString& sInputName, const QString& sOutputName);

public slots:
    void ReadStdOut();
    void Finished(int ec);

private:
    Ui::RunGpx *ui;
    QProcess *process;
    LogModel *lm;
};

#endif // RUNGPX_H
