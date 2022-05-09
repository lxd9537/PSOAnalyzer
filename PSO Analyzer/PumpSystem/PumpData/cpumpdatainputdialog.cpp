#include "cpumpdatainputdialog.h"
#include "ui_cpumpdatainputdialog.h"
#include <QColor>

CPumpDataInputDialog::CPumpDataInputDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CPumpDataInputDialog)
{
    ui->setupUi(this);

    QFont font("Microsoft YaHei", 11);
    this->setFont(font);
    this->setWindowTitle("水泵性能数据");
    this->setFixedSize(this->width(), this->height());
    this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);

    ui->tableWidget->horizontalHeader()->setStyleSheet("font: 700 9pt \"Microsoft JhengHei UI\";");
}

CPumpDataInputDialog::~CPumpDataInputDialog()
{
    delete ui;
}

void CPumpDataInputDialog::init(const CPump::PerformTestData &perform_data) {
    ui->model->setText(perform_data.model);
    if (perform_data.points < 5)
        ui->maxRows->setValue(5);
    else if (perform_data.points > 10)
        ui->maxRows->setValue(10);
    else
        ui->maxRows->setValue(perform_data.points);

    ui->motorPower->setValue(perform_data.motor_power);
    ui->minFlow->setValue(perform_data.flow_min);
    ui->maxFlow->setValue(perform_data.flow_max);

    for (int i=0; i<POINTS_MAX; i++) {
        m_flow[i] = 0;
        m_head[i] = 0;
        m_power[i] = 0;
        m_npshr[i] = 0;
        m_effi[i] = 0;
    }
    ui->tableWidget->blockSignals(true);
    if (perform_data.points > ui->tableWidget->rowCount())
        ui->tableWidget->insertRow(perform_data.points - ui->tableWidget->rowCount());
    int rows = ui->tableWidget->rowCount();
    for (int i=0; i<rows; i++) {
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setText(QString::number(perform_data.flow[i], 'f', 2));
        m_flow[i] = perform_data.flow[i];
        ui->tableWidget->setItem(i,0,item);
    }
    for (int i=0; i<rows; i++) {
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setText(QString::number(perform_data.head[i], 'f', 2));
        m_head[i] = perform_data.head[i];
        ui->tableWidget->setItem(i,1,item);
    }
    for (int i=0; i<rows; i++) {
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setText(QString::number(perform_data.power[i], 'f', 2));
        m_power[i] = perform_data.power[i];
        ui->tableWidget->setItem(i,2,item);
    }
    for (int i=0; i<rows; i++) {
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setText(QString::number(perform_data.npshr[i], 'f', 2));
        m_npshr[i] = perform_data.npshr[i];
        ui->tableWidget->setItem(i,3,item);
    }
    for (int i=0; i<rows; i++) {
        m_effi[i] = CPump::effi(m_flow[i], m_head[i], m_power[i]);;
    }

    ui->tableWidget->blockSignals(false);
};

void CPumpDataInputDialog::setViewOnly() {
    ui->buttonBox->setStandardButtons(QDialogButtonBox::Close);
    ui->buttonBox->setCenterButtons(true);
}

CPump::PerformTestData CPumpDataInputDialog::getData() {

    CPump::PerformTestData perform_data;
    perform_data.model = ui->model->text();
    perform_data.points = ui->maxRows->value();
    perform_data.motor_power = ui->motorPower->value();
    perform_data.flow_min = ui->minFlow->value();
    perform_data.flow_max = ui->maxFlow->value();
    for (int i=0; i<POINTS_MAX; i++) {
        perform_data.flow[i] = m_flow[i];
        perform_data.head[i] = m_head[i];
        perform_data.power[i] = m_power[i];
        perform_data.npshr[i] = m_npshr[i];
        perform_data.effi[i] = m_effi[i];
    }

    return perform_data;
}

void CPumpDataInputDialog::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Enter) {
        return;
    }
    QWidget::keyPressEvent(event);
}

void CPumpDataInputDialog::on_tableWidget_itemChanged(QTableWidgetItem *item)
{
    if (item->row() < 0 || item->row() > 9 || item->column() < 0 || item->column() > 3)
        return;
    if (item->text() == "")
        return;

    bool ok;
    float f = item->text().toFloat(&ok);
    if (ok && f >= 0 && f < 99999) {
        switch(item->column()) {
        case 0: m_flow[item->row()] = f; break;
        case 1: m_head[item->row()] = f; break;
        case 2: m_power[item->row()] = f; break;
        case 3: m_npshr[item->row()] = f; break;
        }
        m_effi[item->row()] = CPump::effi(m_flow[item->row()], m_head[item->row()], m_power[item->row()]);
    }
    else {
        item->setText("");
        switch(item->column()) {
        case 0: m_flow[item->row()] = 0; break;
        case 1: m_head[item->row()] = 0; break;
        case 2: m_power[item->row()] = 0; break;
        case 3: m_npshr[item->row()] = 0; break;
        }
        m_effi[item->row()] = 0;
    }
}


void CPumpDataInputDialog::on_maxRows_valueChanged(int arg1)
{
    if (arg1 > ui->tableWidget->rowCount()) {
        for (int i = ui->tableWidget->rowCount(); i<arg1; i++) {
            ui->tableWidget->insertRow(i);
            for (int j=0; j<4; j++) {
                QTableWidgetItem *item = new QTableWidgetItem();
                item->setText("");
                ui->tableWidget->setItem(i,j,item);
            }
        }
    } else if (arg1 < ui->tableWidget->rowCount()) {
        for (int i = ui->tableWidget->rowCount() - 1; i > arg1 - 1; i--)
            ui->tableWidget->removeRow(i);
    }
}

