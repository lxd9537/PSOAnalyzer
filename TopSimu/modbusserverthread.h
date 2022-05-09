#ifndef MODBUSSERVERTHREAD_H
#define MODBUSSERVERTHREAD_H

#include <QDebug>
#include <QThread>
#include <QTimer>
#include <QModbusTcpServer>
#include <QModbusDataUnit>
#include <defines.h>

#define MODBUS_SERVER_RECONNECT_DELAY            (60)
#define ERROR_CODE_MODBUS_SERVER_GEN_ERROR       (201)
#define ERROR_CODE_MODBUS_SERVER_CREATE_FAILED   (204)
#define EVENT_CODE_MODBUS_SERVER_CLOSING         (211)
#define EVENT_CODE_MODBUS_SERVER_CONNECTING      (212)
#define EVENT_CODE_MODBUS_SERVER_CONNECTED       (213)
#define EVENT_CODE_MODBUS_SERVER_UNCONNECTED     (214)

#define ERROR_TEXT_MODBUS_SERVER_CREATE_FAILED   ("Failed to create Modbus client.")
#define EVENT_TEXT_MODBUS_SERVER_CLOSING         ("Modbus server is closing.")
#define EVENT_TEXT_MODBUS_SERVER_CONNECTING      ("Modbus server is connecting.")
#define EVENT_TEXT_MODBUS_SERVER_CONNECTED       ("Modbus server is connected.")
#define EVENT_TEXT_MODBUS_SERVER_UNCONNECTED     ("Modbus server is unconnected.")

class CModbusServerThread : public QThread
{
    Q_OBJECT
public:
    typedef struct {
        QString  server_ip;
        uint     server_port;
        uint     server_id;
        uint     update_interval; //ms
    }Config;

public:
    CModbusServerThread(Config config,
                        int id,
                        int holding_registers_start,
                        int holding_registers_numbers);
    ~CModbusServerThread();
    bool SetHoldingRegisters(uint start_address, uint numbers, const QVector<uint> &holding_registers);
    bool GetHoldingRegisters(uint start_address, uint numbers, QVector<uint> &holding_registers);
    const QString *getCurrentErrorText() {return &m_current_error_text;};

signals:
    void eventReport(int id, int error_code);
    void dataWritten(int address, int size);

private slots:
    void onTimeout();
    void onDataWritten(int address, int size);
    void onModbusStateChanged(QModbusDevice::State state);
    void onModbusErrorOccurs();

private:
    void run();

    int m_id;
    Config m_modbus_config;
    QModbusTcpServer *m_modbus_server;
    QTimer *m_timer;
    uint holding_registers_numbers;
    uint holding_registers_max;
    QString m_current_error_text;

};

#endif // MBCLIENTTHREAD_H
