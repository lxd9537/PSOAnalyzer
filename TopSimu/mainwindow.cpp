#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <config.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    CModbusServerThread::Config modbus_server_config;

    if (!loadConfigFromIni(modbus_server_config)) {
        qDebug()<<"Load configuration from config.ini failed.";
    } else {
        /* create a modbus server thread */
        m_modbus_server_thread = new CModbusServerThread(modbus_server_config,
                                                         1,
                                                         HOLDING_REGISTERS_SERVER_START_ADDR,
                                                         HOLDING_REGISTERS_SERVER_NUMBERS);
        if (m_modbus_server_thread) {
            m_modbus_server_thread->start();
            connect(m_modbus_server_thread, &CModbusServerThread::eventReport, this, &MainWindow::onEventReported);
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;

    delete m_modbus_server_thread;
}

void MainWindow::onEventReported(int id, int code) {

}

