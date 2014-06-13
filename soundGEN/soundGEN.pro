#-------------------------------------------------
#
# Project created by QtCreator 2013-10-15T17:29:55
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = soundGEN
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    sndcontroller.cpp

HEADERS  += mainwindow.h \
    sndcontroller.h

FORMS    += mainwindow.ui

unix:!macx: LIBS += -L$$PWD/api/lib/ -lfmodex64

INCLUDEPATH += $$PWD/api
DEPENDPATH += $$PWD/api
