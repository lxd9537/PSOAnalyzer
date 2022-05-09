#include "cpumppanel.h"
#include "ui_cpumppanel.h"

CPumpPanel::CPumpPanel(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CPumpPanel)
{
    ui->setupUi(this);
}

CPumpPanel::~CPumpPanel()
{
    delete ui;
}

void CPumpPanel::valueUpdate(const PumpPanelValue& values) {
    if (values.running != m_values.running || values.fault != m_values.fault || !m_init_flag) {
        if (values.fault) {
            ui->pump_status->setText("故障");
            ui->pump_status->setStyleSheet("background-color: rgb(255, 255, 0);");
        } else if (values.running) {
            ui->pump_status->setText("运行");
            ui->pump_status->setStyleSheet("background-color: rgb(0, 255, 0);");
        } else {
            ui->pump_status->setText("停止");
            ui->pump_status->setStyleSheet("background-color: rgb(255, 255, 255);");
        }
    }
    if (values.opened != m_values.opened || values.closed != m_values.closed || !m_init_flag) {
        if (values.opened) {
            ui->valve_status->setText("全开");
            ui->valve_status->setStyleSheet("background-color: rgb(0, 255, 0);");
        } else if (values.closed) {
            ui->valve_status->setText("全关");
            ui->valve_status->setStyleSheet("background-color: rgb(255, 255, 255);");
        } else {
            ui->valve_status->setText("中间位置");
            ui->valve_status->setStyleSheet("background-color: rgb(255, 255, 255);");
        }
    }
    if (values.freq != m_values.freq || !m_init_flag)
        ui->frequency->setText(QString::number(values.freq / 10.0, 'f', 1));
    if (values.speed != m_values.speed || !m_init_flag)
        ui->speed->setText(QString::number(values.speed));
    if (values.voltage != m_values.voltage || !m_init_flag)
        ui->voltage->setText(QString::number(values.voltage));
    if (values.current != m_values.current || !m_init_flag)
        ui->current->setText(QString::number(values.current));
    if (values.power != m_values.power || !m_init_flag)
        ui->power->setText(QString::number(values.power / 10.0, 'f', 1));
    if (values.pressure_in != m_values.pressure_in || !m_init_flag)
        ui->pressure_in->setText(QString::number(values.pressure_in / 10000.0, 'f', 3));
    if (values.pressure_out != m_values.pressure_out || !m_init_flag)
        ui->pressure_out->setText(QString::number(values.pressure_out / 10000.0, 'f', 3));
    if (values.flow != m_values.flow || !m_init_flag)
        ui->flow->setText(QString::number(values.flow));
    if (values.effi != m_values.effi || !m_init_flag)
        ui->effi->setText(QString::number(values.effi / 10.0, 'f', 1));

    m_values = values;
}

void CPumpPanel::setValidity(bool validity) {
    m_value_avai_flag = validity;
    if (!m_value_avai_flag) {
        ui->pump_status->clear();
        ui->pump_status->setStyleSheet("background-color: rgb(255, 255, 255);");
        ui->valve_status->clear();
        ui->valve_status->setStyleSheet("background-color: rgb(255, 255, 255);");
        ui->frequency->clear();
        ui->speed->clear();
        ui->voltage->clear();
        ui->current->clear();
        ui->power->clear();
        ui->pressure_in->clear();
        ui->pressure_out->clear();
        ui->flow->clear();
        ui->effi->clear();
    }
}

void CPumpPanel::setPumpNum(int num) {
    ui->pump_num->setText(QString::number(num));
}
