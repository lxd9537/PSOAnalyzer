#include "dialog_reportoption.h"
#include "ui_dialog_reportoption.h"

Dialog_ReportOption::Dialog_ReportOption(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_ReportOption)
{
    ui->setupUi(this);
}

Dialog_ReportOption::~Dialog_ReportOption()
{
    delete ui;
}

void Dialog_ReportOption::accept()
{
    if(ui->lineEdit_1->text().toDouble() <= 50 && ui->lineEdit_1->text().toDouble() >= 25)
    {
        m_reportOption.stagingHz = ui->lineEdit_1->text().toDouble();
    }
    else
    {
        QMessageBox::warning(nullptr,"Warning","Staging Hz is greater than 50Hz or less than 25Hz!");
        return;
    }

    if(ui->lineEdit_2->text().toDouble() <= 50 && ui->lineEdit_2->text().toDouble() >= 25
            && ui->lineEdit_2->text().toDouble() < ui->lineEdit_1->text().toDouble())
    {
        m_reportOption.destagingHz = ui->lineEdit_2->text().toDouble();
    }
    else
    {
        QMessageBox::warning(nullptr,"Warning","De-staging Hz is greater than 50Hz or less than 25Hz"
            " or greater-equal than staging Hz!");
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

Dialog_ReportOption::ReportOption Dialog_ReportOption::getReportOption()
{
    return m_reportOption;
}
