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
#    memorycardhelper.cpp \
#    other/qdevicewatcher_linux.cpp

HEADERS  += mainwindow.h \
    udpserver.h \
    tcpserver.h
#    memorycardhelper.h \
#    other/qdevicewatcher.h \
#    other/qdevicewatcher_p.h

FORMS    += mainwindow.ui
#LIBS += -lQDeviceWatcher
