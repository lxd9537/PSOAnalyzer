#ifndef SETVFDMODELDIALOG_H
#define SETVFDMODELDIALOG_H

#include <QDialog>
#include "pumpbooster.h"

namespace Ui {
class InputLineEditDialog;
}

class InputLineEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InputLineEditDialog(QWidget *parent = nullptr);
    ~InputLineEditDialog();
    void Initiation(double current_value,double min_value,double max_value);
    void Initiation(int current_value,int min_value,int max_valuee);
    void Initiation(long current_value,long min_value,long max_value);

public Q_SLOTS:
    void on_applyButton_clicked();

signals:
    void VfdModelChanged(PumpBooster::VfdModel vfd_model);

private:
    Ui::InputLineEditDialog *ui;
    double m_min_value_double,m_max_value_double;
    int m_min_value_int,m_max_value_int;
    long m_min_value_long,m_max_value_long;
    bool CheckMin(double value);
    bool CheckMin(int value);
    bool CheckMin(long value);
    bool CheckMax(double value);
    bool CheckMax(int value);
    bool CheckMax(long value);
};

#endif // SETVFDMODELDIALOG_H
