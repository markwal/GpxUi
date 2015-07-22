#include "logmodel.h"
#include "rungpx.h"
#include "ui_rungpx.h"

#include <QProcess>
#include <QMessageBox>
#include <QPushButton>

RunGpx::RunGpx(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RunGpx)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    lm = new LogModel(this);
    ui->lvLog->setModel(lm);

    process = new QProcess(this);
    process->setProcessChannelMode(QProcess::MergedChannels);
    connect(process, SIGNAL(readyReadStandardOutput()), SLOT(ReadStdOut()));
    connect(process, SIGNAL(finished(int)), SLOT(Finished(int)));
}

RunGpx::~RunGpx()
{
    if (process->state() == QProcess::Running) {
        process->terminate();
        process->waitForFinished(3000);
    }
    delete ui;
}

void RunGpx::ReadStdOut()
{
    char rgb[1024];

    forever {
        qint64 cb = process->readLine(rgb, sizeof(rgb));
        if (cb <= 0)
            break;
        lm->append(QLatin1String(rgb));
        QApplication::processEvents();
    }
}

void RunGpx::Translate(const QString& sInputName, const QString& sOutputName)
{
    QString sApp = QLatin1String("gpx.exe");
    QStringList slArgs;
    slArgs << QLatin1String("-v") << sInputName << sOutputName;

    *lm << tr("testing run on... ") << tr("Running GPX...\n") << QString("%1 %2\n").arg(sApp, slArgs.join(" "));

    process->start(sApp, slArgs);
    if (!process->waitForStarted()) {
        QMessageBox::critical(this, tr("Failure to launch GPX"),
            tr("Could not start %1 with arguments: %2.").arg(sApp, slArgs.join(" ")));
    }
    else
        exec();
}

void RunGpx::Finished(int ec)
{
    *lm << tr("GPX finished with code = %1.\n").arg(ec);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}
