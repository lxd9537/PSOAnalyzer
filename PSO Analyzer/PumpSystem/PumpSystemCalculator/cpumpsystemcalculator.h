#ifndef CPUMPSYSTEMCALCULATOR_H
#define CPUMPSYSTEMCALCULATOR_H

#include <QDialog>
#include <QTextDocument>
#include <QTextCursor>
#include <QThread>
#include <QProgressDialog>
#include "PumpSystem/Pump/cpump.h"
#include "PumpSystem/PumpSystem/cpumpsystem.h"
#include "PumpSystem/SystemProfile/csystemprofileinputdialog.h"

namespace Ui {
class CPumpSystemCalculator;
}

class CPumpSystemCalculator : public QDialog
{
    Q_OBJECT

public:
    explicit CPumpSystemCalculator(CPumpSystem &pump_system, QWidget *parent = nullptr);
    ~CPumpSystemCalculator();

private slots:
    void on_startCalculate_clicked();
    void on_terminateCalculate_clicked();
    void onCalculationFinished();
signals:
    void calculateBestCombination(float flow, float head);
private:
    Ui::CPumpSystemCalculator *ui;
    CPumpSystem &ex_pump_system;
    QThread m_thread;

    //泵组计算
    QVector<QPointF> m_flow_head_list;
    int m_point_index;
    bool m_calculate_start;
    QProgressDialog *m_calculate_progress = nullptr;
    QHash<QString, QPointF> m_setpoint_map;

    //报告文本输出
    QTextDocument *m_doc;
    QTextCharFormat m_format_title;
    QTextCharFormat m_format_content;

private:

};

#endif // CPUMPSYSTEMCALCULATOR_H
