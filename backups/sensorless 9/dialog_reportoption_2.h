#ifndef DIALOG_REPORTOPTION_2_H
#define DIALOG_REPORTOPTION_2_H

#include <QDialog>
#include <QMessageBox>

namespace Ui {
class Dialog_ReportOption_2;
}

class Dialog_ReportOption_2 : public QDialog
{
    Q_OBJECT

public:
    struct ReportOption{
        double perferedHz;
        double CNYPerKWh;
        double daysInYear;
    };

public:
    explicit Dialog_ReportOption_2(QWidget *parent = nullptr);
    ~Dialog_ReportOption_2();
    ReportOption getReportOption();
    ReportOption m_reportOption;

private:
    Ui::Dialog_ReportOption_2 *ui;

protected:
    virtual void accept();

};

#endif // DIALOG_REPORTOPTION_H
