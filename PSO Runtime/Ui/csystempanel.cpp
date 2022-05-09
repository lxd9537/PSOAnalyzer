#include "csystempanel.h"
#include "ui_csystempanel.h"

CSystemPanel::CSystemPanel(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CSystemPanel)
{
    ui->setupUi(this);
}

CSystemPanel::~CSystemPanel()
{
    delete ui;
}

void CSystemPanel::valueUpdate(const SystemPanelValue& values) {
    if (values.level != m_values.level || !m_init_flag)
        ui->level->setText(QString::number(values.level / 100.0, 'f', 2));
    if (values.press != m_values.press || !m_init_flag)
        ui->press->setText(QString::number(values.press));
    if (values.press_set != m_values.press_set || !m_init_flag)
        ui->press_set->setText(QString::number(values.press_set));
    if (values.flow != m_values.flow || !m_init_flag)
        ui->flow->setText(QString::number(values.flow));

    m_values = values;
}

void CSystemPanel::setValidity(bool validity) {
    m_value_avai_flag = validity;
    if (!m_value_avai_flag) {
        ui->level->clear();
        ui->press->clear();
        ui->press_set->clear();
        ui->flow->clear();
    }
}

void CSystemPanel::setSystemNum(int num) {
    ui->pump_num->setText(QString::number(num));
}
