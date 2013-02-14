# Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = termenvox
TEMPLATE = app

QMAKE_CXXFLAGS = /wd4100

SOURCES += main.cpp \
    mainwindow.cpp \
    thereminwidget.cpp \
    theremin.cpp

HEADERS += \
    mainwindow.h \
    thereminwidget.h \
    theremin.h

FORMS += mainwindow.ui

DEFINES += __WINDOWS_ASIO__ __WINDOWS_DS__ __LITTLE_ENDIAN__ __WINDOWS_MM__

INCLUDEPATH += $$PWD/STK/include
DEPENDPATH += $$PWD/STK/include

LIBS += winmm.lib dsound.lib Wsock32.lib Ole32.lib Advapi32.lib

win32:CONFIG(release, debug|release): LIBS += "$$PWD/STK/lib/STK.lib"
else:win32:CONFIG(debug, debug|release): LIBS += "$$PWD/STK/lib/STKd.lib"
