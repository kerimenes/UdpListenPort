#-------------------------------------------------
#
# Project created by QtCreator 2017-09-05T12:16:17
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UdpListenPort
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    udpserver.cpp \
    tcpserver.cpp

HEADERS  += mainwindow.h \
    udpserver.h \
    tcpserver.h

FORMS    += mainwindow.ui
