#ifndef PUMPCURVEDIALOG_H
#define PUMPCURVEDIALOG_H

#include <QDialog>
#include "qcustomplot.h"
#include "pumpunit.h"

namespace Ui {
class PumpCurveDialog;
}

class PumpCurveDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PumpCurveDialog(QWidget *parent = nullptr);
    ~PumpCurveDialog();
    void SetSystemProfile(PumpUnit::SysProfile system_profile);
    void SetPumpPerform(int pump_number, Pump::PerformData pump_perform);
    void Initiation(bool curve_content);
    void ShowIndividualCurve();
    void ShowCombineCurve();

private:
    Ui::PumpCurveDialog     *ui;
    PumpUnit::SysProfile    *m_system_profile;
    Pump::PerformData       *m_pump_perform;

};

#endif // PUMPCURVEDIALOG_H
