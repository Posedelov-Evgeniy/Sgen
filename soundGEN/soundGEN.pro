#-------------------------------------------------
#
# Project created by QtCreator 2013-10-15T17:29:55
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = soundGEN
TEMPLATE = app

android {
    # Add files and directories to ship with the application
    # by adapting the examples below.
    # file1.source = myfile
    # dir1.source = mydir
    file1.source = $$PWD/base_functions.cpp
    file2.source = $$PWD/base_functions.h
    file3.source = $$PWD/config.cfg
    file4.source = $$PWD/functions.cpp.cfg
    file5.source = $$PWD/examples
    file6.source = $$PWD/translations
    file7.source = $$PWD/api/android/lib/libfmodex.so
    DEPLOYMENTFOLDERS = file1 file2 file3 file4 file5 file6 file7

    # If your application uses the Qt Mobility libraries, uncomment
    # the following lines and add the respective components to the
    # MOBILITY variable.
    # CONFIG += mobility
    # MOBILITY +=

    LIBS += $$PWD/api/android/lib/armeabi/libfmodex.so
    INCLUDEPATH += $$PWD/api/android/inc
    DEPENDPATH += $$PWD/api/android/inc
    PRE_TARGETDEPS += $$PWD/api/android/lib/armeabi/libfmodex.so
    # Please do not modify the following two lines. Required for deployment.
    include(deployment.pri)
    qtcAddDeployment()
} else:win32 {
    RC_FILE += soundGEN.rc
    LIBS += $$PWD/api/windows/lib/fmod_vc.lib
    INCLUDEPATH += $$PWD/api/windows/inc
    DEPENDPATH += $$PWD/api/windows/inc
    PRE_TARGETDEPS += $$PWD/api/windows/lib/fmod.dll

    include(deployment_desktop.pri)
} else:win64 {
    RC_FILE += soundGEN.rc
    LIBS += $$PWD/api/windows/lib/fmod64_vc.lib
    INCLUDEPATH += $$PWD/api/windows/inc
    DEPENDPATH += $$PWD/api/windows/inc
    PRE_TARGETDEPS += $$PWD/api/windows/lib/fmod64.dll

    include(deployment_desktop.pri)
} else:unix {
    *-64 {
        LIBS += -L$$PWD/api/linux/lib/x86_64/ -lfmod
        PRE_TARGETDEPS += $$PWD/api/linux/lib/x86_64/libfmod.so
    } else {
        LIBS += -L$$PWD/api/linux/lib/x86/ -lfmod
        PRE_TARGETDEPS += $$PWD/api/linux/lib/x86/libfmod.so
    }
    INCLUDEPATH += $$PWD/api/linux/inc
    DEPENDPATH += $$PWD/api/linux/inc

    include(deployment_desktop.pri)
}

SOURCES += main.cpp\
    base_functions.cpp \
    classes/environmentinfo.cpp \
    abstractsndcontroller.cpp \
    kiss_fft/kiss_fft.c \
    kiss_fft/kiss_fftr.c \
    classes/sndanalyzer.cpp \
    mainwindow.cpp \
    sndcontroller.cpp \
    widgets/soundpicker.cpp \
    soundlist.cpp \
    widgets/functiongraphicdrawer.cpp \
    classes/graphicthread.cpp \
    widgets/mgraphicdrawsurface.cpp \
    widgets/mfftdrawsurface.cpp \
    widgets/channelsettings.cpp \
    classes/highlighter.cpp \
    classes/utextblockdata.cpp \
    classes/utextedit.cpp \
    widgets/dialogfunctions.cpp \
    widgets/dialogexport.cpp

HEADERS  += base_functions.h \
    classes/environmentinfo.h \
    abstractsndcontroller.h \
    widgets/soundpicker.h \
    soundlist.h \
    sndcontroller.h \
    kiss_fft/_kiss_fft_guts.h \
    kiss_fft/kiss_fft.h \
    kiss_fft/kissfft.hh \
    kiss_fft/kiss_fftr.h \
    classes/sndanalyzer.h \
    mainwindow.h \
    widgets/functiongraphicdrawer.h \
    classes/graphicthread.h \
    widgets/mgraphicdrawsurface.h \
    widgets/mfftdrawsurface.h \
    widgets/channelsettings.h \
    classes/highlighter.h \
    classes/utextblockdata.h \
    classes/utextedit.h \
    widgets/dialogfunctions.h \
    widgets/dialogexport.h

FORMS    += mainwindow.ui \
    widgets/soundpicker.ui \
    widgets/functiongraphicdrawer.ui \
    widgets/channelsettings.ui \
    widgets/dialogfunctions.ui \
    widgets/dialogexport.ui

OTHER_FILES += \
    config.cfg \
    functions.cpp.cfg \
    scripts/lin_deploy.sh \
    scripts/win_deploy.bat \
    soundGEN.rc \
    android/AndroidManifest.xml \
    android/res/drawable/icon.png \
    android/res/drawable/logo.png \
    android/res/drawable-hdpi/icon.png \
    android/res/drawable-ldpi/icon.png \
    android/res/drawable-mdpi/icon.png \
    android/res/layout/splash.xml \
    android/res/values/libs.xml \
    android/res/values/strings.xml \
    android/res/values-de/strings.xml \
    android/res/values-el/strings.xml \
    android/res/values-es/strings.xml \
    android/res/values-et/strings.xml \
    android/res/values-fa/strings.xml \
    android/res/values-fr/strings.xml \
    android/res/values-id/strings.xml \
    android/res/values-it/strings.xml \
    android/res/values-ja/strings.xml \
    android/res/values-ms/strings.xml \
    android/res/values-nb/strings.xml \
    android/res/values-nl/strings.xml \
    android/res/values-pl/strings.xml \
    android/res/values-pt-rBR/strings.xml \
    android/res/values-ro/strings.xml \
    android/res/values-rs/strings.xml \
    android/res/values-ru/strings.xml \
    android/res/values-zh-rCN/strings.xml \
    android/res/values-zh-rTW/strings.xml \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
    android/src/org/kde/necessitas/origo/QtActivity.java \
    android/src/org/kde/necessitas/origo/QtApplication.java \
    android/version.xml

TRANSLATIONS += translations/soundGEN_ru_RU.ts
