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
#CONFIG += g


SOURCES += main.cpp\
        mainwindow.cpp \
    supertcpmanager.cpp \
    epollengineer.cpp \
    abstractsocket.cpp \
    clientsocket.cpp \
    serversocket.cpp \
    httpserver.cpp \
    httprequest.cpp \
    httpresponse.cpp \
    downloadlistviewitem.cpp \
    downloadlistviewitemdelegate.cpp \
    downloadlistmodel.cpp

HEADERS  += mainwindow.h \
    supertcpmanager.h \
    epollengineer.h \
    abstractsocket.h \
    clientsocket.h \
    serversocket.h \
    httpserver.h \
    httprequest.h \
    httpresponse.h \
    mainwindow.h \
    downloadlistviewitem.h \
    downloadlistviewitemdelegate.h \
    downloadlistmodel.h

FORMS    += mainwindow.ui \
    download_item_row.ui
