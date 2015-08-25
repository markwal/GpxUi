
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

#ifdef Q_OS_WIN
    bool copyToVersionIndependentLocation(IniEditor &ie)
    {
        // Windows gpx.exe assumes gpx.ini goes in the .exe folder, we put the
        // .exe in a folder for its version so we can upgrade while it's running
        // So here, we copy it up a folder so upgrade can get it, but only if
        // we appear to be in the squirrel folder (../Update.exe exists).
        QDir dir(ie.fileName());
        dir.cdUp();
        QFileInfo fi(dir.absoluteFilePath("Update.exe"));
        return fi.exists() ? ie.copyTo(dir) : true;
    }
#else
    bool copyToVersionIndependentLocation(IniEditor &ie)
    {
        // already there
        return true;
    }
#endif
}

#ifdef Q_OS_WIN
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

static void registerGcodeExtension(void)
{
    QDir dir(QApplication::instance()->applicationDirPath());
    QString sGpxUiCommand = "\"" % dir.absoluteFilePath("GpxUi.exe").replace('/', '\\') % "\" \"%1\"";
    QString sGpxCommand = "\"" % dir.absoluteFilePath("gpx.exe").replace('/', '\\') % "\" \"%1\"";
    QSettings settings("HKEY_CURRENT_USER\\Software\\Classes\\", QSettings::NativeFormat);
#ifdef PERHAPSFOROLDVERSIONS
    settings.setValue(".gcode/shell/gpxui/command/Default", sGpxUiCommand);
    settings.setValue(".gcode/shell/gpxui/Default", "Open with GpxUi");
    settings.setValue(".gcode/shell/gpxconvert/command/Default", sGpxCommand);
    settings.setValue(".gcode/shell/gpxconvert/Default", "GPX convert to x3g");
#endif // PERHAPSFOROLDVERSIONS
    settings.setValue(".gcode/OpenWithProgids/gpx.gcode.1", "");
    settings.setValue("gpx.gcode.1/shell/open/command/Default", sGpxUiCommand);
    settings.setValue("gpx.gcode.1/Default", "GPX gcode file");
    settings.setValue("gpx.gcode.1/shell/gpxconvert/command/Default", sGpxCommand);
    settings.setValue("gpx.gcode.1/shell/gpxconvert/Default", "GPX convert to x3g");
}

static void unregisterGcodeExtension(void)
{
    QSettings settings("HKEY_CURRENT_USER\\Software\\Classes\\", QSettings::NativeFormat);
    settings.remove(".gcode/OpenWithProgids/gpx.gcode.1");
    settings.remove("gpx.gcode.1");
}
#endif // Q_OS_WIN

int main(int argc, char *argv[])
{
    QApplication::setStyle("Fusion");
    QApplication a(argc, argv);

    a.setOrganizationName("MarkWal");
    a.setOrganizationDomain("markwal.github.io");
    a.setApplicationName("GpxUi");
    GpxUiInfo::init();

    QCommandLineParser clp;
    clp.setApplicationDescription("GpxUi is a graphical user interface for running GPX, a command line utility that converts .gcode to .x3g");
    clp.addHelpOption();
    clp.addVersionOption();
    clp.addPositionalArgument("gcodefile", "Gcode file to translate to x3g");

#ifdef Q_OS_WIN
// Squirrel for Windows event handlers
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
#endif // Q_OS_WIN
    clp.process(a);
    const QStringList rgsArgs = clp.positionalArguments();

#ifdef Q_OS_WIN
    if (clp.isSet(cloInstall)) {
        setPathForAppDir(false);
        runUpdate("--createShortcut=GpxUi.exe", false);
        registerGcodeExtension();
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
        registerGcodeExtension();
        return 0;
    }
    else if (clp.isSet(cloObsolete)) {
        setPathForAppDir(true);
        return 0;
    }
    else if (clp.isSet(cloUninstall)) {
        runUpdate("--removeShortcut=GpxUi.exe", true);
        setPathForAppDir(true);
        unregisterGcodeExtension();
        return 0;
    }
    else if (clp.isSet(cloFirstRun)) {
    }
#endif // Q_OS_WIN

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
