#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    CModbusClientThread::Config modbus_client_config;
    CModbusServerThread::Config modbus_server_config;
    CInfluxDbInsertThread::Config influxdb_insert_config;
    CMySqlThread::Config mysql_config;

    if (!loadConfigFromIni(modbus_client_config,
                           modbus_server_config,
                           influxdb_insert_config,
                           mysql_config)) {
        QMessageBox::about(this, "Warning", "载入配置文件config.ini失败！");
        //qDebug()<<"Load configuration from config.ini failed.";
    } else {
        /* create a modbus client thread */
        m_modbus_client_thread = new CModbusClientThread(modbus_client_config,
                                                         MODBUS_CLIENT_ID,
                                                         HOLDING_REGISTERS_READ_START_ADDR,
                                                         HOLDING_REGISTERS_READ_NUMBERS,
                                                         HOLDING_REGISTERS_WRITE_START_ADDR,
                                                         HOLDING_REGISTERS_WRITE_NUMBERS,
                                                         m_holding_registers);
        if (m_modbus_client_thread) {
            m_modbus_client_thread->start();
            connect(m_modbus_client_thread, &CModbusClientThread::eventReport, this, &MainWindow::onEventReported);
            /*connect(m_modbus_client_thread, &CModbusClientThread::readSuccessfully,
                    this, &MainWindow::onModbusClientToUpdateUi);
            connect(m_modbus_client_thread, &CModbusClientThread::readSuccessfully,
                    this, &MainWindow::onModbusClientToInsertIntoInfluxDb);
            connect(this, &MainWindow::ProcessParaUpdated,
                    this, &MainWindow::onProcessParaUpdated);*/
        }
        /* create a modbus server thread */
        m_modbus_server_thread = new CModbusServerThread(modbus_server_config,
                                                         MODBUS_SERVER_ID,
                                                         HOLDING_REGISTERS_SERVER_START_ADDR,
                                                         HOLDING_REGISTERS_SERVER_NUMBERS);
        if (m_modbus_server_thread) {
            m_modbus_server_thread->start();
            connect(m_modbus_server_thread, &CModbusServerThread::eventReport, this, &MainWindow::onEventReported);
        }

        /* create a influx database to insert */
        m_influxdb_insert_thread = new CInfluxDbInsertThread(influxdb_insert_config,
                                                             INFLUXDB_INSERT_ID,
                                                             m_influx_insert_units);
        if (m_influxdb_insert_thread) {
            m_influxdb_insert_thread->start();
            connect(m_influxdb_insert_thread, &CInfluxDbInsertThread::eventReport, this, &MainWindow::onEventReported);
        }


        /* create a mysql database */
        m_mysql_thread = new CMySqlThread(mysql_config, MYSQL_CLIENT_ID);
        if (m_mysql_thread) {
            m_mysql_thread->start();
            connect(m_mysql_thread, &CMySqlThread::eventReport, this, &MainWindow::onEventReported);
            m_mysql_thread->openDatabase();
        }

        /* connect modbus and influxdb insert */
        if (m_modbus_client_thread && m_influxdb_insert_thread) {
            connect(m_modbus_client_thread, &CModbusClientThread::readSuccessfully,
                    this, &MainWindow::onModbusClientToInsertIntoInfluxDb);
            connect(this, &MainWindow::influxdbInsertRequired,
                    m_influxdb_insert_thread, &CInfluxDbInsertThread::onInsertRequired);
        }
    }
    /* init influx db insert unit */
    InfluxDbUnitPrepare();

}

MainWindow::~MainWindow()
{
    delete ui;

    delete m_modbus_client_thread;
}
void MainWindow::InfluxDbUnitPrepare() {
    CInfluxDbInsertThread::InsertUnit unit;
    /* prepare pump units */
    for (int i=0; i < PUMPS_NUMBERS; i++) {
        unit.enable = false;
        unit.meansurement = "pumps";
        unit.tag_key = "pno";
        unit.tag_value = "p" + QString::number(i + 1);
        QList<CInfluxDbInsertThread::Point> fields
                = {{"frq",""},
                   {"amp",""},
                   {"volt",""},
                   {"pow",""},
                   {"rpm",""},
                   {"pin",""},
                   {"pout",""},
                   {"flow",""},
                   {"head",""}};
        unit.fields.clear();
        unit.fields.append(fields);
        m_influx_insert_units.append(unit);
    }

    /* prepare system units */
    for (int i=0; i < SYSTEMS_NUMBERS; i++) {
        unit.enable = true;
        unit.meansurement = "systems";
        unit.tag_key = "sno";
        unit.tag_value = "s" + QString::number(i + 1);
        QList<CInfluxDbInsertThread::Point> fields
                = {{"suclev",""},
                   {"flow",""},
                   {"pout",""},
                   {"pset",""}};
        unit.fields.clear();
        unit.fields.append(fields);
        m_influx_insert_units.append(unit);
    }

}

