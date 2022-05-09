#-------------------------------------------------
#
# Project created by QtCreator 2019-06-12T13:53:08
#
#-------------------------------------------------

QT       += core gui serialbus serialport widgets network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = test
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
        clicklabel.cpp \
        confirmdialog.cpp \
        controlvalve.cpp \
        doubleinputdialog.cpp \
        doubleinputedit.cpp \
        editablelineedit.cpp \
        global.cpp \
        intinputdialog.cpp \
        intinputedit.cpp \
        main.cpp \
        mainwindow.cpp \
        modifydialog.cpp \
        mycustomplot.cpp \
        polyfit.cpp \
        pump.cpp \
        pumpcurvedialog.cpp \
        pumpoperatedialog.cpp \
        pumpunit.cpp \
        qcustomplot.cpp \
        qxtcsvmodel.cpp \
        readonlylineedit.cpp \
        settingsdialog.cpp

HEADERS += \
        clicklabel.h \
        confirmdialog.h \
        controlvalve.h \
        doubleinputdialog.h \
        doubleinputedit.h \
        editablelineedit.h \
        global.h \
        intinputdialog.h \
        intinputedit.h \
        mainwindow.h \
        modifydialog.h \
        mycustomplot.h \
        polyfit.h \
        pump.h \
        pumpcurvedialog.h \
        pumpoperatedialog.h \
        pumpunit.h \
        qcustomplot.h \
        qxtcsvmodel.h \
        readonlylineedit.h \
        settingsdialog.h

FORMS += \
        confirmdialog.ui \
        doubleinputdialog.ui \
        intinputdialog.ui \
        mainwindow.ui \
        modifydialog.ui \
        pumpcurvedialog.ui \
        pumpoperatedialog.ui \
        settingsdialog.ui \
        setvfdmodeldialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target



