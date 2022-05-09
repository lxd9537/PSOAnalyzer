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
        double stagingHz;
        double destagingHz;
        double CNYPerKWh;
        double daysInYear;
    };

public:
    explicit Dialog_ReportOption(QWidget *parent = nullptr);
    ~Dialog_ReportOption();
    ReportOption getReportOption();
    ReportOption m_reportOption;

private:
    Ui::Dialog_ReportOption *ui;

protected:
    virtual void accept();

};

#endif // DIALOG_REPORTOPTION_H