void MainWindow::onEventReported(int id) {
    QString owner_name, event_text;
    switch(id) {
    case MODBUS_CLIENT_ID:
        owner_name = "Modbus client ";
        event_text = *(qobject_cast<CModbusClientThread *>(sender())->getCurrentErrorText());
        break;
    case MODBUS_SERVER_ID:
        owner_name = "Modbus sever ";
        event_text = *(qobject_cast<CModbusServerThread *>(sender())->getCurrentErrorText());
        break;
    case MYSQL_CLIENT_ID:
        owner_name = "MySQL client ";
        event_text = *(qobject_cast<CMySqlThread *>(sender())->getCurrentErrorText());
        break;
    case INFLUXDB_INSERT_ID:
        owner_name = "InfluxDb insert client ";
        event_text = *(qobject_cast<CInfluxDbInsertThread *>(sender())->getCurrentErrorText());
        break;
    }
    ui->eventDisplay->InsertNewLine(owner_name + ": " + event_text);

    //write to MySql
    if (!m_mysql_thread->recordEvent(owner_name, event_text)) {
        ui->eventDisplay->InsertNewLine("MySQL event database: failed to insert event.");
    }
}

void MainWindow::onModbusClientToInsertIntoInfluxDb() {
    qDebug()<<"MainWindow has received a signal from Modbus client to insert into influx Db.";

    Q_ASSERT(m_influx_insert_units.count() >= PUMPS_NUMBERS + SYSTEMS_NUMBERS);
    Q_ASSERT(m_holding_registers.count() >= 80 + PUMPS_NUMBERS);

    /* set pumps values */
    for (int i=0; i<PUMPS_NUMBERS; i++) {
        Q_ASSERT(m_influx_insert_units.at(i).fields.count() >= 9);
        uint start_addr = i * 9 + 9;
        m_influx_insert_units[i].enable = (m_holding_registers[start_addr - 1] & 0b100);
        if (!m_influx_insert_units[i].enable)
            continue;
        m_influx_insert_units[i].fields[0].value =
                QString::number((m_holding_registers[start_addr] / 10.0));
        m_influx_insert_units[i].fields[1].value =
                QString::number((m_holding_registers[start_addr + 1]) /10.0);
        m_influx_insert_units[i].fields[2].value =
                QString::number((m_holding_registers[start_addr + 2]) /1.0);
        m_influx_insert_units[i].fields[3].value =
                QString::number((m_holding_registers[start_addr + 3]) / 10.0);
        m_influx_insert_units[i].fields[4].value =
                QString::number((m_holding_registers[start_addr + 4]) / 1.0);
        m_influx_insert_units[i].fields[5].value =
                QString::number(((qint16)(m_holding_registers[start_addr + 5])) / 10000.0);
        m_influx_insert_units[i].fields[6].value =
                QString::number(((qint16)(m_holding_registers[start_addr + 6])) /10000.0);
        m_influx_insert_units[i].fields[7].value =
                QString::number((m_holding_registers[start_addr + 7]) / 1.0);
        m_influx_insert_units[i].fields[8].value =
                QString::number((m_holding_registers[80 + i]) / 1.0);
    }

    /* set systems values */
    int start_index = PUMPS_NUMBERS;
    for (int i=0; i< SYSTEMS_NUMBERS; i++) {
        Q_ASSERT(m_influx_insert_units.at(i).fields.count() >= 4);
        uint start_addr = i * 4;
        m_influx_insert_units[start_index + i].enable = true;
        m_influx_insert_units[start_index + i].fields[0].value =
                QString::number((m_holding_registers[start_addr + 0]) / 100.0);
        m_influx_insert_units[start_index + i].fields[1].value =
                QString::number((m_holding_registers[start_addr + 1]) / 1.0);
        m_influx_insert_units[start_index + i].fields[2].value =
                QString::number((m_holding_registers[start_addr + 2]) / 10000.0);
        m_influx_insert_units[start_index + i].fields[3].value =
                QString::number((m_holding_registers[start_addr + 3]) / 10000.0);
    }

    emit influxdbInsertRequired();
}

void MainWindow::on_pushButton_clicked()
{

}

