#include <errno.h>

#include "orderedmap.h"
#include "iniedit.h"

extern "C" {
#define INI_ALLOW_MULTILINE 0
#define NOTIFY_SECTION_CHANGE 1
#include "inih/ini.h"
}

#include <QString>
#include <QTextStream>
QTextStream &qStdout();

static char *inihReader(char *szBuffer, int cbBuffer, void *user)
{
    IniEditor *pie = (IniEditor *)user;
    return pie->parserReader(szBuffer, cbBuffer);
}

static int inihHandler(void *user, const char *szSection, const char *szName, const char *szValue)
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

    msect[sSectionParsing].append(QString("%1").arg(++ilineParsing), Element(true, QString(), szLineParsing));
    return szBuffer;
}

int IniEditor::parserHandler(const char *szSection, const char *szName, const char *szValue)
{
    QString sSection(szSection);
    msect[sSectionParsing].remove(QString("%1").arg(ilineParsing)); // remove the provisional
    sSectionParsing = sSection;
    if (szName[0])
        msect[sSectionParsing].append(szName, Element(false, szValue, szLineParsing));
    return true;
}

bool IniEditor::read(QString sPathname)
{
    msect.clear();
    fe = QFileDevice::NoError;
    szLineParsing = NULL;
    ilineParsing = 0;
    sSectionParsing = ""; // fake section for front matter before first section

    fileParsing.setFileName(sPathname);
    if (!fileParsing.open(QIODevice::ReadOnly | QIODevice::Text)) {
        fe = fileParsing.error();
        return false;
    }

    int lnError = ini_parse_stream(inihReader, this, inihHandler, this);
    if (lnError < 0) {
        return false;
    }
    // ignore parsing errors, allow editing of the rest, is that correct?
    
    fileParsing.close();
    return true;
}

void IniEditor::dump(void)
{
    for (SectionIterator is = msect.begin(); is != msect.end(); is++) {
        qStdout() << "[" << is.key() << "]" << endl;
        for (ElementIterator ie = is.value().begin(); ie != is.value().end(); ie++) {
            Element &e = ie.value();
            if (e.fNonValueLine)
                qStdout() << e.sLine;
            else
                qStdout() << ie.key() << "=" << e.sValue << endl;
        }
    }
}
