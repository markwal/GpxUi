#include "mainwindow.h"
#include <QApplication>
#include <QtGui>
#include <QSplashScreen>

int main(int argc, char *argv[])
{
    QApplication::setStyle("Fusion");
    QApplication a(argc, argv);
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

    MainWindow w;
    w.show();

    return a.exec();
}
