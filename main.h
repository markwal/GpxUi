#ifndef MAIN_H
#define MAIN_H

#include <QtGlobal>
#include <QCommandLineOption>

class QTextStream;
QTextStream &qStdout();

// Ini file goes where GPX expects to find it
#ifdef Q_OS_WIN
#define INIFILENAME_PREFIX ""
#else
#define INIFILENAME_PREFIX "."
#endif

class QDir;
class QCommandLineParser;
class IniEditor;

namespace GpxUiInfo
{
void init();
const QDir &iniLocation();
const QDir &machineIniLocation();
};

class SetupEvents
{
public:
#ifdef Q_OS_WIN
    void addOptions(QCommandLineParser &);
    bool handleOptions(QCommandLineParser &);

    SetupEvents() :
        cloInstall("squirrel-install", "Perform squirrel post install action", "version"),
        cloFirstRun("squirrel-firstrun", "First run after squirrel install"),
        cloUpdated("squirrel-updated", "Perform squirrel post update action", "version"),
        cloObsolete("squirrel-obsolete", "Perform squirrel post update cleanup action", "version"),
        cloUninstall("squirrel-uninstall", "Perform squirrel pre uninstall action", "version") {}

    QCommandLineOption cloInstall;
    QCommandLineOption cloFirstRun;
    QCommandLineOption cloUpdated;
    QCommandLineOption cloObsolete;
    QCommandLineOption cloUninstall;
#else
    void AddOptions(QCommandLineParser &clp) {}
    bool handleOptions(QCommandLineParser &clp) {return true;}
#endif // Q_OS_WIN
};

#ifdef Q_OS_WIN
bool CopyToVersionIndependentLocation(IniEditor &);
#else
#define CopyToVersionIndependentLocation(ie) (true)
#endif // !Q_OS_WIN

#endif // MAIN_H

