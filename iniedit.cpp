#include <errno.h>

#include "orderedmap.h"
#include "iniedit.h"

extern "C" {
#define INI_ALLOW_MULTILINE 0
#define NOTIFY_SECTION_CHANGE 1
#include "inih/ini.h"
}

#include <QString>

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

    om[sSectionParsing].append(QString("%d").arg(++lnParsing), Element(true, QString(), szLineParsing));
    return szBuffer;
}

int IniEditor::parserHandler(const char *szSection, const char *szName, const char *szValue)
{
    QString sSection(szSection);
    om[sSectionParsing].remove(QString("%d").arg(lnParsing)); // remove the provisional
    sSectionParsing = sSection;
    if (szName[0])
        om[sSectionParsing].append(szName, Element(false, szValue, szLineParsing));
    return true;
}

bool IniEditor::read(QString sPathname)
{
    om.clear();
    fe = QFileDevice::NoError;
    szLineParsing = NULL;
    lnParsing = 0;
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


