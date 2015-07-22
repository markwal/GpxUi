#ifndef LOGMODEL_H
#define LOGMODEL_H

#include <QStringListModel>

class LogModel : public QStringListModel
{
public:
    explicit LogModel(QWidget *parent = 0);

    void append(const QString& s);

    LogModel& operator<<(const QString& s);

private:
    QString sPartial;
};

#endif // LOGMODEL_H
