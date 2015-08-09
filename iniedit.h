#ifndef INIEDIT_H
#define INIEDIT_H

#include <QString>
#include <QFile>

class Element
{
public:
    Element() {}
    Element(bool fNonValueLine, QString sValue, QString sLine):
        fNonValueLine(fNonValueLine), sValue(sValue), sLine(sLine) {}
private:
    bool fNonValueLine;
    QString sValue;
    QString sLine;
};

typedef OrderedMap<QString, Element> Section;

class IniEditor
{
public:
    IniEditor() {};
    ~IniEditor() {};

    bool read(QString sPathname);
    QFileDevice::FileError error() {return fe;}

private:
    OrderedMap<QString, Section> om;
    QFileDevice::FileError fe;
    QFile fileParsing;
    char *szLineParsing;
    int lnParsing;
    QString sSectionParsing;


public: // not really though
    char *parserReader(char *szBuffer, int cbBuffer);
    int parserHandler(const char *szSection, const char *szName, const char *szValue);
};

#endif // INIEDIT_H
