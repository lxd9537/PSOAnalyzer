#include "pumpoperatedialog.h"
#include "ui_pumpoperatedialog.h"
#include "QMessageBox"
#include <QDoubleSpinBox>

PumpOperateDialog::PumpOperateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PumpOperateDialog)
{
    ui->setupUi(this);

    m_input_dialog          = new DoubleInputDialog(this);
    m_current_double_edit   = ui->lineEdit;
    m_confirm_dialog        = new ConfirmDialog(this);

    connect(ui->Button_man,&QPushButton::clicked,this,&PumpOperateDialog::on_Button_man_clicked);
    connect(ui->Button_off,&QPushButton::clicked,this,&PumpOperateDialog::on_Button_off_clicked);
    connect(ui->Button_auto,&QPushButton::clicked,this,&PumpOperateDialog::on_Button_auto_clicked);
    connect(ui->Button_trip,&QPushButton::clicked,this,&PumpOperateDialog::on_Button_trip_clicked);
    connect(ui->lineEdit,&DoubleInputEdit::clicked,this,&PumpOperateDialog::on_DoubleInputEdit_clicked);
    connect(m_input_dialog,&DoubleInputDialog::ValueChanged,this,&PumpOperateDialog::on_DoubleInputEdit_changed);
}

PumpOperateDialog::~PumpOperateDialog()
{
    delete m_input_dialog;
    delete m_confirm_dialog;
    delete ui;
}

void PumpOperateDialog::Initiation(int pump_number, PumpUnit* pump_unit)
{
    m_current_pump_number = pump_number;
    m_pump = pump_unit->GetOnePump((pump_number));

    QString title("Pump ");
    title += QString::number(m_current_pump_number+1);
    ui->label->setText(title);
    on_PumpStatusChanged(m_current_pump_number,m_pump->ReadRwData());
}

void PumpOperateDialog::on_PumpStatusChanged(int pump_number,Pump::RwData rw_data)
{
    if(m_current_pump_number == pump_number)
    {
        ui->lineEdit->Initiation(rw_data.man_Hz,0,50);

        if(rw_data.trip)
            ui->Button_trip->setStyleSheet("color:red;font: 8pt \"Arial\"");
        else
            ui->Button_trip->setStyleSheet("color:black;font: 8pt \"Arial\"");

        if(rw_data.Moa == Pump::Man)
        {
            ui->Button_man->setStyleSheet("color:red;font: 8pt \"Arial\"");
            ui->Button_off->setStyleSheet("color:black;font: 8pt \"Arial\"");
            ui->Button_auto->setStyleSheet("color:black;font: 8pt \"Arial\"");
        }
        else if (rw_data.Moa == Pump::Off)
        {
            ui->Button_man->setStyleSheet("color:black;font: 8pt \"Arial\"");
            ui->Button_off->setStyleSheet("color:green;font: 8pt \"Arial\"");
            ui->Button_auto->setStyleSheet("color:black;font: 8pt \"Arial\"");
        }
        else
            {
                ui->Button_man->setStyleSheet("color:black;font: 8pt \"Arial\"");
                ui->Button_off->setStyleSheet("color:black;font: 8pt \"Arial\"");
                ui->Button_auto->setStyleSheet("color:green;font: 8pt \"Arial\"");
            }
    }
}
void PumpOperateDialog::on_Button_man_clicked()
{
    m_confirm_dialog->SetTipText("Are you sure to set to MANUAL?");
    int result = m_confirm_dialog->exec();
    if (result == QDialog::Accepted)
    {
        Pump::RwData rw_data = m_pump->ReadRwData();
        rw_data.Moa=Pump::Man;
        m_pump->WriteRWData(rw_data);
    }
    else if (result == QDialog::Rejected)
            return;

}
void PumpOperateDialog::on_Button_off_clicked()
{
    m_confirm_dialog->SetTipText("Are sure to set to OFF?");
    int result = m_confirm_dialog->exec();
    if (result == QDialog::Accepted)
    {
        Pump::RwData rw_data = m_pump->ReadRwData();
        rw_data.Moa=Pump::Off;
        m_pump->WriteRWData(rw_data);
    }
     else if (result == QDialog::Rejected)
            return;

}
void PumpOperateDialog::on_Button_auto_clicked()
{
    m_confirm_dialog->SetTipText("Are you sure to set AUTO?");
    int result = m_confirm_dialog->exec();
    if (result == QDialog::Accepted)
    {
        Pump::RwData rw_data = m_pump->ReadRwData();
        rw_data.Moa=Pump::Auto;
        m_pump->WriteRWData(rw_data);
    }
    else if (result == QDialog::Rejected)
            return;

}
void PumpOperateDialog::on_Button_trip_clicked()
{
    m_confirm_dialog->SetTipText("Are you sure to toggle TRIP?");
    int result = m_confirm_dialog->exec();
    if (result == QDialog::Accepted)
    {
        Pump::RwData rw_data = m_pump->ReadRwData();
        rw_data.trip=!rw_data.trip;
        m_pump->WriteRWData(rw_data);
    }
    else if (result == QDialog::Rejected)
            return;

}
void PumpOperateDialog::on_DoubleInputEdit_clicked()
{
    m_current_double_edit = static_cast<DoubleInputEdit*>(sender());
    m_input_dialog->Initiation(
                m_current_double_edit->GetCurrentValue(),
                m_current_double_edit->GetMinValue(),
                m_current_double_edit->GetMaxValue(),
                m_current_double_edit->GetDisplayFormat(),
                m_current_double_edit->GetDisplayPrecision());

    m_input_dialog->show();
}
void PumpOperateDialog::on_DoubleInputEdit_changed(double value)
{
    m_current_double_edit->SetCurrentValue(value);
    Pump::RwData rw_data = m_pump->ReadRwData();
    rw_data.man_Hz=value;
    m_pump->WriteRWData(rw_data);
}
