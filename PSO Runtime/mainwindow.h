#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include "Modbus/Client/modbusclientthread.h"
#include "Modbus/Server/modbusserverthread.h"
#include "Database/InfluxDb/influxdbinsertthread.h"
#include "Database/MySql/mysqlthread.h"
#include <QMessageBox>

#define CONFIG_FILE_NAME ("/config.ini")
/* thread id */
#define MODBUS_CLIENT_ID            (1)
#define MODBUS_SERVER_ID            (2)
#define MYSQL_CLIENT_ID             (3)
#define INFLUXDB_INSERT_ID          (4)

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

signals:
    void influxdbInsertRequired();


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onEventReported(int id);
    void onModbusClientToInsertIntoInfluxDb();
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;

    /* Modbus client */
    CModbusClientThread *m_modbus_client_thread;
    QVector<uint> m_holding_registers = QVector<uint>(HOLDING_REGISTERS_CLIENT_MAX,0);

    /* Modbus server */
    CModbusServerThread *m_modbus_server_thread;

    /* InfluxDb insert */
    CInfluxDbInsertThread *m_influxdb_insert_thread;
    QVector<CInfluxDbInsertThread::InsertUnit> m_influx_insert_units;

    /* mysql */
    CMySqlThread *m_mysql_thread;

private:
    bool loadConfigFromIni(CModbusClientThread::Config &modbus_client_config,
                           CModbusServerThread::Config &modbus_server_config,
                           CInfluxDbInsertThread::Config &influxdb_insert_config,
                           CMySqlThread::Config &mysql_config);
    void InfluxDbUnitPrepare();
};
#endif // MAINWINDOW_H
