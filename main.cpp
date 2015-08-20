
#include "main.h"
#include "mainwindow.h"
#include "iniedit.h"

#include <QApplication>
#include <QtGui>
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

static void runUpdate(const QString sParam)
{
    QDir dir(QApplication::instance()->applicationDirPath());
    dir.cdUp();
    QStringList sArgs;
    sArgs << sParam;
    QProcess::startDetached(dir.filePath("Update.exe"), sArgs);
}

static void addAppDirToPath()
{
    QCoreApplication &a = *QApplication::instance();
    QDir dir(a.applicationDirPath());
    dir.cdUp();

    QSettings settings("HKEY_CURRENT_USER\\Environment", QSettings::NativeFormat);
    QString sPath = settings.value("PATH", QString()).toString();
    if (sPath.isEmpty())
        settings.setValue("PATH", a.applicationDirPath());
    else if (!sPath.contains(a.applicationDirPath()))
        settings.setValue("PATH", sPath + ";" + a.applicationDirPath());
    PostMessage(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"Environment");
    PostMessage(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)"Environment");
}

static void removeAppDirFromPath()
{
    QCoreApplication &a = *QApplication::instance();
    QDir dir(a.applicationDirPath());
    dir.cdUp();

    QSettings settings("HKEY_CURRENT_USER\\Environment", QSettings::NativeFormat);
    QString sPath = settings.value("PATH", QString()).toString();
    sPath = sPath.remove(QString(";") + a.applicationDirPath());
    sPath = sPath.remove(a.applicationDirPath());
    if (sPath.isEmpty())
        settings.remove("PATH");
    else
        settings.setValue("PATH", sPath);
    PostMessage(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"Environment");
    PostMessage(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)"Environment");
}

int main(int argc, char *argv[])
{
    QApplication::setStyle("Fusion");
    QApplication a(argc, argv);

    a.setOrganizationName("MarkWal");
    a.setOrganizationDomain("markwal.github.io");
    a.setApplicationName("GpxUi");
    GpxUiInfo::init();

    if (argc > 1) {
        QCommandLineParser clp;
        clp.setApplicationDescription("GpxUi is a graphical user interface for running GPX, a command line utility that converts .gcode to .x3g");
        clp.addHelpOption();
        clp.addVersionOption();
        QCommandLineOption cloInstall("squirrel-install", "Perform squirrel post install action", "version");
        clp.addOption(cloInstall);
        QCommandLineOption cloFirstRun("squirrel-firstrun", "First run after squirrel install");
        clp.addOption(cloFirstRun);
        QCommandLineOption cloUpdated("squirrel-updated", "Perform squirrel post update action", "version");
        clp.addOption(cloUpdated);
        QCommandLineOption cloObsolete("squirrel-obsolete", "Perform squirrel post update cleanup action", "version");
        clp.addOption(cloObsolete);
        QCommandLineOption cloUninstall("squirrel-uninstall", "Perform squirrel pre uninstall action", "version");
        clp.addOption(cloUninstall);
        clp.process(a);

        if (clp.isSet(cloInstall)) {
            addAppDirToPath();
            runUpdate("--createShortcut=GpxUi.exe");
            return 0;
        }
        else if(clp.isSet(cloUpdated)) {
            // TODO copy *.ini from the obsolete folder
            addAppDirToPath();
            return 0;
        }
        else if (clp.isSet(cloObsolete)) {
            removeAppDirFromPath();
            return 0;
        }
        else if (clp.isSet(cloUninstall)) {
            removeAppDirFromPath();
            runUpdate("--removeShortcut=GpxUi.exe");
            return 0;
        }
        else if (clp.isSet(cloFirstRun)) {
        }
        else {
            clp.showHelp();
            Q_UNREACHABLE();
            return 0;
        }
    }

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
