#include <windows.h>

#include "main.h"
#include "iniedit.h"

#include <QApplication>
#include <QSettings>
#include <QProcess>
#include <QDir>
#include <QStringBuilder>
#include <QCommandLineParser>

// Returns true if current process prinicipal is a member of Administrators
// Assumes no impersonation and that the current principal has the right to
// open its own process and process token.
// See https://msdn.microsoft.com/en-us/library/windows/desktop/aa376389(v=vs.85).aspx
bool IsUserAdmin(void)
{
    SID_IDENTIFIER_AUTHORITY sia = SECURITY_NT_AUTHORITY;
    PSID psidAdminGroup; 

    if (!AllocateAndInitializeSid(&sia, 2, SECURITY_BUILTIN_DOMAIN_RID,
            DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &psidAdminGroup))
        return false;

    BOOL f = false;
    if (!CheckTokenMembership(NULL, psidAdminGroup, /* out */&f))
        f = false;
    FreeSid(psidAdminGroup); 

    return f;
}

bool CopyToVersionIndependentLocation(IniEditor &ie)
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

void SetupEvents::addOptions(QCommandLineParser &clp)
{
// Squirrel for Windows event handlers
    clp.addOption(cloInstall);
    clp.addOption(cloFirstRun);
    clp.addOption(cloUpdated);
    clp.addOption(cloObsolete);
    clp.addOption(cloUninstall);
}

bool SetupEvents::handleOptions(QCommandLineParser &clp)
{
    if (clp.isSet(cloInstall)) {
        setPathForAppDir(false);
        runUpdate("--createShortcut=GpxUi.exe", false);
        registerGcodeExtension();
        return true;
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
        return true;
    }
    else if (clp.isSet(cloObsolete)) {
        setPathForAppDir(true);
        return true;
    }
    else if (clp.isSet(cloUninstall)) {
        runUpdate("--removeShortcut=GpxUi.exe", true);
        setPathForAppDir(true);
        unregisterGcodeExtension();
        return true;
    }
    else if (clp.isSet(cloFirstRun)) {
    }

    return false;
}

