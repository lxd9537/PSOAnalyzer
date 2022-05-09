#include "inputlineeditdialog.h"
#include "ui_inputlineeditdialog.h"

InputLineEditDialog::InputLineEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InputLineEditDialog)
{
    ui->setupUi(this);

    //connections
    connect(ui->cancelButton, &QPushButton::clicked, [this]() {hide();});
    connect(ui->applyButton, &QPushButton::clicked, this, &InputLineEditDialog::on_applyButton_clicked);
}

InputLineEditDialog::~InputLineEditDialog()
{
    delete ui;
}
void InputLineEditDialog::Initiation(double current_value,double min_value,double max_value)
{
    QString str = QString::number(current_value);
    ui->lineEdit->setText(str);
    m_min_value_double=min_value;
    m_max_value_double=max_value;

}

void InputLineEditDialog::Initiation(int current_value,int min_value,int max_value)
{
    QString str = QString::number(current_value);
    ui->lineEdit->setText(str);
    m_min_value_int=min_value;
    m_max_value_int=max_value;
}

void InputLineEditDialog::Initiation(long current_value,long min_value,long max_value)
{
    QString str = QString::number(current_value);
    ui->lineEdit->setText(str);
    m_min_value_long=min_value;
    m_max_value_long=max_value;
}



void InputLineEditDialog::on_applyButton_clicked()
{
    PumpBooster::VfdModel current_vfd_model;
    switch(ui->setmodelCombo->currentIndex())
    {
    case 0: current_vfd_model = PumpBooster::FC102;break;
    case 1: current_vfd_model = PumpBooster::ATV610;break;
    case 2: current_vfd_model = PumpBooster::ACQ580;break;
    case 3: current_vfd_model = PumpBooster::Hydrovar;break;
    default: current_vfd_model = PumpBooster::FC102;
    }

    emit VfdModelChanged(current_vfd_model);
    hide();
}

