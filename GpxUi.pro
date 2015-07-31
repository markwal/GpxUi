#-------------------------------------------------
#
# Project created by QtCreator 2015-06-21T14:02:10
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GpxUi
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    rungpx.cpp

HEADERS  += mainwindow.h \
    rungpx.h \
    build/version.h

FORMS    += mainwindow.ui \
    rungpx.ui

DISTFILES += \
    gpxui.rc

RESOURCES += \
    gpxui.qrc

RC_FILE = gpxui.rc

QMAKE_CXXFLAGS_RELEASE += -g
QMAKE_CFLAGS_RELEASE += -g
QMAKE_LFLAGS_RELEASE =
