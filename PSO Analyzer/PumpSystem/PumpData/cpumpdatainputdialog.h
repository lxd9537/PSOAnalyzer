#ifndef CPUMPDATAINPUTDIALOG_H
#define CPUMPDATAINPUTDIALOG_H

#include <QDialog>
#include <QKeyEvent>
#include <QDebug>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QTimer>
#include "PumpSystem/Pump/cpump.h"

namespace Ui {
class CPumpDataInputDialog;
}

class CPumpDataInputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CPumpDataInputDialog(QWidget *parent = nullptr);
    ~CPumpDataInputDialog();

    void init(const CPump::PerformTestData &perform_data);
    void setViewOnly();
    CPump::PerformTestData getData();

protected:
    void keyPressEvent(QKeyEvent *event);

private slots:
    void on_tableWidget_itemChanged(QTableWidgetItem *item);
    void on_maxRows_valueChanged(int arg1);

private:

    Ui::CPumpDataInputDialog *ui;

    double m_flow[POINTS_MAX];
    double m_head[POINTS_MAX];
    double m_power[POINTS_MAX];
    double m_npshr[POINTS_MAX];
    double m_effi[POINTS_MAX];
};

#endif // CPUMPDATAINPUTDIALOG_H
