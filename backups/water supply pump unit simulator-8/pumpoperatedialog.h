#ifndef PUMPOPERATEDIALOG_H
#define PUMPOPERATEDIALOG_H

#include <QDialog>
#include <pumpunit.h>
#include <doubleinputedit.h>
#include <confirmdialog.h>

namespace Ui {
class PumpOperateDialog;
}

class PumpOperateDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PumpOperateDialog(QWidget *parent = nullptr);
    ~PumpOperateDialog();
    void Initiation(int pump_number, PumpUnit* pump_unit);

private:
    Ui::PumpOperateDialog       *ui;
    int                         m_current_pump_number = 0;
    DoubleInputDialog           *m_input_dialog;
    DoubleInputEdit             *m_current_double_edit;
    ConfirmDialog               *m_confirm_dialog;
    Pump                        *m_pump;

public Q_SLOT:
    void on_PumpStatusChanged(int pump_number,Pump::RwData rw_data);

private Q_SLOT:
    void on_Button_man_clicked();
    void on_Button_off_clicked();
    void on_Button_auto_clicked();
    void on_Button_trip_clicked();
    void on_DoubleInputEdit_clicked();
    void on_DoubleInputEdit_changed(double value);
};

#endif // PUMPOPERATEDIALOG_H
