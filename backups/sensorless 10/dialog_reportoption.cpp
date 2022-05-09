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
    if(ui->lineEdit_days_per_year->text().toDouble()>0 and ui->lineEdit_days_per_year->text().toDouble()<=366)
        m_reportOption.days_per_year = ui->lineEdit_days_per_year->text().toDouble();
    else
    {
        QMessageBox::warning(nullptr,"Warning","Invalid input!");
        return;
    }

    if(ui->lineEdit_cost_per_kwh->text().toDouble()>0)
        m_reportOption.cost_per_kWh = ui->lineEdit_cost_per_kwh->text().toDouble();
    else
    {
        QMessageBox::warning(nullptr,"Warning","Invalid input!");
        return;
    }

    m_reportOption.client = ui->lineEdit_client->text();
    m_reportOption.project = ui->lineEdit_project->text();
    m_reportOption.report_number = ui->lineEdit_report_number->text();

    QDialog::accept();
}

Dialog_ReportOption::ReportOption Dialog_ReportOption::getReportOption()
{
    return m_reportOption;
}

void Dialog_ReportOption::setReportOption(ReportOption *reportOption)
{
    m_reportOption = *reportOption;
    ui->lineEdit_client->setText(m_reportOption.client);
    ui->lineEdit_project->setText(m_reportOption.project);
    ui->lineEdit_report_number->setText(m_reportOption.report_number);
    ui->lineEdit_cost_per_kwh->setText(QString::number(m_reportOption.cost_per_kWh));
    ui->lineEdit_days_per_year->setText(QString::number(m_reportOption.days_per_year));
}
