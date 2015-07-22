#include "logmodel.h"

#include <QWidget>

LogModel::LogModel(QWidget *parent) :
    QStringListModel(parent)
{
    sPartial = QString();
}

void LogModel::append(const QString& s)
{
    int irow = rowCount();
    QString sT;

    if (sPartial.isNull()) {
        sT = s;
        insertRows(irow, 1);
    }
    else {
        sT = sPartial + s;
        sPartial.clear();
        irow--;
    }
    if (sT.endsWith('\n'))
        sT.chop(1);
    else
        sPartial = QString(sT);
    setData(index(irow), sT);
}

LogModel& LogModel::operator<<(const QString& s)
{
    append(s);
    return *this;
}

