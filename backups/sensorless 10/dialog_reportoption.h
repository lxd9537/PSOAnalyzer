#ifndef DIALOG_REPORTOPTION_H
#define DIALOG_REPORTOPTION_H

#include <QDialog>
#include <QMessageBox>

namespace Ui {
class Dialog_ReportOption;
}

class Dialog_ReportOption : public QDialog
{
    Q_OBJECT

public:
    struct ReportOption{
        QString client;
        QString project;
        QString report_number;
        double cost_per_kWh;
        double days_per_year;
    };

public:
    explicit Dialog_ReportOption(QWidget *parent = nullptr);
    ~Dialog_ReportOption();
    ReportOption getReportOption();
    void setReportOption(ReportOption *reportOption);
    ReportOption m_reportOption;

private:
    Ui::Dialog_ReportOption *ui;

protected:
    virtual void accept();

};

#endif // DIALOG_REPORTOPTION_H
