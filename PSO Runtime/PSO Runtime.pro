QT       += core gui serialbus network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Alarm/ceventtextdisplay.cpp \
    Database/InfluxDb/influxdbinsertthread.cpp \
    Database/MySql/mysqlthread.cpp \
    Modbus/Client/modbusclientthread.cpp \
    Modbus/Server/modbusserverthread.cpp \
    Ui/cpumppanel.cpp \
    Ui/csystempanel.cpp \
    main.cpp \
    mainwindow.cpp \

HEADERS += \
    Alarm/ceventtextdisplay.h \
    Database/InfluxDb/influxdbinsertthread.h \
    Database/MySql/mysqlthread.h \
    Modbus/Client/modbusclientthread.h \
    Modbus/Server/modbusserverthread.h \
    Tools/tools.h \
    Ui/cpumppanel.h \
    Ui/csystempanel.h \
    mainwindow.h \


FORMS += \
    Ui/cpumppanel.ui \
    Ui/csystempanel.ui \
    mainwindow.ui

    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
