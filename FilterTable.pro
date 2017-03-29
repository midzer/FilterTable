#-------------------------------------------------
#
# Project created by QtCreator 2017-03-29T09:14:23
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FilterTable
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    filterheader.cpp \
    sortfilterproxymodel.cpp

HEADERS  += mainwindow.h \
    filterheader.h \
    sortfilterproxymodel.h
