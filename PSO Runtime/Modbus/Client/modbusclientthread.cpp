#include "modbusclientthread.h"
#include <QtGlobal>

CModbusClientThread::CModbusClientThread(Config config,
                               int id,
                               int holding_registers_read_start,
                               int holding_registers_read_numbers,
                               int holding_registers_write_start,
                               int holding_registers_write_numbers,
                               QVector<uint> &holding_registers):m_holding_registers(holding_registers)
{
    m_id = id;
    m_modbus_config = config;

    Q_ASSERT(holding_registers_read_numbers > 0);
    Q_ASSERT(holding_registers_write_numbers > 0);
    m_holding_registers_max = qMax(holding_registers_read_start + holding_registers_read_numbers,
                                   holding_registers_write_start + holding_registers_write_numbers);
    m_holding_registers_read_start = holding_registers_read_start;
    m_holding_registers_read_numbers = holding_registers_read_numbers;
    m_holding_registers_write_start = holding_registers_write_start;
    m_holding_registers_write_numbers = holding_registers_write_numbers;
    m_holding_registers_backup = new QVector<uint>(m_holding_registers_max, 0);

    /* create a modbus client */
    m_modbus_client = new QModbusTcpClient();
    if (m_modbus_client) {
        m_modbus_client->setConnectionParameter(QModbusDevice::NetworkAddressParameter, m_modbus_config.server_ip);
        m_modbus_client->setConnectionParameter(QModbusDevice::NetworkPortParameter, m_modbus_config.server_port);
        m_modbus_client->setTimeout(m_modbus_config.timeout);
        m_modbus_client->setNumberOfRetries(m_modbus_config.number_of_retries);
        connect(m_modbus_client, &QModbusClient::errorOccurred, this, &CModbusClientThread::onModbusErrorOccurs);
        connect(m_modbus_client, &QModbusClient::stateChanged, this, &CModbusClientThread::onModbusStateChanged);
        connect(this, &CModbusClientThread::writeFinished, this, &CModbusClientThread::onWriteFinished);
        connect(this, &CModbusClientThread::readFinished, this, &CModbusClientThread::onReadFinished);
        m_modbus_client->connectDevice();
    } else
        qDebug()<<"Create modbus client failed.";

    m_timer = new QTimer();
    qDebug()<<"Mobus client: timer is created.";
    if (m_modbus_client && m_timer) {
        m_timer->setInterval(m_modbus_config.polling_interval);
        m_timer->start();
        connect(m_timer, &QTimer::timeout, this, &CModbusClientThread::onTimeout);
    }
    m_regiesters_initialized = false;
}

CModbusClientThread::~CModbusClientThread() {
    m_modbus_client->disconnectDevice();
    delete m_modbus_client;
    delete m_holding_registers_backup;
}

void CModbusClientThread::onTimeout() {
    /* reconnection */
    static uint disconnect_count;
    if (m_modbus_client->state() == QModbusDevice::UnconnectedState) {
        if (disconnect_count ++ > MODBUS_CLIENT_RECONNECT_DELAY) {
            disconnect_count = 0;
            qDebug()<<"Modbus client: try to connect.";
            m_modbus_client->connectDevice();
        }
        return;
    }
    m_timer->stop();
    qDebug()<<"Modbus client: timer stopped.";
    WriteRequest();
}

void CModbusClientThread::onWriteReply() {
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (reply) {
        if (reply->error() == QModbusDevice::NoError) {
            emit void writeSuccesfully();
            qDebug()<<"Modbus client: write finished with no error";
        } else {
            m_current_error_text = reply->errorString();
            emit eventReport(m_id);
            qDebug()<<"Modbus client: write finished with error.";
        }
        reply->deleteLater();
    }
    emit writeFinished();
}

void CModbusClientThread::onReadReply() {
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (reply) {
        if (reply->error() == QModbusDevice::NoError) {
            const QModbusDataUnit unit = reply->result();
            int total = int(unit.values().count());
            for (int i = 0; i < total; i++) {
                uint index = unit.startAddress() + i;
                Q_ASSERT(index < (uint)m_holding_registers_max);
                m_holding_registers[index] = unit.value(i);
                (*m_holding_registers_backup)[index] = unit.value(i);
            }
            m_regiesters_initialized = true;
            emit readSuccessfully();
            qDebug()<<"Modbus client: read finished with no error";
        } else {
            m_current_error_text = reply->errorString();
            emit eventReport(m_id);
            qDebug()<<"Modbus client: read finished with error.";
        }
        reply->deleteLater();
    }
    emit readFinished();
}

void CModbusClientThread::onWriteFinished() {
    ReadRequest();
}

void CModbusClientThread::onReadFinished() {
    m_timer->start();
    qDebug()<<"Modbus client: timer re-starts.";
}

void CModbusClientThread::onModbusStateChanged(QModbusDevice::State state) {
    qDebug()<<"Modbus client connection state is changed. Current state is:"<<state;
    switch (state) {
    case QModbusDevice::ClosingState:
        m_current_error_text = EVENT_TEXT_MODBUS_CLIENT_CLOSING;
        emit eventReport(m_id);
        break;
    case QModbusDevice::ConnectingState:
        m_current_error_text = EVENT_TEXT_MODBUS_CLIENT_CONNECTING;
        emit eventReport(m_id);
        break;
    case QModbusDevice::ConnectedState:
        m_current_error_text = EVENT_TEXT_MODBUS_CLIENT_CONNECTED;
        emit eventReport(m_id);
        break;
    case QModbusDevice::UnconnectedState:
        m_current_error_text = EVENT_TEXT_MODBUS_CLIENT_UNCONNECTED;
        emit eventReport(m_id);
        break;
    }
}
void CModbusClientThread::onModbusErrorOccurs() {
    m_current_error_text = m_modbus_client->errorString();
    emit eventReport(m_id);
}

void CModbusClientThread::run() {

    exec();
}

void CModbusClientThread::ReadRequest() {
    qDebug()<<"Modbus client read request.";
    QModbusDataUnit data_unit;
    data_unit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters,
                                m_holding_registers_read_start,
                                m_holding_registers_read_numbers);
    auto *reply = m_modbus_client->sendReadRequest(data_unit, m_modbus_config.server_id);
    if (reply) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, &CModbusClientThread::onReadReply);
            return;
        }
        else
            reply->deleteLater(); // broadcast replies return immediatel
    }
    emit readFinished();
}

void CModbusClientThread::WriteRequest() {
    qDebug()<<"Modbus client write request.";
    QModbusDataUnit data_unit;
    data_unit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, m_holding_registers_write_start, m_holding_registers_write_numbers);
    bool any_change = false;
    if (m_regiesters_initialized) {
        for (int i = 0; i < m_holding_registers_write_numbers; i++) {
            if (m_holding_registers.at(i + m_holding_registers_write_start)
                    != m_holding_registers_backup->at(i + m_holding_registers_write_start)) {
                any_change = true;
                break;
            }
        }
    }
    if (any_change) {
        for (int i = 0; i < m_holding_registers_write_numbers; i++)
            data_unit.setValue(i, m_holding_registers[i + m_holding_registers_write_start]);
        auto *reply = m_modbus_client->sendWriteRequest(data_unit, m_modbus_config.server_id);
        if (reply) {
            if (!reply->isFinished()) {
                connect(reply, &QModbusReply::finished, this, &CModbusClientThread::onWriteReply);
                return;
            } else
                reply->deleteLater();
        }
    }
    emit writeFinished();
}

