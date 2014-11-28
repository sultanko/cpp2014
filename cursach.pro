#-------------------------------------------------
#
# Project created by QtCreator 2014-11-23T15:22:14
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cursach
TEMPLATE = app
CONFIG += c++11


SOURCES += main.cpp\
        mainwindow.cpp \
    supertcpmanager.cpp \
    epollengineer.cpp

HEADERS  += mainwindow.h \
    supertcpmanager.h \
    epollengineer.h

FORMS    += mainwindow.ui
