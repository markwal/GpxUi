#ifndef INIEDIT_H
#define INIEDIT_H

#include "orderedmap.h"

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

    inline int operator==(Element &e) { return (this == &e); }
};

typedef OrderedMap<QString, Element> ElementMap;
typedef ElementMap::iterator ElementIterator;
typedef ElementMap::const_iterator ConstElementIterator;

class Section : public ElementMap
{
public:
    Section() {}
    ~Section() {}

    const QString getValue(const QString &key);
    const QString getValue(const QString &key, const QString &sDefault);
};

typedef OrderedMap<QString, Section> SectionMap;
typedef SectionMap::iterator SectionIterator;

class IniEditor
{
public:
    IniEditor() {}
    ~IniEditor() {}

    void clear(void);
    bool read(QString sPathname);
    QFileDevice::FileError error() {return fe;}

    Section &section(QString s) {return msect[s];}
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
