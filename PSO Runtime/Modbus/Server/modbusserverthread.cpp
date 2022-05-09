#include "modbusserverthread.h"

CModbusServerThread::CModbusServerThread(Config config,
                                        int id,
                                        int holding_registers_start,
                                        int holding_registers_numbers) {
    m_id = id;
    m_modbus_config = config;

    Q_ASSERT(holding_registers_numbers > 0);
    holding_registers_max = holding_registers_start + holding_registers_numbers;

    /* create a modbus server */
    m_modbus_server = new QModbusTcpServer();
    if (m_modbus_server) {
        m_modbus_server->setConnectionParameter(QModbusDevice::NetworkAddressParameter, m_modbus_config.server_ip);
        m_modbus_server->setConnectionParameter(QModbusDevice::NetworkPortParameter, m_modbus_config.server_port);
        m_modbus_server->setServerAddress(m_modbus_config.server_id);
        connect(m_modbus_server, &QModbusServer::errorOccurred, this, &CModbusServerThread::onModbusErrorOccurs);
        connect(m_modbus_server, &QModbusServer::dataWritten, this, &CModbusServerThread::onDataWritten);
        connect(m_modbus_server, &QModbusServer::stateChanged, this, &CModbusServerThread::onModbusStateChanged);
        QModbusDataUnitMap reg;
        reg.insert(QModbusDataUnit::HoldingRegisters, {QModbusDataUnit::HoldingRegisters,
                                                       holding_registers_start,
                                                       (quint16)holding_registers_numbers });
        m_modbus_server->setMap(reg);
        m_modbus_server->connectDevice();

        m_timer = new QTimer();
        if (m_modbus_server && m_timer) {
            m_timer->setInterval(m_modbus_config.update_interval);
            m_timer->start();
            connect(m_timer, &QTimer::timeout, this, &CModbusServerThread::onTimeout);
        };
    }
}

CModbusServerThread::~CModbusServerThread() {
    m_modbus_server->disconnectDevice();
    delete m_modbus_server;
    delete m_timer;
}
void CModbusServerThread::run() {

    exec();
}

void CModbusServerThread::onTimeout() {
    /* reconnection */
    static uint disconnect_count;
    if (m_modbus_server->state() == QModbusDevice::UnconnectedState) {
        if (disconnect_count ++ > MODBUS_SERVER_RECONNECT_DELAY) {
            disconnect_count = 0;
            m_modbus_server->connectDevice();
        }
        return;
    }
}

bool CModbusServerThread::SetHoldingRegisters(uint start_address, uint numbers, const QVector<uint> &holding_registers) {
    if (start_address + numbers > holding_registers_max)
        return false;

    for (uint i=0; i<numbers; i++) {
        m_modbus_server->setData(QModbusDataUnit::HoldingRegisters, start_address + i, holding_registers[i]);
    }
    return true;
}

bool CModbusServerThread::GetHoldingRegisters(uint start_address, uint numbers, QVector<uint> &holding_registers) {
    if (start_address + numbers > holding_registers_max)
        return false;

    for (uint i=0; i<numbers; i++) {
        m_modbus_server->data(QModbusDataUnit::HoldingRegisters,
                                       (quint16)(start_address + i), (quint16*)&holding_registers[i]);
    }
    return false;
}
void CModbusServerThread::onDataWritten(int address, int size) {
    emit dataWritten(address, size);
}
void CModbusServerThread::onModbusStateChanged(QModbusDevice::State state) {
    qDebug()<<"Connect state is changed. Current state is:"<<state;
    switch (state) {
    case QModbusDevice::ClosingState:
        m_current_error_text = EVENT_TEXT_MODBUS_SERVER_CLOSING;
        emit eventReport(m_id);
        break;
    case QModbusDevice::ConnectingState:
        m_current_error_text = EVENT_TEXT_MODBUS_SERVER_CONNECTING;
        emit eventReport(m_id);
        break;
    case QModbusDevice::ConnectedState:
        m_current_error_text = EVENT_TEXT_MODBUS_SERVER_CONNECTED;
        emit eventReport(m_id);
        break;
    case QModbusDevice::UnconnectedState:
        m_current_error_text = EVENT_TEXT_MODBUS_SERVER_UNCONNECTED;
        emit eventReport(m_id);
        break;
    }
}

void CModbusServerThread::onModbusErrorOccurs() {
    m_current_error_text = m_modbus_server->errorString();
    emit eventReport(m_id);
}
