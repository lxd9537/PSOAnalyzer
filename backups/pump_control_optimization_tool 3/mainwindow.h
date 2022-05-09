#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <pumpsys.h>
#include <qcustomplot.h>
#include <QtMath>
#include <QDir>
#include "global.h"
#include "qxtcsvmodel.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{

    QCPItemEllipse *m_test_point_ellipse[POINTS_MAX];

    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:

    Ui::MainWindow          *ui;
    bool                    m_assembly_plot_flag;
    double                  m_sensorless_Hz;
    QTimer                  *m_timer_replot;
    void plot_efficiency_curve(double *effi, double *head, long start_flow, long stop_flow, long flow_step, double flow_axis_max, double head_axis_max);

private slots:
    void on_comboBox_pumpmodel_qty_currentIndexChanged(const QString &arg1);
    void on_comboBox_model_number_currentIndexChanged(const QString &arg1);
    void on_comboBox_choose_pump_curve_currentTextChanged(const QString &arg1);
    void on_pushButton_import_pump_curve_clicked();
    void on_pushButton_plot_pump_curve_clicked();
    void on_pushButton_plot_assembly_curve_clicked();
    void on_lineEdit_Hz_editingFinished();
    void on_comboBox_pump_number_2_currentTextChanged(const QString &arg1);
    void on_comboBox_choose_sensorless_curve_currentTextChanged(const QString &arg1);
    void on_pushButton_plot_sensorless_curve_clicked();
    void on_comboBox_pump_number_3_currentTextChanged(const QString &arg1);
    void on_pushButton_plot_vfd_curve_clicked();
    void on_pushButton_Hz_calculation_clicked();
    void on_comboBox_pump_qty_currentTextChanged(const QString &arg1);
    void on_pushButton_import_piping_curve_clicked();
    void on_pushButton_plot_piping_curve_clicked();
    void on_pushButton_show_import_result_clicked();
    void on_pushButton_energy_saving_calculation_clicked();
    void on_lineEdit_target_flow_editingFinished();
    void on_pushButton_demo_clear_clicked();
    void on_pushButton_demo_start_clicked();
    void on_timer_replot();
    void on_pushButton_test_clicked();
};

#endif // MAINWINDOW_H
