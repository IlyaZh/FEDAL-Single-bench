#-------------------------------------------------
#
# Project created by QtCreator 2018-08-29T11:46:09
#
#-------------------------------------------------

QT       += core gui serialport serialbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FedalSingle
TEMPLATE = app
VERSION = 1.2.0

CONFIG += c++17
QMAKE_CXXFLAGS += -Wunused-value
QMAKE_CXXFLAGS += -Werror

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    appsettings.cpp \
    deviceform.cpp \
    models/data_thread.cpp \
    settingsdialog.cpp \
    aboutdialog.cpp \
    serialporthandler.cpp

HEADERS  += mainwindow.h \
    globals.h \
    appsettings.h \
    deviceform.h \
    models/data_thread.h \
    models/queue.hpp \
    mocks/serial_mock.hpp \
    settingsdialog.h \
    aboutdialog.h \
    serialporthandler.h

FORMS    += mainwindow.ui \
    deviceform.ui \
    settingsdialog.ui \
    aboutdialog.ui

RESOURCES += \
    resources.qrc

RC_ICONS = FEDAL.ico

TRANSLATIONS = qt_en.ts

