#include "rungpx.h"
#include "ui_rungpx.h"

#include <QDir>
#include <QProcess>
#include <QMessageBox>
#include <QPushButton>
#include <QTextCursor>
#include <QTimer>

#ifdef Q_OS_WIN
static const QLatin1String sGpxExe("gpx.exe");
#else
static const QLatin1String sGpxExe("gpx");
#endif

RunGpx::RunGpx(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RunGpx)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    tc = ui->teLogOutput->textCursor();
    tc.movePosition(QTextCursor::End);

    pprocess = new QProcess(this);
    pprocess->setProcessChannelMode(QProcess::MergedChannels);
    connect(pprocess, SIGNAL(readyReadStandardOutput()), SLOT(ReadStdOut()));
    connect(pprocess, SIGNAL(finished(int)), SLOT(Finished(int)));

    ptimer = new QTimer(this);
    ptimer->setSingleShot(true);
    connect(ptimer, SIGNAL(timeout()), SLOT(DelayReadStdOut()));
}

RunGpx::~RunGpx()
{
    if (pprocess->state() == QProcess::Running) {
        pprocess->terminate();
        pprocess->waitForFinished(3000);
    }
    delete pprocess;
    pprocess = NULL;
    delete ui;
}

void RunGpx::DelayReadStdOut()
{
    QByteArray rgb = pprocess->read(256000L);
    if (!rgb.isEmpty()) {
        tc.insertText(QLatin1String(rgb));
        ptimer->start(10);
    }
    else if (pprocess->state() != QProcess::Running) {
        tc.insertText(tr("GPX finished with code = 0x%1.\n").arg(pprocess->exitCode(), 0, 16));
    }
    ui->teLogOutput->setTextCursor(tc);
}

void RunGpx::ReadStdOut()
{
    if (!ptimer->isActive())
        ptimer->start(10);
}

void RunGpx::Translate(const QString& sInputName, const QString& sOutputName)
{
    QDir dir(QApplication::instance()->applicationDirPath());
    QString sApp = dir.absoluteFilePath(sGpxExe);
    QStringList slArgs;
    slArgs << QLatin1String("-v") << sInputName << sOutputName;

    tc.insertText(tr("Running GPX...\n%1 %2\n\n").arg(sApp, slArgs.join(" ")));

    pprocess->start(sApp, slArgs);
    if (!pprocess->waitForStarted()) {
        QMessageBox::critical(this, tr("Failure to launch GPX"),
            tr("Could not start %1 with arguments: %2.").arg(sApp, slArgs.join(" ")));
    }
    else
        exec();
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
void RunGpx::Finished(int ec)
{
    ptimer->start(10);  // make sure we've read everything
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}
#pragma GCC diagnostic pop
