#include <errno.h>

#include "iniedit.h"

extern "C" {
#define INI_ALLOW_MULTILINE 0
#define NOTIFY_SECTION_CHANGE 1
#include "inih/ini.h"
}

#include <QString>
#include <QTextStream>
QTextStream &qStdout();

// readline callback for inih, turn a C callback into a C++ method call
char *IniEditor::inihReader(char *szBuffer, int cbBuffer, void *user)
{
    IniEditor *pie = (IniEditor *)user;
    return pie->parserReader(szBuffer, cbBuffer);
}

// parser callback for inih, turn a C callback into a C++ method call
int IniEditor::inihHandler(void *user, const char *szSection, const char *szName, const char *szValue)
{
    IniEditor *pie = (IniEditor *)user;
    return pie->parserHandler(szSection, szName, szValue);
}

char *IniEditor::parserReader(char *szBuffer, int cbBuffer)
{
    if (fileParsing.atEnd())
        return NULL;

    qint64 cb = fileParsing.readLine(szLineParsing = szBuffer, cbBuffer);
    if (cb == 0)
        return NULL;

    ll.append(Line(false, ll.count(), QString(), QString(), szLineParsing));
    return szBuffer;
}

int IniEditor::parserHandler(const char *szSection, const char *szName, const char *szValue)
{
    QString sSection(szSection);
    if (sSectionParsing != sSection)
        sSectionParsing = sSection;
    if (szName[0]) {
        LineIterator iline = ll.end();
        iline--;
        iline->fHasProperty = true;
        QString s(szName);
        iline->sName = s;
        iline->sValue = szValue;
        if (parserCallback != NULL)
            parserCallback(user, sSectionParsing, *iline);
        si[sSectionParsing].insert(iline->sName, iline);
    }
    return true;
}

void IniEditor::clear()
{
    si.clear();
    fe = QFileDevice::NoError;
}

bool IniEditor::read(QString sPathname, ParserCallback pc = NULL, void *user = NULL)
{
    clear();
    szLineParsing = NULL;
    ilineParsing = 0;
    sSectionParsing = ""; // fake section for front matter before first section
    parserCallback = pc;
    this->user = user;

    fileParsing.setFileName(sPathname);
    if (!fileParsing.open(QIODevice::ReadOnly | QIODevice::Text)) {
        fe = fileParsing.error();
        return false;
    }

    int iline = ini_parse_stream(inihReader, this, inihHandler, this);
    if (iline < 0) {
        return false;
    }
    // ignore parsing errors, allow editing of the rest, is that correct?
    
    fileParsing.close();
    return true;
}

void Section::dump() const
{
    for (LineIndex::const_iterator il = li.constBegin(); il != li.constEnd(); il++) {
        qStdout() << il.value()->sName << "=" << il.value()->sValue << endl;
    }
}

void IniEditor::dump() const
{
    for (SectionIndex::const_iterator is = si.constBegin(); is != si.constEnd(); is++) {
        qStdout() << "[" << is.key() << "]" << endl;
        is.value().dump();
    }

    for (ConstLineIterator il = ll.constBegin(); il != ll.constEnd(); il++ ) {
        qStdout() << il->sLine;
    }
    qStdout() << endl;
}

void Section::insert(const QString &sPropertyName, LineIterator il)
{
    li.insert(sPropertyName, il);
}

const QString Section::value(const QString &sPropertyName, const QString &sDefault) const
{
    LineIndex::const_iterator il = li.constFind(sPropertyName);
    return (il == li.constEnd()) ? sDefault : il.value()->sValue;
}

const QString Section::value(const QString &sPropertyName) const
{
    return value(sPropertyName, QString::null);
}
