
#include "main.h"
#include "mainwindow.h"
#include "iniedit.h"

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

static void runUpdate(const QString sParam, bool fWait)
{
    QDir dir(QApplication::instance()->applicationDirPath());
    dir.cdUp();
    QString sApp = dir.filePath("Update.exe");
    QStringList sArgs;
    sArgs << sParam;

    if (fWait) {
        QProcess process;
        process.start(sApp, sArgs);
        process.waitForFinished(5000);
    }
    else {
        QProcess::startDetached(dir.filePath("Update.exe"), sArgs);
    }
}

static void setPathForAppDir(bool fRemove)
{
    QCoreApplication &a = *QApplication::instance();
    QDir dir(a.applicationDirPath());
    dir.cdUp();

    QSettings settings("HKEY_CURRENT_USER\\Environment", QSettings::NativeFormat);
    QString sPath = settings.value("PATH", QString()).toString();

    if (fRemove) {
        sPath = sPath.remove(QString(";") + a.applicationDirPath());
        sPath = sPath.remove(a.applicationDirPath());
        if (sPath.isEmpty())
            settings.remove("PATH");
        else
            settings.setValue("PATH", sPath);
    }
    else {
        if (sPath.isEmpty())
            settings.setValue("PATH", a.applicationDirPath());
        else if (!sPath.contains(a.applicationDirPath()))
            settings.setValue("PATH", a.applicationDirPath() + ";" + sPath);
        else
            return;
    }

    SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"Environment", SMTO_ABORTIFHUNG, 500, NULL);
    SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)"Environment", SMTO_ABORTIFHUNG, 500, NULL);
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
            setPathForAppDir(false);
            runUpdate("--createShortcut=GpxUi.exe", false);
            return 0;
        }
        else if(clp.isSet(cloUpdated)) {
            QDir dirSrc = GpxUiInfo::iniLocation();
            QDir dirDest = dirSrc;
            dirSrc.cdUp();

            QStringList slFilters;
            slFilters << "*.ini";

            QStringList sl = dirSrc.entryList(slFilters);
            int is = sl.size();
            while (is--) {
                QFile::copy(dirSrc.absoluteFilePath(sl[is]), dirDest.absoluteFilePath(sl[is]));
            }

            setPathForAppDir(false);
            return 0;
        }
        else if (clp.isSet(cloObsolete)) {
            setPathForAppDir(true);
            return 0;
        }
        else if (clp.isSet(cloUninstall)) {
            runUpdate("--removeShortcut=GpxUi.exe", true);
            setPathForAppDir(true);
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
