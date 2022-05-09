#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <pump_db.h>
#include <dialog_getdbpara.h>
#include <pumpsys.h>
#include <qcustomplot.h>
#include <QtMath>
#include "dialog_showloadprofile.h"
#include <dialog_editloadprofile.h>
#include <QAxObject>
#include <QDir>
#include <excelbase.h>
#include <dialog_reportoption.h>
#include <dialog_reportoption_2.h>

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

    QColor m_color[2][6]={
        {QColor(255,0,0),   QColor(0,255,0),     QColor(0,0,255),   QColor(255,128,0), QColor(255,255,0),  QColor(128,0,128)},
        {QColor(255,0,255), QColor(128,128,128), QColor(0,255,255), QColor(0,128,255), QColor(64,128,128), QColor(0,128,255)}};

    QColor m_color_2[2][10]={
        {QColor(255,128,0),
          QColor(235,118,0),
          QColor(215,108,0),
          QColor(195,98,0),
          QColor(175,88,0),
          QColor(155,78,0),
          QColor(135,68,0),
          QColor(115,58,0),
          QColor(95,48,0),
          QColor(75,38,0)},

        {QColor(0,128,255),
        QColor(0,118,235),
        QColor(0,108,215),
        QColor(0,98,195),
        QColor(0,88,175),
        QColor(0,78,155),
        QColor(0,68,135),
        QColor(0,58,115),
        QColor(0,48,95),
        QColor(0,38,75)}};

    struct pumpActualPerform{
        int    infor;
        int    pumps;
        double flow_total;
        double flow_fix;
        double flow_vfd;
        double head;
        double Hz_fix;
        double Hz_vfd;
        double power_axis_total;
        double power_axis_fix;
        double power_axis_vfd;
        double power_input_total;
        double power_input_fix;
        double power_input_vfd;
        double effi_pump_fix;
        double effi_pump_vfd;
        double effi_motor_fix;
        double effi_motor_vfd;
        double effi_total;
        double energy_specific;
    };

    enum controlMethod{
        none=0,
        fullSpeed=1,
        oneVfdC1=2,
        oneVfdC2=3,
        allVfdHzC1=4,
        allVfdHzC2=5,
        allVfdEffi=6};

    struct operationMode{
        controlMethod controlMethod;
        int iSpare;
        double stagingHz;
        double destagingHz;
        bool plotPowerCurve;
        bool plotEfficiencyCurve;
        bool plotVfdCurves;
        bool bSpare;
        QString modeName;
    };

    struct pointsOnCurve{
        int    pumps[101];
        int    spare1;
        double flow_total[101];
        double flow_fix[101];
        double flow_vfd[101];
        double head[101];
        double Hz_fix[101];
        double Hz_vfd[101];
        double power_axis_total[101];
        double power_axis_fix[101];
        double power_axis_vfd[101];
        double power_input_total[101];
        double power_input_fix[101];
        double power_input_vfd[101];
        double effi_pump_fix[101];
        double effi_pump_vfd[101];
        double effi_motor_fix[101];
        double effi_motor_vfd[101];
        double effi_total[101];
        double energy_specific[101];
    };

    struct LowestEsPoint{
        int     pumps;
        int     spare;
        double  flow;
        double  head;
        double  Es;
        double  Hz;
        double  input_power_total;
    };

    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void on_SetDbPara_clicked();
    void on_Load_clicked();
    void on_SaveAs_clicked();
    void on_ReportOption_clicked();

    void on_GenerateReport_clicked();
    void on_GenerateReport2_clicked();

    void on_family_changed();
    void on_model_changed();
    void on_speed_changed();
    void on_motor_changed();
    void on_method_1_changed();
    void on_method_2_changed();
    void on_loadprofile_app_changed();

    void on_connect_clicked();
    void on_multi_pump_clicked();
    void on_single_pump_clicked();
    void on_transport_clicked();

    void on_show_loadprofile_clicked();
    void on_edit_loadprofile_clicked();
    void on_new_loadprofile_clicked();
    void on_delete_loadprofile_clicked();

    bool get_pump_selection();
    bool get_system_profile();
    bool get_operation_mode();

    bool calcuSynchroSpeedPerform(bool isVfd, int pumps,double flow, double head, pumpActualPerform *perform);
    bool calcuOneVfdPerform(int pumps,double flow, double head, pumpActualPerform *perform);
    bool generatePerformList(pumpActualPerform (*perform)[10][101], operationMode operationMode);

    bool generatePointsOnCurve_fullSpeed(pointsOnCurve *pointsOnCurve, operationMode &operationMode);
    bool generatePointsOnCurve_oneVfdC1(pointsOnCurve *pointsOnCurve, operationMode &operationMode);
    bool generatePointsOnCurve_oneVfdC2(pointsOnCurve *pointsOnCurve, operationMode &operationMode);
    bool generatePointsOnCurve_allVfdHzC1(pointsOnCurve *pointsOnCurve, operationMode &operationMode);
    bool generatePointsOnCurve_allVfdHzC2(pointsOnCurve *pointsOnCurve, operationMode &operationMode);
    bool generatePointsOnCurve_allVfdEffi(pointsOnCurve *pointsOnCurve, operationMode &operationMode);
    bool generatePointsOnCurve_specificEnergy(pointsOnCurve (*pointsOnCurve)[10], operationMode &operationMode);
    bool generatePointsOnCurve_bestEs(pointsOnCurve *pointsOnCurve, operationMode &operationMode);
    bool generatePointsOnCurve_fixedSpeed(pointsOnCurve *pointsOnCurve, double Hz);

    void fillReport();
    void fillReport2();

    Ui::MainWindow          *ui;
    Dialog_GetDbPara        *m_dialog_getdbpara;
    Pump_db                 m_pumpDb;
    Pump                    m_pump;
    PumpSys                 m_pumpSys;
    Pump_db::pumpSelection  m_pumpSelection;
    Pump_db::motorSelection m_motorSelection;
    PumpSys::PumpSysPara    m_sysPara;
    operationMode           m_operationMode[2];

    QStandardItemModel     *m_loadprofile_data;
    Dialog_ShowLoadProfile *m_showLoadprofileDialog;
    Dialog_EditLoadProfile *m_editLoadprofileDialog;

    QScopedPointer<ExcelBase>  m_xls;
    QList< QList<QVariant> >   m_datas;

    Dialog_ReportOption::ReportOption     m_reportOption ={"","","",1,365};
    Dialog_ReportOption_2::ReportOption     m_reportOption_2;

    LowestEsPoint       m_lowestEsCurve[101];
    LowestEsPoint       m_lowestEsAllCurve;


protected:



};

#endif // MAINWINDOW_H
