#include "main.h"
#include "mainwindow.h"
#include "iniedit.h"

#include <QApplication>
#include <QtGui>
#include <QSplashScreen>
#include <QTextStream>
#include <QStandardPaths>

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
        dirIniLocation.setPath(QStandardPaths::HomeLocation());
        dirMachineIniLocation = dirIniLocation;
        const char *sz = ".gpx";
        if (!dirMachineIniLocation.cd(sz)) {
            dirMachineIniLocation.mkdir(sz);
            dirMachineIniLocation.cd(sz);
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
    a.setApplicationName("GpxUi");
    GpxUiInfo::init();

//    QSplashScreen splash;
//    splash.show();
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

    return a.exec();
}
