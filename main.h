#ifndef MAIN_H
#define MAIN_H

#include <QtGlobal>

class QTextStream;
QTextStream &qStdout();

// Ini file goes where GPX expects to find it
#ifdef Q_OS_WIN
#define INIFILENAME_PREFIX ""
#else
#define INIFILENAME_PREFIX "."
#endif

class QDir;
class IniEditor;

namespace GpxUiInfo
{
void init();
const QDir &iniLocation();
const QDir &machineIniLocation();
bool copyToVersionIndependentLocation(IniEditor &ie);
};


#endif // MAIN_H

