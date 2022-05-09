#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModbusServer>
#include <QLineEdit>
#include <settingsdialog.h>
#include <pumpunit.h>
#include <QLabel>
#include <modifydialog.h>
#include <pumpoperatedialog.h>
#include <clicklabel.h>
#include <QTimer>
#include <QTime>
#include <readonlylineedit.h>
#include <pumpcurvedialog.h>
#include <mypushbutton.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow      *ui;
    QModbusServer       *m_modbusDevice[6];
    SettingsDialog      *m_settingsDialog;
    ModifyDialog        *m_modifyDialog;
    PumpOperateDialog   *m_operateDialog;
    PumpCurveDialog     *m_pumpCurveDialog;

    QTimer              *m_timer_100ms;
    QTimer              *m_timer_500ms;
    QTimer              *m_timer_1000ms;

    QImage              m_img_pump_run,m_img_pump_fault,m_img_pump_stop,m_img_level;
    QLabel              *m_label_pump[6];
    QWidget             *m_widget_pump[6];
    ReadOnlyLineEdit    *m_Edit_Hz[6];
    ReadOnlyLineEdit    *m_Edit_power[6];
    ReadOnlyLineEdit    *m_Edit_flow[6];
    ReadOnlyLineEdit    *m_Edit_head[6];
    MyPushButton        *m_Pump_Operate[6];
    PumpUnit            m_pump_unit;
    bool                m_simulator_on;
    long                m_simulating_total_sec = 0;
    int                 m_simulating_sec = 0;
    int                 m_simulating_min;
    int                 m_simulating_hour;
    int                 m_simulating_day;
    int                 m_simulating_month;
    int                 m_simulating_year;
    int                 m_speed_up = 1; //1/10/100

    void Initiation();
    void ModbusRegUpdate();

private Q_SLOT:
    void on_actionImportPumpUnitConfiguration();
    void on_actionConfigCommunications();
    void on_actionModifySystemProfile();
    void on_actionIndividual_Curves();
    void on_actionCombined_Curves();
    void on_connectButton_clicked();
    void on_disconnectButton_clicked();
    void on_startButton_clicked();
    void on_pauseButton_clicked();
    void on_stopButton_clicked();
    void on_snapshotButton_clicked();
    void on_pumpButton_clicked(int pump_number);
    void on_cycle_100ms();
    void on_cycle_500ms();
    void on_cycle_1000ms();
    void on_speedupButton_1x();
    void on_speedupButton_10x();
    void on_speedupButton_100x();

protected:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void paintEvent(QPaintEvent *event);
};

#endif // MAINWINDOW_H
