
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = STKtest
TEMPLATE = app

QMAKE_CXXFLAGS = /wd4100

SOURCES += main.cpp\
        mainwindow.cpp \
    thereminwidget.cpp \
    theremin.cpp

HEADERS  += mainwindow.h \
    thereminwidget.h \
    theremin.h

FORMS    += mainwindow.ui

DEFINES += __WINDOWS_ASIO__ __WINDOWS_DS__ __LITTLE_ENDIAN__ __WINDOWS_MM__

LIBS += -L$$PWD/../STK/lib STK.lib winmm.lib dsound.lib Wsock32.lib Ole32.lib Advapi32.lib

INCLUDEPATH += $$PWD/../STK/include
DEPENDPATH += $$PWD/../STK/include

#win32:CONFIG(release, debug|release): PRE_TARGETDEPS += -L$$PWD/../STK/lib STK.lib
#else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../STK/lib STKd.lib
