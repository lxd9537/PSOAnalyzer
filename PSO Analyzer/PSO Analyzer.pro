QT       += core gui network charts sql

include($$PWD/qtpropertybrowser/qtpropertybrowser.pri)

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Curves/ChartView/ccustomchartview.cpp \
    Curves/ColorQueue/ccolorqueue.cpp \
    Curves/HistCurve/chistcurvewidget.cpp \
    Curves/PerformCurve/cperformcurvewidget.cpp \
    Database/InfluxDb/cinfluxdbquerythread.cpp \
    Database/MySql/cmysqlthread.cpp \
    Polyfit/polyfit.cpp \
    PumpSystem/Pump/cpump.cpp \
    PumpSystem/PumpCalculator/cpumpcalculator.cpp \
    PumpSystem/PumpData/cpumpdatainputdialog.cpp \
    PumpSystem/PumpData/cpumpdatainputtable.cpp \
    PumpSystem/PumpData/cpumpdatamanagedialog.cpp \
    PumpSystem/PumpSystemCalculator/cpumpsystemcalculator.cpp \
    PumpSystem/PumpSystem/cpumpsystem.cpp \
    PumpSystem/SystemProfile/ccommonprofileinput.cpp \
    PumpSystem/SystemProfile/cpumpprofileinput.cpp \
    PumpSystem/SystemProfile/csystemprofileinputdialog.cpp \
    main.cpp \
    mainwindow.cpp \

HEADERS += \
    Curves/ChartView/ccustomchartview.h \
    Curves/ColorQueue/ccolorqueue.h \
    Curves/HistCurve/chistcurvewidget.h \
    Curves/PerformCurve/cperformcurvewidget.h \
    Database/InfluxDb/cinfluxdbquerythread.h \
    Database/MySql/cmysqlthread.h \
   Polyfit/polyfit.h \
    PumpSystem/Pump/cpump.h \
    PumpSystem/PumpCalculator/cpumpcalculator.h \
    PumpSystem/PumpData/cpumpdatainputdialog.h \
    PumpSystem/PumpData/cpumpdatainputtable.h \
    PumpSystem/PumpData/cpumpdatamanagedialog.h \
    PumpSystem/PumpSystemCalculator/cpumpsystemcalculator.h \
    PumpSystem/PumpSystem/cpumpsystem.h \
    PumpSystem/SystemProfile/ccommonprofileinput.h \
    PumpSystem/SystemProfile/cpumpprofileinput.h \
    PumpSystem/SystemProfile/csystemprofileinputdialog.h \
   mainwindow.h \

FORMS += \
    Curves/HistCurve/chistcurvewidget.ui \
    Curves/PerformCurve/cperformcurvewidget.ui \
    PumpSystem/PumpCalculator/cpumpcalculator.ui \
    PumpSystem/PumpData/cpumpdatainputdialog.ui \
    PumpSystem/PumpData/cpumpdatamanagedialog.ui \
    PumpSystem/PumpSystemCalculator/cpumpsystemcalculator.ui \
   mainwindow.ui \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
