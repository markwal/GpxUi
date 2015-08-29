
#include "main.h"
#include "mainwindow.h"
#include "iniedit.h"
#include "build/version.h"

#include <QApplication>
#include <QtGui>
#include <QTextStream>
#include <QStandardPaths>
#include <QMessageBox>

QTextStream &qStdout()
{
    static QTextStream ts(stdout);
    return ts;
}

namespace GpxUiInfo
{
    static QDir dirIni;
    static QDir dirMachineIni;

    void init()
    {
#ifdef Q_OS_WIN
        dirIni.setPath(QApplication::instance()->applicationDirPath());
        dirMachineIni = dirIni;
#else
        dirIni.setPath(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
        dirMachineIni = dirIni;
        const char *sz = ".gpx";
        if (!dirMachineIni.cd(sz)) {
            dirMachineIni.mkdir(sz);
            dirMachineIni.cd(sz);
        }
#endif
    }

    const QDir &iniLocation()
    {
        return dirIni;
    }

    const QDir &machineIniLocation()
    {
        return dirMachineIni;
    }
}

int main(int argc, char *argv[])
{
    QApplication::setStyle("Fusion");
    QApplication a(argc, argv);

    a.setOrganizationName("MarkWal");
    a.setOrganizationDomain("markwal.github.io");
    a.setApplicationName("GpxUi " GPXUI_VERSION);
    GpxUiInfo::init();

    QCommandLineParser clp;
    clp.setApplicationDescription("GpxUi is a graphical user interface for running GPX, a command line utility that converts .gcode to .x3g");
    clp.addHelpOption();
    clp.addVersionOption();
    clp.addPositionalArgument("gcodefile", "Gcode file to translate to x3g");

    SetupEvents se;
    se.addOptions(clp);

    clp.process(a);
    const QStringList rgsArgs = clp.positionalArguments();

    if (se.handleOptions(clp))
        return 0;

/*
    QString szPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(szPath);
    if (!szPath.isEmpty())
    {
        Slic3r s;
        dir.cdUp();
        dir.cd("slic3r/print");
        QFile file(dir.filePath("medium.ini"));
        QSettings settings(file.fileName(), QSettings::IniFormat);
        s.append(file.fileName().toLatin1().data());
        foreach (const QString &sKey, settings.childKeys()) {
            s.append(QString("%1 = %2").arg(sKey, settings.value(sKey).toString()));
            qDebug() << sKey;
            qDebug() << settings.value(sKey);
        }
        s.exec();
    }
*/
    MainWindow w;
    w.show();

    if (rgsArgs.size() > 0)
        w.setInputName(rgsArgs.at(0));

    return a.exec();
}
