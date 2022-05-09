#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QSettings>
#include <Curves/HistCurve/chistcurvewidget.h>
#include "Curves/PerformCurve/cperformcurvewidget.h"
#include <Database/MySql/cmysqlthread.h>
#include <Database/InfluxDb/cinfluxdbquerythread.h>
#include <PumpSystem/PumpCalculator/cpumpcalculator.h>
#include "PumpSystem/SystemProfile/csystemprofileinputdialog.h"
#include "PumpSystem/SystemProfile/csystemprofileinputdialog.h"
#include <PumpSystem/PumpData/cpumpdatainputdialog.h>
#include <PumpSystem/PumpData/cpumpdatamanagedialog.h>
#include <PumpSystem/PumpSystemCalculator/cpumpsystemcalculator.h>

#define CONFIG_FILE_NAME ("/config.ini")
#define INFLUXDB_QUERY_ID           (5)
#define MYSQL_CLIENT_ID             (3)

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void influxdbQureyRequired();
    void curveUpdate();


private slots:
    void onTimeChartToQueryInfluxDb(int para_code);
    void onInfluxDbQueryFinished();
    void onEventReported(int id);

     void on_pumpDataManage_triggered();

    void on_openProject_triggered();

    void on_newProject_triggered();

    void on_histCurve_triggered();

    void on_singlePumpCalculator_triggered();

    void on_performCurve_triggered();

    void on_pumpGroupCalculator_triggered();

    void on_viewProject_triggered();

private:
    Ui::MainWindow *ui;
    CHistCurveWidget *m_time_widget = nullptr;
    CPerfomCurveWidget *m_perform_curve = nullptr;

    /* time chart */
    QList<CHistCurveWidget::CurveConfig> m_curve_list;
    QList<CHistCurveWidget::CurveData> m_curve_data;
    CHistCurveWidget::CurveConfig m_current_curve;

    /* InfluxDb query */
    CInfluxDbQueryThread *m_influxdb_query_thread;
    CInfluxDbQueryThread::QueryUnit m_influx_query_unit;
    QSemaphore m_semaphore;

    /* mysql */
    CMySqlThread *m_mysql_thread;

    /* project file */
    QFile *m_pro_file;
    CSystemProfileInputDialog::SystemProfile m_system_profile;
    bool m_system_profile_loaded;

    bool loadConfigFromIni(CInfluxDbQueryThread::Config &influxdb_query_config,
                           CMySqlThread::Config &mysql_config);
    void TimeChartInit();
};
#endif // MAINWINDOW_H
