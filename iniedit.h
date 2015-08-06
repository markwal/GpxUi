#ifndef INIEDIT_H
#define INIEDIT_H

class Element
{
private:
    QString sValue;
    QString sLine;
}

class Section
{
private:
    OrderedMap<QString, Element> omLines;
}

class IniEditor
{
public:
    IniEditor();
    ~IniEditor();

private:
    OrderedMap<QString, Section> om;
}

#endif // INIEDIT_H
