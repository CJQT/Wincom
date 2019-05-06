#-------------------------------------------------
#
# Project created by QtCreator 2019-05-05T14:30:24
#
#-------------------------------------------------

QT       += core gui
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mycom
TEMPLATE = app


SOURCES += main.cpp\
        wincom.cpp \
    qextserialbase.cpp \
    qextserialport.cpp \
    win_qextserialport.cpp

HEADERS  += wincom.h \
    qextserialbase.h \
    qextserialport.h \
    win_qextserialport.h

FORMS    += wincom.ui
