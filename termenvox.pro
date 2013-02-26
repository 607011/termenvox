# Copyright (c) 2013 Oliver Lau <ola@ct.de>, Heise Zeitschriften Verlag. All rights reserved.

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = termenvox
TEMPLATE = app

win32 {
QMAKE_CXXFLAGS += /wd4100
DEFINES -= UNICODE
DEFINES += _CRT_SECURE_NO_WARNINGS
}

SOURCES += main.cpp \
    mainwindow.cpp \
    thereminwidget.cpp \
    theremin.cpp \
    camwidget.cpp \
    opencv.cpp

HEADERS += \
    mainwindow.h \
    thereminwidget.h \
    theremin.h \
    main.h \
    camwidget.h \
    opencv.h \
    util.h

FORMS += mainwindow.ui

DEFINES += __WINDOWS_ASIO__ __WINDOWS_DS__ __LITTLE_ENDIAN__ __WINDOWS_MM__

INCLUDEPATH += $$PWD/STK/include \
    C:/Developer/OpenCV-2.4.4-beta/build/include

DEPENDPATH += $$PWD/STK/include \
    C:/Developer/OpenCV-2.4.4-beta/build/include

LIBS += winmm.lib dsound.lib Wsock32.lib Ole32.lib Advapi32.lib \
    /L"C:/Developer/OpenCV-2.4.4-beta/build/x86/vc10/lib" \
    opencv_core244.lib opencv_highgui244.lib opencv_imgproc244.lib opencv_objdetect244.lib opencv_haartraining_engine.lib

win32:CONFIG(release, debug|release): LIBS += "$$PWD/STK/lib/STK.lib"
else:win32:CONFIG(debug, debug|release): LIBS += "$$PWD/STK/lib/STKd.lib"

OTHER_FILES += NOTES.txt \
    xml/palm.xml \
    xml/fist.xml
