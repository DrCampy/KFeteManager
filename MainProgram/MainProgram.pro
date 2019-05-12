#-------------------------------------------------
#
# Project created by QtCreator 2018-12-16T15:02:50
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = KFeteManager
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
    credentialsmanager.cpp \
        main.cpp \
        mainwindow.cpp \
    loginview.cpp \
    salesview.cpp \
    cartemodel.cpp \
    catalog.cpp \
    currentordermodel.cpp \
    carteview.cpp \
    databasemanager.cpp \
    clientlist.cpp \
    order.cpp \
    catalogmanager.cpp \
    cartemanager.cpp \
    countmoney.cpp \
    clientmanager.cpp \
    ordermanager.cpp \
    customwidgets.cpp \
    currentorderview.cpp \
    sessionsmanager.cpp \
    historiesmanager.cpp

HEADERS += \
    credentialsmanager.h \
        mainwindow.h \
    loginview.h \
    salesview.h \
    cartemodel.h \
    catalog.h \
    currentordermodel.h \
    carteview.h \
    databasemanager.h \
    clientlist.h \
    order.h \
    catalogmanager.h \
    cartemanager.h \
    countmoney.h \
    clientmanager.h \
    ordermanager.h \
    customwidgets.h \
    currentorderview.h \
    sessionsmanager.h \
    historiesmanager.h


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    scripts.qrc

DISTFILES += \
    Notes.txt
