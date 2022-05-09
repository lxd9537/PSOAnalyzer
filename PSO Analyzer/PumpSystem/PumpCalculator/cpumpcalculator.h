#ifndef CPUMPCALCULATOR_H
#define CPUMPCALCULATOR_H

#include <QDialog>
#include <Database/MySql/cmysqlthread.h>
#include <PumpSystem/Pump/cpump.h>

namespace Ui {
class CPumpCalculator;
}

class CPumpCalculator : public QDialog
{
    Q_OBJECT
public:

public:
    explicit CPumpCalculator(CMySqlThread *mysql_thread, QWidget *parent = nullptr);
    ~CPumpCalculator();

private slots:

    void on_calculationTypeList_currentTextChanged(const QString &arg1);

    void on_calculateButton_clicked();

private:
    Ui::CPumpCalculator *ui;
    CMySqlThread *m_mysql_thread;
    CPump::CalculateType m_calculate_type;

};

#endif // CPUMPCALCULATOR_H
