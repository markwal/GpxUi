#include "mainwindow.h"
#include "orderedmap.h"
#include "iniedit.h"

#include <QApplication>
#include <QtGui>
#include <QSplashScreen>
#include <QTextStream>
#include <QStandardPaths>

QTextStream &qStdout()
{
    static QTextStream ts(stdout);
    return ts;
}

int main(int argc, char *argv[])
{
    QApplication::setStyle("Fusion");
    QApplication a(argc, argv);

    a.setOrganizationName("MarkWal");
    a.setOrganizationDomain("markwal.github.io");
    a.setApplicationName("GpxUi");

    // set the current working directory to where gpx.ini belongs
    QDir dir;
#ifdef Q_OS_WIN
    dir.cd(a.applicationDirPath());
#else // !Q_OS_WIN
    dir.cd(QStandardPaths::HomeLocation());
#endif // !Q_OS_WIN
//    QSplashScreen splash;
//    splash.show();
/*
    QString szPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(szPath);
    if (!szPath.isEmpty())
    {
        Slic3r s;
        dir.cdUp();
        dir.cd("slic3r/print");
        QFile file(dir.filePath("medium.ini"));
        QSettings settings(file.fileName(), QSettings::IniFormat);
        s.append(file.fileName().toLatin1().data());
        foreach (const QString &sKey, settings.childKeys()) {
            s.append(QString("%1 = %2").arg(sKey, settings.value(sKey).toString()));
            qDebug() << sKey;
            qDebug() << settings.value(sKey);
        }
        s.exec();
    }
*/
    OrderedMap<QString, QString> om;
    om.append("key1", "value1");
    om.append("key2", "value2");
    om.append("key3", "value3");
    qStdout() << om["key1"] << endl;
    qStdout() << om["key3"] << endl;

    OrderedMap<QString, QString>::iterator i = om.begin();
    for (;i != om.end();i++) {
        qStdout() << i.key() << "=" << i.value() << endl;
    }

    IniEditor ie;

    ie.read("C:/cyg/home/markw/GpxUi/GPX/gpx.ini");
    qStdout() << ie.section("printer")["machine_type"].sValue << endl;

    ie.dump();

    MainWindow w;
    w.show();

    return a.exec();
}
