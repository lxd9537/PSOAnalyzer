#-------------------------------------------------
#
# Project created by QtCreator 2019-09-11T15:30:57
#
#-------------------------------------------------

QT       += core gui
QT       += printsupport
QT += serialbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pump_control_optimization_tool
TEMPLATE = app

RC_ICONS = graph.ico

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++14
#CONFIG += precompile_header
#PRECOMPILED_HEADER = stable.h

SOURCES += \
        examples.cpp \
        global.cpp \
        main.cpp \
        mainwindow.cpp \
        mycustomplot.cpp \
        piping.cpp \
        polyfit.cpp \
        pump.cpp \
        pumpsys.cpp \
        qcustomplot.cpp \
        qxtcsvmodel.cpp \
        readonlylineedit.cpp

HEADERS += \
        global.h \
        mainwindow.h \
        mycustomplot.h \
        piping.h \
        polyfit.h \
        pump.h \
        pumpsys.h \
        qcustomplot.h \
        qxtcsvmodel.h \
        readonlylineedit.h \
        stable.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
