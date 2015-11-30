#-------------------------------------------------
#
# Project created by QtCreator 2014-05-02T10:39:58
#
#-------------------------------------------------

QT       += core gui serialport


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RobotDebug
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ragdebug.cpp

HEADERS  += mainwindow.h \
    ragdebug.h \
    idPresets.hpp

FORMS    += mainwindow.ui

PKGCONFIG += libudev
