#ifndef INIEDIT_H
#define INIEDIT_H

#include <QString>
#include <QLinkedList>
#include <QMap>
#include <QFile>

class Line
{
public:
    Line() {}
    Line(bool fHasProperty, int iline, QString sName, QString sValue, QString sLine):
        fHasProperty(fHasProperty), iline(iline), sName(sName), sValue(sValue), sLine(sLine) {}

    bool fHasProperty;
    int iline;
    QString sName;
    QString sValue;
    QString sLine;

    inline int operator==(Line &e) { return (this == &e); }
};

typedef QLinkedList<Line> LineList;
typedef LineList::iterator LineIterator;
typedef LineList::const_iterator ConstLineIterator;
typedef QMap<QString, LineIterator> LineIndex;

class Section
{
public:
    Section() {}
    ~Section() {}

    void insert(const QString &sPropertyName, LineIterator il);
    const Line &getLine(const QString &sPropertyName);

    const QString value(const QString &sPropertyName) const;
    const QString value(const QString &sPropertyName, const QString &sDefault) const;
    inline const QString operator[](const QString &sPropertyName) const { return value(sPropertyName); }

    void dump() const;

private:
    LineIndex li;
};

typedef QMap<QString, Section> SectionIndex;

class IniEditor
{
public:
    IniEditor() {}
    ~IniEditor() {}

    typedef bool (*ParserCallback)(void *user, QString &sSection, Line &line);

    void clear();
    bool read(QString sPathname, ParserCallback pc, void *user);
    QFileDevice::FileError error() {return fe;}

    const Section &section(QString s) {return si[s];}
    void dump() const;

private:
    SectionIndex si;
    LineList ll;

    QFileDevice::FileError fe;
    QFile fileParsing;
    char *szLineParsing;
    int ilineParsing;
    QString sSectionParsing;
    ParserCallback parserCallback;
    void *user;

    char *parserReader(char *szBuffer, int cbBuffer);
    int parserHandler(const char *szSection, const char *szName, const char *szValue);
    static char *inihReader(char *szBuffer, int cbBuffer, void *user);
    static int inihHandler(void *user, const char *szSection, const char *szName, const char *szValue);
};

#endif // INIEDIT_H
