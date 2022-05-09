#include "dialog_reportoption_2.h"
#include "ui_dialog_reportoption_2.h"

Dialog_ReportOption_2::Dialog_ReportOption_2(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_ReportOption_2)
{
    ui->setupUi(this);
}

Dialog_ReportOption_2::~Dialog_ReportOption_2()
{
    delete ui;
}

void Dialog_ReportOption_2::accept()
{
    if(ui->lineEdit_1->text().toDouble() <= 50 && ui->lineEdit_1->text().toDouble() >= 25)
    {
        m_reportOption.perferedHz = ui->lineEdit_1->text().toDouble();
    }
    else
    {
        QMessageBox::warning(nullptr,"Warning","Staging Hz is greater than 50Hz or less than 25Hz!");
        return;
    }

    if(ui->lineEdit_3->text().toDouble() > 0 && ui->lineEdit_3->text().toDouble() < 9999)
    {
        m_reportOption.CNYPerKWh = ui->lineEdit_3->text().toDouble();
    }
    else
    {
        QMessageBox::warning(nullptr,"Warning","CNY per kWh is greater than 9999 or less-equal than 0");
        return;
    }

    if(ui->lineEdit_4->text().toDouble() > 0 && ui->lineEdit_3->text().toDouble() < 365)
    {
        m_reportOption.daysInYear = ui->lineEdit_4->text().toDouble();
    }
    else
    {
        QMessageBox::warning(nullptr,"Warning","Days in a year is greater than 365 or less-equal than 0");
        return;
    }

    QDialog::accept();
}

Dialog_ReportOption_2::ReportOption Dialog_ReportOption_2::getReportOption()
{
    return m_reportOption;
}
