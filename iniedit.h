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

    bool fNonValueLine;
    QString sValue;
    QString sLine;
};

typedef OrderedMap<QString, Element> Section;
typedef OrderedMap<QString, Section> SectionMap;
typedef SectionMap::iterator SectionIterator;
typedef OrderedMap<QString, Element> ElementMap;
typedef ElementMap::iterator ElementIterator;


class IniEditor
{
public:
    IniEditor() {};
    ~IniEditor() {};

    bool read(QString sPathname);
    QFileDevice::FileError error() {return fe;}

    Section section(QString s) {return msect[s];}
    void dump(void);

private:
    SectionMap msect;
    QFileDevice::FileError fe;
    QFile fileParsing;
    char *szLineParsing;
    int ilineParsing;
    QString sSectionParsing;

public: // not really though
    char *parserReader(char *szBuffer, int cbBuffer);
    int parserHandler(const char *szSection, const char *szName, const char *szValue);
};

#endif // INIEDIT_H
