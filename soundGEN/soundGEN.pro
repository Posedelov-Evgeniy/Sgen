#-------------------------------------------------
#
# Project created by QtCreator 2013-10-15T17:29:55
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = soundGEN
TEMPLATE = app

win32: LIBS += $$PWD/api/windows/lib/fmodex_vc.lib
else:unix:!macx: LIBS += -L$$PWD/api/linux/lib/ -lfmodex64

win32: INCLUDEPATH += $$PWD/api/windows/inc
else:unix:!macx: INCLUDEPATH += $$PWD/api/linux/inc

win32: DEPENDPATH += $$PWD/api/windows/inc
else:unix:!macx: DEPENDPATH += $$PWD/api/linux/inc

win32: PRE_TARGETDEPS += $$PWD/api/windows/lib/fmodex_vc.lib
else:unix:!macx: PRE_TARGETDEPS += $$PWD/api/linux/lib/libfmodexL64.so

SOURCES += main.cpp\
    base_functions.cpp \
    mainwindow.cpp \
    sndcontroller.cpp \
    widgets/soundpicker.cpp \
    soundlist.cpp \
    widgets/functiongraphicdrawer.cpp \
    classes/graphicthread.cpp \
    widgets/mgraphicdrawsurface.cpp \
    classes/highlighter.cpp \
    classes/utextblockdata.cpp \
    classes/utextedit.cpp \
    widgets/dialogfunctions.cpp

HEADERS  += base_functions.h \
    soundlist.h \
    sndcontroller.h \
    mainwindow.h \
    widgets/soundpicker.h \
    widgets/functiongraphicdrawer.h \
    classes/graphicthread.h \
    widgets/mgraphicdrawsurface.h \
    classes/highlighter.h \
    classes/utextblockdata.h \
    classes/utextedit.h \
    widgets/dialogfunctions.h

FORMS    += mainwindow.ui \
    widgets/soundpicker.ui \
    widgets/functiongraphicdrawer.ui \
    widgets/dialogfunctions.ui

OTHER_FILES += \
    config.cfg \
    functions.cpp.cfg \
    win_deploy.bat \
    lin_deploy.sh

TRANSLATIONS += translations/soundGEN_ru_RU.ts
