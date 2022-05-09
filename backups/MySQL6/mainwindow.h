#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <pump_db.h>
#include <dialog_getdbpara.h>
#include <pumpsys.h>
#include <qcustomplot.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{

    struct systemDesign{
      int       dutyPumps;
      double    totalFlow;
      double    headAtZeroFlow;
      double    headAtTotalFlow;
      double    controlCurveShaping;
    };

    QColor m_color[12]={
        QColor(200,30,30),      //red
        QColor(30,200,30),      //green
        QColor(30,30,200),      //blue
        QColor(200,150,30),     //orange
        QColor(150,75,150),     //purple
        QColor(0,0,0),          //black
        QColor(200,30,175),     //pink
        QColor(125,125,125),     //gray
        QColor(0,255,255),
        QColor(0,128,255),
        QColor(64,128,128),
        QColor(128,128,0),
    };

    struct pumpActualPerform{
        int pumps;
        int infor;
        double total_flow;
        double head;
        double Hz;
        double axis_power;
        double input_power;
        double efficiency;
    };

    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:

    void on_SetDbPara_clicked();
    void on_connect_clicked();
    void on_family_changed();
    void on_model_changed();
    void on_speed_changed();
    void on_motor_changed();
    void on_multi_pump_clicked();
    void on_single_pump_clicked();
    void on_SaveAs_clicked();
    void on_Load_clicked();

    void get_pump_selection();
    void get_motor_selection();
    void get_system_design();

    void show_multi_pump_curve();
    void show_single_pump_curve();
    bool get_system_config_from_ui();
    bool get_pump_config_from_ui();

    bool get_actual_perform(int pumps,double flow, double head, pumpActualPerform *perform);
    bool get_actual_perform_list();
    bool get_best_effi();
    bool get_fix_effi();



    Ui::MainWindow          *ui;
    Dialog_GetDbPara        *m_dialog_getdbpara;
    Pump_db                 m_pumpDb;
    Pump                    m_pump;
    PumpSys                 m_pumpSys;
    Pump_db::pumpSelection  m_pumpSelection;
    Pump_db::motorSelection m_motorSelection;
    PumpSys::PumpSysPara    m_sysPara;

    pumpActualPerform       m_pumpActualPerform[6][101];
    int                     m_best_pump_qty[101];
    double                  m_best_effi[101];
    double                  m_best_Hz[101];

    int                     m_fix_pump_qty_1[101];
    int                     m_fix_pump_qty_2[101];
    double                  m_fix_effi_1[101];
    double                  m_fix_effi_2[101];
    double                  m_fix_Hz_1[101];
    double                  m_fix_Hz_2[101];

protected:


};

#endif // MAINWINDOW_H
