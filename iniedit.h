#ifndef INIEDIT_H
#define INIEDIT_H

#include <QString>
#include <QLinkedList>
#include <QMap>
#include <QFile>
#include <QFileInfo>

class Line
{
public:
    Line() {}
    Line(bool fHasProperty, int iline, QString sName, QString sValue, QString sLine):
        fHasProperty(fHasProperty), iline(iline), sName(sName), sValue(sValue), sLine(sLine)
    {
        fDeleted = false;
    }

    struct {
        bool fHasProperty : 1;
        bool fDeleted : 1;
    };
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

class IniEditor;

class Section
{
    friend class IniEditor;

public:
    Section() {}
    Section(IniEditor *pie);
    ~Section() {}

    void setParent(IniEditor *pie);
    void setName(QString s) { sName = s; }

    void insert(const QString &sPropertyName, LineIterator il);
    void remove(const QString &sPropertyName);

    const QString value(const QString &sPropertyName) const;
    const QString value(const QString &sPropertyName, const QString &sDefault) const;
    int intValue(const QString &sPropertyName, int iDefault) const;
    double value(const QString &sPropertyName, double dDefault) const;
    inline const QString operator[](const QString &sPropertyName) const { return value(sPropertyName); }

    void setValue(const QString &sPropertyName, const QString &s);
    void setValue(const QString &sPropertyName, const QString &s, const QString &sDefault);
    void setValue(const QString &sPropertyName, int i, int iDefault);
    void setValue(const QString &sPropertyName, double d, double dDefault, int cdecimals = 4);

    void dump() const;

private:
    QString sName;
    LineIndex li;
    LineIterator ilineLast;
    IniEditor *pie; // parent
};

typedef QMap<QString, Section> SectionIndex;

class IniEditor
{
    friend class Section;

public:
    IniEditor() {}
    ~IniEditor() {}

    typedef bool (*ParserCallback)(void *user, QString &sSection, Line &line);

    void clear();
    void setFilename(const QString &sPathName);
    bool copyTo(QDir dir);
    inline QString fileName() { return file.fileName(); }

    bool read(ParserCallback pc, void *user);
    bool write();
    QFileDevice::FileError error() { return fe; }

    Section &section(QString s);
    void dump() const;

private:
    SectionIndex si;
    LineList ll;

    QFileDevice::FileError fe;
    QFile file;

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
