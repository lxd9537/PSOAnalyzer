#ifndef MODBUSCLIENTTHREAD_H
#define MODBUSCLIENTTHREAD_H

#include <QDebug>
#include <QThread>
#include <QTimer>
#include <QModbusClient>
#include <QModbusTcpClient>
#include <QModbusDataUnit>

/* Modbus client configuration */
#define HOLDING_REGISTERS_CLIENT_MAX        (500)
#define HOLDING_REGISTERS_READ_START_ADDR   (0)
#define HOLDING_REGISTERS_READ_NUMBERS      (100)
#define HOLDING_REGISTERS_WRITE_START_ADDR  (100)
#define HOLDING_REGISTERS_WRITE_NUMBERS     (20)

#define MODBUS_CLIENT_RECONNECT_DELAY            (30)
#define ERROR_TEXT_MODBUS_CLIENT_CREATE_FAILED   ("Failed to create Modbus client.")
#define EVENT_TEXT_MODBUS_CLIENT_CLOSING         ("Modbus client is closing.")
#define EVENT_TEXT_MODBUS_CLIENT_CONNECTING      ("Modbus client is connecting.")
#define EVENT_TEXT_MODBUS_CLIENT_CONNECTED       ("Modbus client is connected.")
#define EVENT_TEXT_MODBUS_CLIENT_UNCONNECTED     ("Modbus client is unconnected.")

class CModbusClientThread : public QThread
{
    Q_OBJECT
public:
    typedef struct {
        QString  server_ip;
        uint     server_port;
        uint     server_id;
        uint     timeout;
        uint     number_of_retries;
        uint     polling_interval; //ms
    }Config;

public:
    CModbusClientThread(Config config,
                   int id,
                   int holding_registers_read_start,
                   int holding_registers_read_numbers,
                   int holding_registers_write_start,
                   int holding_registers_write_numbers,
                   QVector<uint> &holding_registers);
    ~CModbusClientThread();
    const QString *getCurrentErrorText() {return &m_current_error_text;};

signals:
    void eventReport(int id);
    void readFinished();
    void writeFinished();
    void readSuccessfully();
    void writeSuccessfully();

private slots:
    void onTimeout();
    void onWriteReply();
    void onReadReply();
    void onWriteFinished();
    void onReadFinished();
    void onModbusStateChanged(QModbusDevice::State state);
    void onModbusErrorOccurs();

private:
    void run();
    void ReadRequest();
    void WriteRequest();

    int m_id;
    Config m_modbus_config;
    int m_holding_registers_max;
    int m_holding_registers_read_start;
    int m_holding_registers_read_numbers;
    int m_holding_registers_write_start;
    int m_holding_registers_write_numbers;
    bool m_regiesters_initialized;
    QString m_current_error_text;

    QModbusTcpClient *m_modbus_client;
    QTimer *m_timer;
    QVector<uint> &m_holding_registers;
    QVector<uint> *m_holding_registers_backup;
};

#endif