bool MainWindow::loadConfigFromIni(CModbusClientThread::Config &modbus_client_config,
                       CModbusServerThread::Config &modbus_server_config,
                       CInfluxDbInsertThread::Config &influxdb_insert_config,
                       CMySqlThread::Config &mysql_config) {
    QSettings *configIniRead = new QSettings(QApplication::applicationDirPath()
                                             + CONFIG_FILE_NAME,
                                             QSettings::IniFormat);
    if (!configIniRead) {
        qDebug()<<"Load configuration file failed.";
        return false;
    } else
        qDebug()<<"Configuration file is loaded.";

    /* modbus client config */
    bool ok;
    if (configIniRead->contains("ModbusTcpClient/ServerIp")) {
        modbus_client_config.server_ip = configIniRead->value("ModbusTcpClient/ServerIp").toString();
        modbus_client_config.server_id = configIniRead->value("ModbusTcpClient/ServerId").toUInt(&ok);
        modbus_client_config.server_port = configIniRead->value("ModbusTcpClient/ServerPort").toUInt(&ok);
        modbus_client_config.timeout = configIniRead->value("ModbusTcpClient/Timeout").toUInt(&ok);
        modbus_client_config.number_of_retries = configIniRead->value("ModbusTcpClient/NumberOfRetries").toUInt(&ok);
        modbus_client_config.polling_interval = configIniRead->value("ModbusTcpClient/PollingInterval").toUInt(&ok);
        if (!ok) {
            QMessageBox::about(this, "Warning", "Modbus client 配置错误！");
            qDebug()<<"Modbus client configuration error.";
        }
    } else {
        QMessageBox::about(this, "Warning", "Modbus client 配置缺失！");
        qDebug()<<"Missing modbus client items configuration in ini file.";
    }

    /* modbus server config */
    if (configIniRead->contains("ModbusTcpServer/ServerIp")) {
        modbus_server_config.server_ip = configIniRead->value("ModbusTcpServer/ServerIp").toString();
        modbus_server_config.server_id = configIniRead->value("ModbusTcpServer/ServerId").toUInt(&ok);
        modbus_server_config.server_port = configIniRead->value("ModbusTcpServer/ServerPort").toUInt(&ok);
        modbus_server_config.update_interval = configIniRead->value("ModbusTcpServer/UpdateInterval").toUInt(&ok);
        if (!ok) {
            QMessageBox::about(this, "Warning", "Modbus server 配置错误！");
            qDebug()<<"Modbus server configuration error.";
            return false;
        }
    } else {
        QMessageBox::about(this, "Warning", "Modbus server 配置缺失！");
        qDebug()<<"Missing modbus server configuration in ini file.";
    }

    /* influx db insert config */
    if (configIniRead->contains("InfluxDB/HostName")) {
        influxdb_insert_config.hostName = configIniRead->value("InfluxDB/HostName").toString();
        influxdb_insert_config.portNumber = configIniRead->value("InfluxDB/PortNumber").toUInt(&ok);
        influxdb_insert_config.dbName = configIniRead->value("InfluxDB/DbName").toString();
        influxdb_insert_config.userName = configIniRead->value("InfluxDB/UserName").toString();
        influxdb_insert_config.password = configIniRead->value("InfluxDB/Password").toString();
        if (!ok) {
            QMessageBox::about(this, "Warning", "InfluxDb client 配置错误！");
            qDebug()<<"InfluxDB insert configuration error.";
            return false;
        }
    } else {
        QMessageBox::about(this, "Warning", "InfluxDb client 配置缺失！");
        qDebug()<<"Missing InfluxDB insert configuration in ini file.";
        return false;
    }

    /* my sql config */
    if (configIniRead->contains("MySQL/DbType")) {
        mysql_config.dbType = configIniRead->value("MySQL/DbType").toString();
        mysql_config.hostName = configIniRead->value("MySQL/HostName").toString();
        mysql_config.portNumber = configIniRead->value("MySQL/PortNumber").toUInt(&ok);
        mysql_config.dbName = configIniRead->value("MySQL/DbName").toString();
        mysql_config.userName = configIniRead->value("MySQL/UserName").toString();
        mysql_config.password = configIniRead->value("MySQL/Password").toString();
        if (!ok) {
            QMessageBox::about(this, "Warning", "MySql client 配置错误！");
            qDebug()<<"MySql configuration error.";
            return false;
        }
    } else {
        QMessageBox::about(this, "Warning", "MySql client 配置缺失！");
        qDebug()<<"Missing MySql server configuration in ini file.";
        return false;
    }

    delete configIniRead;
    return true;
}
