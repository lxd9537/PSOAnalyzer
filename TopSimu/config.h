#ifndef CONFIG_H
#define CONFIG_H

#include "modbusserverthread.h"
#include <QSettings>
#include <QApplication>

bool loadConfigFromIni(CModbusServerThread::Config &modbus_server_config) {
    QSettings *configIniRead = new QSettings(QApplication::applicationDirPath()
                                             + CONFIG_FILE_NAME,
                                             QSettings::IniFormat);
    if (!configIniRead) {
        qDebug()<<"Load configuration file failed.";
        return false;
    } else
        qDebug()<<"Configuration file is loaded.";

    bool ok;
    /* modbus server config */
    if (configIniRead->contains("ModbusTcpServer/ServerIp")) {
        modbus_server_config.server_ip = configIniRead->value("ModbusTcpServer/ServerIp").toString();
        modbus_server_config.server_id = configIniRead->value("ModbusTcpServer/ServerId").toUInt(&ok);
        modbus_server_config.server_port = configIniRead->value("ModbusTcpServer/ServerPort").toUInt(&ok);
        modbus_server_config.update_interval = configIniRead->value("ModbusTcpServer/UpdateInterval").toUInt(&ok);
        if (!ok) {
            qDebug()<<"Modbus server configuration error.";
            return false;
        }
    } else {
        qDebug()<<"Missing modbus server configuration in ini file.";
    }

    delete configIniRead;
    return true;
}

#endif // LOADCONFIG_H
