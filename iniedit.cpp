#include <errno.h>

#include "iniedit.h"

extern "C" {
#define INI_ALLOW_MULTILINE 0
#define NOTIFY_SECTION_CHANGE 1
#include "inih/ini.h"
}

#include <QString>
#include <QDir>
#include <QTextStream>
QTextStream &qStdout();

Section::Section(IniEditor *pie): pie(pie)
{
    ilineLast = pie->ll.end();
}

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
    if (file.atEnd())
        return NULL;

    qint64 cb = file.readLine(szLineParsing = szBuffer, cbBuffer);
    if (cb <= 0 || cb > cbBuffer)
        return NULL;

    if (szLineParsing[cb - 1] == '\n')
        szLineParsing[cb - 1] = 0;
    ll.append(Line(false, ll.count(), QString(), QString(), szLineParsing));
    return szBuffer;
}

int IniEditor::parserHandler(const char *szSection, const char *szName, const char *szValue)
{
    QString sSection(szSection);
    LineIterator iline = ll.end();
    iline--;
    if (sSectionParsing != sSection) {
        sSectionParsing = sSection;
        si[sSectionParsing].setParent(this);
        si[sSectionParsing].ilineLast = iline;
    }
    if (szName[0]) {
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

void IniEditor::setFilename(const QString &sPathname)
{
    file.setFileName(sPathname);
}

bool IniEditor::read(ParserCallback pc = NULL, void *user = NULL)
{
    clear();
    szLineParsing = NULL;
    ilineParsing = 0;
    sSectionParsing = ""; // fake section for front matter before first section
    si[sSectionParsing].setParent(this);
    parserCallback = pc;
    this->user = user;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        fe = file.error();
        return false;
    }

    int iline = ini_parse_stream(inihReader, this, inihHandler, this);
    if (iline < 0) {
        return false;
    }
    // ignore parsing errors, allow editing of the rest, is that correct?
    
    file.close();
    return true;
}

bool IniEditor::write()
{
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream ts(&file);
    for (ConstLineIterator iline = ll.constBegin(); iline != ll.constEnd(); iline++) {
        if (!iline->fDeleted)
            ts << iline->sLine << "\n";
    }

    file.close();
    return true;
}

bool IniEditor::copyTo(QDir dir)
{
    QFileInfo fi(file.fileName());
    return file.copy(dir.absoluteFilePath(fi.fileName()));
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

    for (ConstLineIterator iline = ll.constBegin(); iline != ll.constEnd(); iline++ ) {
        qStdout() << iline->sLine;
    }
    qStdout() << endl;
}

Section &IniEditor::section(QString s)
{
    Section &sect = si[s];
    if (sect.sName.isEmpty()) {
        sect.setName(s);
        sect.setParent(this);
    }
    return sect;
}

void Section::setParent(IniEditor *pie)
{
    if (pie != this->pie) {
        this->pie = pie;
        ilineLast = pie->ll.end();
    }
}

void Section::insert(const QString &sPropertyName, LineIterator iline)
{
    li.insert(sPropertyName, iline);
    ilineLast = iline;
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

int Section::intValue(const QString &sPropertyName, int iDefault) const
{
    QString s = value(sPropertyName);
    return s.isEmpty() ? iDefault : s.toInt();
}

double Section::value(const QString &sPropertyName, double dDefault) const
{
    QString s = value(sPropertyName);
    return s.isEmpty() ? dDefault : s.toDouble();
}

void Section::setValue(const QString &sPropertyName, const QString &s)
{
    QString sComment = "";
    LineIterator iline;
    LineIndex::iterator il = li.find(sPropertyName);
    if (il != li.end()) {
        iline = il.value();

        // preserve the comment
        int ich = iline->sLine.indexOf(';');
        if (ich > 0) {
            while (ich > 0 && iline->sLine[ich].isSpace())
                ich--;
            sComment = iline->sLine.mid(ich);
        }

        // set the new line content
        iline->sLine = QString("%1=%2%3").arg(iline->sName).arg(s).arg(sComment);
        iline->sValue = s;
    }
    else {
        iline = ilineLast;
        if (iline == pie->ll.end())
            pie->ll.append(Line(false, -1, "", "", QString("[%1]").arg(sName)));
        else
            iline++;
        iline = pie->ll.insert(iline, Line(true, -1, sPropertyName, s,
            QString("%1=%2").arg(sPropertyName).arg(s)));
        insert(sPropertyName, iline);
    }
}

void Section::remove(const QString &sPropertyName)
{
    LineIndex::iterator il = li.find(sPropertyName);
    if (il != li.end()) {
        LineIterator iline = il.value();
        li.erase(il);
        iline->fDeleted = true;
    }
}

void Section::setValue(const QString &sPropertyName, const QString &s, const QString &sDefault)
{
    if (s == sDefault)
        remove(sPropertyName);
    else
        setValue(sPropertyName, s);
}

void Section::setValue(const QString &sPropertyName, int i, int iDefault)
{
    if (i == iDefault)
        remove(sPropertyName);
    else
        setValue(sPropertyName, QString::number(i));
}

void Section::setValue(const QString &sPropertyName, double d, double dDefault, int cdecimals)
{
    if (d == dDefault)
        remove(sPropertyName);
    else
        setValue(sPropertyName, QString::number(d, 'f', cdecimals));
}
