#-------------------------------------------------
#
# Project created by QtCreator 2019-09-11T15:30:57
#
#-------------------------------------------------

QT       += core gui
QT       += sql printsupport axcontainer network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MySQL
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

CONFIG += c++11

SOURCES += \
        ActivationCode.cpp \
        dialog_editloadprofile.cpp \
        dialog_getdbpara.cpp \
        dialog_reportoption.cpp \
        dialog_reportoption_2.cpp \
        dialog_showloadprofile.cpp \
        excelbase.cpp \
        global.cpp \
        main.cpp \
        mainwindow.cpp \
        mycustomplot.cpp \
        polyfit.cpp \
        pump.cpp \
        pump_db.cpp \
        pumpsys.cpp \
        qcustomplot.cpp

HEADERS += \
        ActivationCode.h \
        dialog_editloadprofile.h \
        dialog_getdbpara.h \
        dialog_reportoption.h \
        dialog_reportoption_2.h \
        dialog_showloadprofile.h \
        excelbase.h \
        global.h \
        mainwindow.h \
        mycustomplot.h \
        polyfit.h \
        pump.h \
        pump_db.h \
        pumpsys.h \
        qcustomplot.h

FORMS += \
        dialog_editloadprofile.ui \
        dialog_getdbpara.ui \
        dialog_reportoption.ui \
        dialog_reportoption_2.ui \
        dialog_showloadprofile.ui \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target