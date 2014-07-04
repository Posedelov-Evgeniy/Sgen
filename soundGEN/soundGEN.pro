#-------------------------------------------------
#
# Project created by QtCreator 2013-10-15T17:29:55
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = soundGEN
TEMPLATE = app

win64: LIBS += -L$$PWD/api/windows/lib/ -lfmodexL64_vc
else:win32: LIBS += -L$$PWD/api/windows/lib/ -lfmodexL_vc
else:unix:!macx: LIBS += -L$$PWD/api/linux/lib/ -lfmodex64

win64: INCLUDEPATH += $$PWD/api/windows/inc
else:win32: INCLUDEPATH += $$PWD/api/windows/inc
else:unix:!macx: INCLUDEPATH += $$PWD/api/linux/inc

win64: DEPENDPATH += $$PWD/api/windows/inc
else:win32: DEPENDPATH += $$PWD/api/windows/inc
else:unix:!macx: DEPENDPATH += $$PWD/api/linux/inc

win64: PRE_TARGETDEPS += $$PWD/api/windows/lib/fmodexL64_vc.lib
else:win32: PRE_TARGETDEPS += $$PWD/api/windows/lib/fmodexL_vc.lib
else:unix:!macx: PRE_TARGETDEPS += $$PWD/api/linux/lib/libfmodexL64.so

SOURCES += main.cpp\
        mainwindow.cpp \
    sndcontroller.cpp \
    widgets/soundpicker.cpp \
    soundlist.cpp \
    widgets/functiongraphicdrawer.cpp \
    widgets/graphicthread.cpp \
    widgets/mgraphicdrawsurface.cpp

HEADERS  += soundlist.h \
    sndcontroller.h \
    mainwindow.h \
    widgets/soundpicker.h \
    widgets/functiongraphicdrawer.h \
    widgets/graphicthread.h \
    widgets/mgraphicdrawsurface.h

FORMS    += mainwindow.ui \
    widgets/soundpicker.ui \
    widgets/functiongraphicdrawer.ui
