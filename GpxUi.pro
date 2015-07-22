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
    rungpx.cpp \
    logmodel.cpp

HEADERS  += mainwindow.h \
    rungpx.h \
    logmodel.h

FORMS    += mainwindow.ui \
    rungpx.ui
