#include "updates.h"
#include "ui_updates.h"

#include <QSettings>
#include <QDir>
#include <QProcess>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>

// ../Update.exe --download=https://markwal.github.io/GpxUi
// ../Update.exe --update=https://markwal.github.io/GpxUi
Updates::Updates(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Updates)
{
    ui->setupUi(this);

    QSettings settings;
    ui->cboxAutoUpdate->setChecked(settings.value("auto_update", true).toBool());

    pprocess = new QProcess(this);
    pprocess->setProcessChannelMode(QProcess::MergedChannels);
    connect(pprocess, SIGNAL(readyRead()), SLOT(readStdout()));
    connect(pprocess, SIGNAL(finished(int)), SLOT(finished(int)));

    checkForUpdates();
}

Updates::~Updates()
{
    if (pprocess->state() == QProcess::Running) {
        pprocess->terminate();
        pprocess->waitForFinished(3000);
    }
    delete pprocess;
    pprocess = NULL;
    delete ui;
}

bool Updates::runUpdateExe(QProcess *pprocess, UpdateAction ua)
{
    QDir dir(QApplication::instance()->applicationDirPath());
    dir.cdUp();
    QString sApp = dir.absoluteFilePath("Update.exe");
    QStringList slArgs;
    if (ua == Check)
        slArgs << QLatin1String("--download=https://markwal.github.io/GpxUi");
    else
        slArgs << QLatin1String("--update=https://markwal.github.io/GpxUi");
    if (ua == UpdateDetached) {
        return pprocess->startDetached(sApp, slArgs);
    }
    pprocess->start(sApp, slArgs);
    return pprocess->waitForStarted();
}

void Updates::doAction(UpdateAction ua)
{
    if (pprocess->state() == QProcess::Running) {
        return; // one at a time
    }

    fUpdating = (ua == Update);
    if (!runUpdateExe(pprocess, ua))
        ui->textStatus->setText("Unable to check for updates.");
    else if (fUpdating)
        ui->textStatus->setText("Installing latest version.");
    else
        ui->textStatus->setText("Checking for available updates.");
}

void Updates::checkForUpdates()
{
    doAction(Check);
}

void Updates::readStdout()
{
    QByteArray rgb = pprocess->readLine();
    if (rgb.isEmpty())
        return;
    if (QChar(rgb[0]).isDigit()) {
        int percent = QString(rgb).toInt();
        if (percent >= 0 || percent <= 100)
            ui->progressBar->setValue(percent);
    }
    else if (rgb[0] == '{') {
        ui->progressBar->setValue(0); // back to 0 to prepare for "Update Now"
        QJsonDocument jd(QJsonDocument::fromJson(rgb));
        const QJsonObject &json = jd.object();
        ui->textCurrent->setText(json["currentVersion"].toString());
        ui->textAvailable->setText(json["futureVersion"].toString());
        ui->textStatus->setText("");
    }
}

void Updates::autoUpdate()
{
    QSettings settings;
    if (!settings.value("auto_update", true).toBool())
        return;
    QDateTime dt = settings.value("last_update_check", QDateTime()).toDateTime();
    QDateTime dtNow = QDateTime::currentDateTimeUtc();
    qint64 cdays = dt.daysTo(dtNow);
    if (dt.isNull() || cdays > 0) {
        settings.setValue("last_update_check", dtNow);
        QProcess process;
        runUpdateExe(&process, UpdateDetached);
    }
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
void Updates::finished(int ec)
{
    if (fUpdating) {
        ui->progressBar->setValue(100);
        ui->textStatus->setText("New version installed for the next start of GpxUi.");
    }
}
#pragma GCC diagnostic pop

void Updates::on_buttonBox_accepted()
{
    QSettings settings;

    settings.setValue("auto_update", ui->cboxAutoUpdate->isChecked());
}

void Updates::on_pbUpdateNow_clicked()
{
   doAction(Update);
}
