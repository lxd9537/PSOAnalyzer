#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QModbusRtuSerialSlave>
#include <QModbusTcpServer>
#include <QUrl>
#include <qxtcsvmodel.h>
#include <QFileDialog>
#include <pumpunit.h>
#include <QPainter>
#include <QPixmap>
#include <modifydialog.h>
#include <global.h>
#include <QTimer>
#include <QDoubleSpinBox>
#include <QTime>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_settingsDialog = new SettingsDialog(this);
    m_modifyDialog = new ModifyDialog(this);
    m_operateDialog = new PumpOperateDialog(this);
    m_pumpCurveDialog = new PumpCurveDialog(this);

    m_timer_100ms = new QTimer(this);
    m_timer_500ms = new QTimer(this);
    m_timer_1000ms = new QTimer(this);

    for(int i=0; i<6; i++)
        m_modbusDevice[i]=nullptr;

    Initiation();

}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_settingsDialog;
    delete m_modifyDialog;
    delete m_operateDialog;
    delete m_timer_100ms;
    delete m_timer_500ms;
    delete m_timer_1000ms;

    for(int i=0; i<6; i++)
    {
        if (m_modbusDevice[i])
            m_modbusDevice[i]->disconnectDevice();
        if(m_modbusDevice[i]!=nullptr)
            delete m_modbusDevice[i];
    }
}

void MainWindow::Initiation()
{

    //menu
    connect(ui->actionImportPumpUnitConfiguration,&QAction::triggered, this, &MainWindow::on_actionImportPumpUnitConfiguration);
    connect(ui->actionConfig,&QAction::triggered, this, &MainWindow::on_actionConfigCommunications);
    connect(ui->actionModifyParameters,&QAction::triggered, this, &MainWindow::on_actionModifySystemProfile);
    connect(ui->actionIndividual_Curves,&QAction::triggered,this,&MainWindow::on_actionIndividual_Curves);
    connect(ui->actionCombined_Curves,&QAction::triggered,this,&MainWindow::on_actionCombined_Curves);

    //connect/disconnect
    connect(ui->connectButton,&QPushButton::clicked,this,&MainWindow::on_connectButton_clicked);
    connect(ui->disconnectButton,&QPushButton::clicked,this,&MainWindow::on_disconnectButton_clicked);

    //simulator
    connect(ui->startButton,&QPushButton::clicked,this,&MainWindow::on_startButton_clicked);
    connect(ui->pauseButton,&QPushButton::clicked,this,&MainWindow::on_pauseButton_clicked);
    connect(ui->stopButton,&QPushButton::clicked,this,&MainWindow::on_stopButton_clicked);
    connect(ui->snapshotButton,&QPushButton::clicked,this,&MainWindow::on_snapshotButton_clicked);

    //pump operate
    connect(ui->label_pump_1,&ClickLabel::clicked,this,&MainWindow::on_pumpButton_clicked);
    connect(ui->label_pump_2,&ClickLabel::clicked,this,&MainWindow::on_pumpButton_clicked);
    connect(ui->label_pump_3,&ClickLabel::clicked,this,&MainWindow::on_pumpButton_clicked);
    connect(ui->label_pump_4,&ClickLabel::clicked,this,&MainWindow::on_pumpButton_clicked);
    connect(ui->label_pump_5,&ClickLabel::clicked,this,&MainWindow::on_pumpButton_clicked);
    connect(ui->label_pump_6,&ClickLabel::clicked,this,&MainWindow::on_pumpButton_clicked);
    connect(&m_pump_unit,&PumpUnit::AnyPumpStatusChanged,m_operateDialog,&PumpOperateDialog::on_PumpStatusChanged);

    //timer
    connect(m_timer_100ms,&QTimer::timeout,this,&MainWindow::on_cycle_100ms);
    connect(m_timer_500ms,&QTimer::timeout,this,&MainWindow::on_cycle_500ms);
    connect(m_timer_1000ms,&QTimer::timeout,this,&MainWindow::on_cycle_1000ms);

    //connect check box
    connect(ui->checkBox_LSL,&QCheckBox::stateChanged,this,&MainWindow::on_checkBox_LSL_stateChange);
    connect(ui->checkBox_PSH,&QCheckBox::stateChanged,this,&MainWindow::on_checkBox_PSH_stateChange);

    //connect spin box
    connect(ui->Spin_tap_open_1,QOverload<double>::of(&QDoubleSpinBox::valueChanged),this,&MainWindow::on_SpinBox_tap1_valueChanged);
    connect(ui->Spin_tap_open_2,QOverload<double>::of(&QDoubleSpinBox::valueChanged),this,&MainWindow::on_SpinBox_tap2_valueChanged);
    connect(ui->Spin_tap_open_3,QOverload<double>::of(&QDoubleSpinBox::valueChanged),this,&MainWindow::on_SpinBox_tap3_valueChanged);

    //make pointer
    m_label_pump[0]=ui->label_pump_1;
    m_label_pump[1]=ui->label_pump_2;
    m_label_pump[2]=ui->label_pump_3;
    m_label_pump[3]=ui->label_pump_4;
    m_label_pump[4]=ui->label_pump_5;
    m_label_pump[5]=ui->label_pump_6;

    m_widget_pump[0]=ui->widget_P_1;
    m_widget_pump[1]=ui->widget_P_2;
    m_widget_pump[2]=ui->widget_P_3;
    m_widget_pump[3]=ui->widget_P_4;
    m_widget_pump[4]=ui->widget_P_5;
    m_widget_pump[5]=ui->widget_P_6;

    m_Edit_Hz[0]=ui->Edit_Hz_P_1;
    m_Edit_Hz[1]=ui->Edit_Hz_P_2;
    m_Edit_Hz[2]=ui->Edit_Hz_P_3;
    m_Edit_Hz[3]=ui->Edit_Hz_P_4;
    m_Edit_Hz[4]=ui->Edit_Hz_P_5;
    m_Edit_Hz[5]=ui->Edit_Hz_P_6;

    m_Edit_power[0]=ui->Edit_kW_P_1;
    m_Edit_power[1]=ui->Edit_kW_P_2;
    m_Edit_power[2]=ui->Edit_kW_P_3;
    m_Edit_power[3]=ui->Edit_kW_P_4;
    m_Edit_power[4]=ui->Edit_kW_P_5;
    m_Edit_power[5]=ui->Edit_kW_P_6;

    m_Edit_flow[0]=ui->Edit_Flow_P_1;
    m_Edit_flow[1]=ui->Edit_Flow_P_2;
    m_Edit_flow[2]=ui->Edit_Flow_P_3;
    m_Edit_flow[3]=ui->Edit_Flow_P_4;
    m_Edit_flow[4]=ui->Edit_Flow_P_5;
    m_Edit_flow[5]=ui->Edit_Flow_P_6;

    m_Edit_MOA[0]=ui->Edit_MOA_P_1;
    m_Edit_MOA[1]=ui->Edit_MOA_P_2;
    m_Edit_MOA[2]=ui->Edit_MOA_P_3;
    m_Edit_MOA[3]=ui->Edit_MOA_P_4;
    m_Edit_MOA[4]=ui->Edit_MOA_P_5;
    m_Edit_MOA[5]=ui->Edit_MOA_P_6;

    //set pump number
    ui->label_pump_1->SetPumpNumber(0);
    ui->label_pump_2->SetPumpNumber(1);
    ui->label_pump_3->SetPumpNumber(2);
    ui->label_pump_4->SetPumpNumber(3);
    ui->label_pump_5->SetPumpNumber(4);
    ui->label_pump_6->SetPumpNumber(5);

    //enable of connect button
    ui->connectButton->setEnabled(true);
    ui->disconnectButton->setEnabled(false);

    //simulator
    m_simulator_on =false;
    ui->startButton->setEnabled(true);
    ui->pauseButton->setEnabled(false);
    ui->stopButton->setEnabled(false);
    //ui->snapshotButton->setEnabled(false);

    //load image
    bool load_result = true;
    load_result &= m_img_pump_run.load(QDir::currentPath()+tr("\\pump_run.png"));
    load_result &= m_img_pump_fault.load(QDir::currentPath()+tr("\\pump_fault.png"));
    load_result &= m_img_pump_stop.load(QDir::currentPath()+tr("\\pump_stop.png"));
    load_result &= m_img_water_tap.load(QDir::currentPath()+tr("\\water_tap.png"));

    if(!load_result)
        QMessageBox::warning(nullptr, tr(""),tr("Load imgage failed!"));

    //start timer
    m_timer_100ms->start(100);
    m_timer_500ms->start(500);
    m_timer_1000ms->start(1000);
}
void MainWindow::on_actionImportPumpUnitConfiguration()
{
    QString filename;
    QFileDialog fileDialog(this);
    filename = fileDialog.getOpenFileName(this,"Import Pump Performance",QDir::currentPath(),"*.csv");

    QString ErrorMessage;
    if(!m_pump_unit.ImportPumpUnitConfiguration(filename,&ErrorMessage))
        QMessageBox::warning(this,"Import Pump Unit Configuration",ErrorMessage);
}

void MainWindow::on_actionConfigCommunications()
{
    m_settingsDialog->Initiation(m_pump_unit.ReadSysProfile().duty_pumps+m_pump_unit.ReadSysProfile().standby_pumps);
    m_settingsDialog->show();
}

void MainWindow::on_actionModifySystemProfile()
{
    m_modifyDialog->Initiation(&m_pump_unit);
    m_modifyDialog->show();
}

void MainWindow::on_actionIndividual_Curves()
{
    //imported check flag
    bool not_any_imported = false;
    QString str="";
    if(!m_pump_unit.ReadOnePumpImportedFlag(0))
    {
        str = "Pump performance data is not imported!";
        not_any_imported = true;
    }
    else if(!m_pump_unit.ReadOneValveImportedFlag(0))
    {
        str = " Valve characteristic data is not imported!";
        not_any_imported = true;
    }
    else if(!m_pump_unit.ReadImportedFlag())
    {
        str = " System profile data is not imported!";
        not_any_imported = true;
    }

    if(not_any_imported)
       {QMessageBox::warning(this,"Pump Curves",str);
        return;}
    //transfer data
    m_pumpCurveDialog->SetSystemProfile(m_pump_unit.ReadSysProfile());
    int pump_installed = m_pump_unit.ReadSysProfile().total_installed;

    for(int i=0; i < pump_installed; i++)
        m_pumpCurveDialog->SetPumpPerform(i,m_pump_unit.ReadOnePumpPerformData(i));

    //show curve
    m_pumpCurveDialog->setWindowFlags(m_pumpCurveDialog->windowFlags()
                                     | Qt::WindowMinimizeButtonHint
                                     | Qt::WindowMaximizeButtonHint);
    //show curve
    if(!m_pumpCurveDialog->isVisible())
    {
        m_pumpCurveDialog->Initiation(1);
        m_pumpCurveDialog->show();
    }
}

void  MainWindow::on_actionCombined_Curves()
{
    //imported check flag
    bool not_any_imported = false;
    QString str="";
    if(!m_pump_unit.ReadOnePumpImportedFlag(0))
    {
        str = "Pump performance data is not imported!";
        not_any_imported = true;
    }
    else if(!m_pump_unit.ReadOneValveImportedFlag(0))
    {
        str = " Valve characteristic data is not imported!";
        not_any_imported = true;
    }
    else if(!m_pump_unit.ReadImportedFlag())
    {
        str = " System profile data is not imported!";
        not_any_imported = true;
    }

    if(not_any_imported)
       {QMessageBox::warning(this,"Pump Curves",str);
        return;}
    //transfer data
    m_pumpCurveDialog->SetSystemProfile(m_pump_unit.ReadSysProfile());
    int pump_installed = m_pump_unit.ReadSysProfile().total_installed;

    for(int i=0; i < pump_installed; i++)
        m_pumpCurveDialog->SetPumpPerform(i,m_pump_unit.ReadOnePumpPerformData(i));

    //show curve
    m_pumpCurveDialog->setWindowFlags(m_pumpCurveDialog->windowFlags()
                                     | Qt::WindowMinimizeButtonHint
                                     | Qt::WindowMaximizeButtonHint);
    //show curve
    if(!m_pumpCurveDialog->isVisible())
    {
        m_pumpCurveDialog->Initiation(0);
        m_pumpCurveDialog->show();
    }
}

void MainWindow::on_connectButton_clicked()
{
    statusBar()->clearMessage();

    //disconnect the connected device
    for(int i=0; i<6; i++)
        if (m_modbusDevice[i])
        {
            m_modbusDevice[i]->disconnect();
            delete m_modbusDevice[i];
            m_modbusDevice[i] = nullptr;
        }

    //create a new device
    int total_pumps = m_pump_unit.ReadSysProfile().duty_pumps + m_pump_unit.ReadSysProfile().standby_pumps;
    if(total_pumps < 1) total_pumps = 1;

    if (m_settingsDialog->ReadCommSettings().connectType == SettingsDialog::Serial)
        for(int i=0; i<total_pumps; i++)
            m_modbusDevice[i] = new QModbusRtuSerialSlave(this);
    else
        for(int i=0; i<total_pumps; i++)
            m_modbusDevice[i] = new QModbusTcpServer(this);

    //create registers
    for(int i=0; i<total_pumps; i++)
        if (!m_modbusDevice[i])
        {
            if (m_settingsDialog->ReadCommSettings().connectType == SettingsDialog::Serial)
                statusBar()->showMessage(tr("Could not create Modbus slave."), 5000);
            else
                statusBar()->showMessage(tr("Could not create Modbus server."), 5000);
            return;
        }
        else
        {
            QModbusDataUnitMap reg;
            reg.insert(QModbusDataUnit::HoldingRegisters, { QModbusDataUnit::HoldingRegisters, 0, 20000 });
            m_modbusDevice[i]->setMap(reg);
        }

    //connection

    for(int i=0; i<total_pumps; i++)
    {
        bool intendToConnect = (m_modbusDevice[i]->state() == QModbusDevice::UnconnectedState);

        if (intendToConnect)
        {
            if (m_settingsDialog->ReadCommSettings().connectType == SettingsDialog::Serial)
            {
                m_modbusDevice[i]->setConnectionParameter(QModbusDevice::SerialPortNameParameter,
                      m_settingsDialog->ReadCommSettings().SerialPortName[i]);
                m_modbusDevice[i]->setConnectionParameter(QModbusDevice::SerialParityParameter,
                    m_settingsDialog->ReadCommSettings().parity);
                m_modbusDevice[i]->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,
                    m_settingsDialog->ReadCommSettings().baud);
                m_modbusDevice[i]->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,
                    m_settingsDialog->ReadCommSettings().dataBits);
                m_modbusDevice[i]->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,
                    m_settingsDialog->ReadCommSettings().stopBits);
            }
            else
            {
                m_modbusDevice[i]->setConnectionParameter(QModbusDevice::NetworkPortParameter, m_settingsDialog->ReadCommSettings().port + i);
                m_modbusDevice[i]->setConnectionParameter(QModbusDevice::NetworkAddressParameter, m_settingsDialog->ReadCommSettings().IpAddr);
            }

            m_modbusDevice[i]->setServerAddress(m_settingsDialog->ReadCommSettings().ModbusID + i);

            if (!m_modbusDevice[i]->connectDevice())
            {
                statusBar()->showMessage(tr("Connect failed: ") + m_modbusDevice[i]->errorString(), 5000);
                return;
            }
            else
            {
                ui->connectButton->setDisabled(true);
                ui->disconnectButton->setEnabled(true);
            }
        }
        else
        {
            m_modbusDevice[i]->disconnectDevice();
            ui->connectButton->setEnabled(true);
            ui->disconnectButton->setDisabled(true);
        }
    }
}
void MainWindow::on_disconnectButton_clicked()
{
    for(int i=0; i<6; i++)
        if (m_modbusDevice[i])
        {
            m_modbusDevice[i]->disconnect();
            delete m_modbusDevice[i];
            m_modbusDevice[i] = nullptr;
        }

    ui->connectButton->setEnabled(true);
    ui->disconnectButton->setDisabled(true);

}

void MainWindow::on_startButton_clicked()
{
    //imported check flag
    bool not_any_imported = false;
    QString str="";
    if(!m_pump_unit.ReadOnePumpImportedFlag(0))
    {
        str = "Pump performance data is not imported!";
        not_any_imported = true;
    }
    else if(!m_pump_unit.ReadOneValveImportedFlag(0))
    {
        str = " Valve characteristic data is not imported!";
        not_any_imported = true;
    }
    else if(!m_pump_unit.ReadImportedFlag())
    {
        str = " System profile data is not imported!";
        not_any_imported = true;
    }

    if(!not_any_imported)
    {
        m_simulator_on = true;
        ui->startButton->setEnabled(false);
        ui->pauseButton->setEnabled(true);
        ui->stopButton->setEnabled(true);
        //ui->snapshotButton->setEnabled(true);
    }
    else
    {
        QMessageBox::warning(this,"Start Simulator Failed",str);
    }
}
void MainWindow::on_pauseButton_clicked()
{
    m_simulator_on = false;
    ui->startButton->setEnabled(true);
    ui->pauseButton->setEnabled(false);
    ui->stopButton->setEnabled(true);
    //ui->snapshotButton->setEnabled(true);
}
void MainWindow::on_stopButton_clicked()
{
    m_simulator_on = false;
    ui->startButton->setEnabled(true);
    ui->pauseButton->setEnabled(false);
    ui->stopButton->setEnabled(false);
    //ui->snapshotButton->setEnabled(false);
    m_pump_unit.Reset();
}
void MainWindow::on_snapshotButton_clicked()
{
    QVector<QStringList> *csvData;
    csvData = new QVector<QStringList>;
    m_pump_unit.StatusSnapShot(csvData);

    QString filename;
    QFileDialog fileDialog(this);
    filename = fileDialog.getSaveFileName(this,"Save snap shot to file...",QDir::currentPath(),"*.csv");
    if(filename=="")
        return;

    QxtCsvModel *csvModel;
    csvModel = new QxtCsvModel(this);

    csvModel->toCSV(filename);
    csvModel->setSource(filename);
    csvModel->insertRows(0,csvData->size());
    csvModel->insertColumns(0,2);

    for(int i=0; i<csvData->size(); i++)
        for(int j=0; j<2; j++)
            csvModel->setText(i,j,csvData->at(i).at(j));

    csvModel->toCSV(filename);

    delete csvData;
    delete csvModel;
}

void MainWindow::on_pumpButton_clicked(int pump_number)
{
    m_operateDialog->Initiation(pump_number,&m_pump_unit);
    m_operateDialog->show();
}
void MainWindow::on_cycle_100ms()
{

}
void MainWindow::on_cycle_500ms()
{
/*    static bool complete_flag = true;
    if(complete_flag)
    {
        complete_flag = false;
        if(m_simulator_on)
            m_pump_unit.Run_cycle(500);
        complete_flag = true;
    }*/
}
void MainWindow::on_cycle_1000ms()
{
    static bool complete_flag = true;
    QString str;

    if(complete_flag)
    {
        complete_flag = false;
        //pump display
        int pump_installed = m_pump_unit.ReadSysProfile().duty_pumps + m_pump_unit.ReadSysProfile().standby_pumps;
        for(int i=0; i<pump_installed;i++)
        {
            str=QString::number(m_pump_unit.ReadOnePumpRoData(i).Hz,'f',1);
            str+=" Hz";
            m_Edit_Hz[i]->setText(str);
            str=QString::number(m_pump_unit.ReadOnePumpRoData(i).power,'f',1);
            str+=" kW";
            m_Edit_power[i]->setText(str);
            str=QString::number(m_pump_unit.ReadOnePumpRoData(i).flow,'f',1);
            str+=" tph";
            m_Edit_flow[i]->setText(str);

            if(m_pump_unit.ReadOnePumpRwData(i).Moa==Pump::Man)
            {
                m_Edit_MOA[i]->setText(tr("Manu"));
            }
            else if(m_pump_unit.ReadOnePumpRwData(i).Moa==Pump::Auto)
            {
                m_Edit_MOA[i]->setText(tr("Auto"));
            }
            else
            {
                m_Edit_MOA[i]->setText(tr("Off"));
            }
        }

        //booster display
        PumpUnit::RoData unit_ro = m_pump_unit.ReadRoData();
        PumpUnit::RwData unit_rw = m_pump_unit.ReadRwData();

        str = QString::number(unit_ro.pressure_at_inlet,'f',1);
        str += " bar";
        ui->Edit_press_inlet->setText(str);

        str = QString::number(unit_ro.pressure_at_outlet,'f',1);
        str += " bar";
        ui->Edit_press_outlet->setText(str);

        str = QString::number(unit_ro.pressure_at_use_point,'f',1);
        str += " bar";
        ui->Edit_press_use_point->setText(str);

        if(unit_rw.level_switch_low)
            ui->checkBox_LSL->setCheckState(Qt::Checked);
        else ui->checkBox_LSL->setCheckState(Qt::Unchecked);

        if(unit_rw.pressure_switch_high)
            ui->checkBox_PSH->setCheckState(Qt::Checked);
        else ui->checkBox_PSH->setCheckState(Qt::Unchecked);

        //valves display
        str = QString::number(m_pump_unit.ReadOneValveRoData(0).flow,'f',1);
        str += " tph";
        ui->Edit_tap_flow_1->setText(str);

        str = QString::number(m_pump_unit.ReadOneValveRoData(1).flow,'f',1);
        str += " tph";
        ui->Edit_tap_flow_2->setText(str);

        str = QString::number(m_pump_unit.ReadOneValveRoData(2).flow,'f',1);
        str += " tph";
        ui->Edit_tap_flow_3->setText(str);

        str = QString::number(m_pump_unit.ReadOneValveRoData(0).open_percent,'f',0);
        str += " %";
        ui->Edit_tap_open_1->setText(str);

        str = QString::number(m_pump_unit.ReadOneValveRoData(1).open_percent,'f',0);
        str += " %";
        ui->Edit_tap_open_2->setText(str);

        str = QString::number(m_pump_unit.ReadOneValveRoData(2).open_percent,'f',0);
        str += " %";
        ui->Edit_tap_open_3->setText(str);

        ui->Spin_tap_open_1->setValue(m_pump_unit.ReadOneValveRwData(0).open_percent_set);
        ui->Spin_tap_open_2->setValue(m_pump_unit.ReadOneValveRwData(1).open_percent_set);
        ui->Spin_tap_open_3->setValue(m_pump_unit.ReadOneValveRwData(2).open_percent_set);

        //imported check flag
        bool not_any_imported=false;
        str="";
        if(!m_pump_unit.ReadOnePumpImportedFlag(0))
        {
            str = "Pump performance data is not imported!";
            not_any_imported = true;
        }
        if(!m_pump_unit.ReadOneValveImportedFlag(0))
        {
            str += " Valve characteristic data is not imported!";
            not_any_imported = true;
        }
        if(!m_pump_unit.ReadImportedFlag())
        {
            str += " System profile data is not imported!";
            not_any_imported = true;
        }
        if(not_any_imported) ui->statusBar->showMessage(str);
        complete_flag = true;
    }
}
void MainWindow::on_checkBox_LSL_stateChange()
{
    PumpUnit::RwData unit_rw = m_pump_unit.ReadRwData();

    if(ui->checkBox_LSL->checkState()==Qt::Checked)
        unit_rw.level_switch_low = true;
    else  if(ui->checkBox_LSL->checkState()==Qt::Unchecked)
              unit_rw.level_switch_low = false;
    m_pump_unit.WriteRWData(unit_rw);

    if(m_simulator_on)
        m_pump_unit.Run_cycle(500);
}
void MainWindow::on_checkBox_PSH_stateChange()
{
    PumpUnit::RwData unit_rw = m_pump_unit.ReadRwData();

    if(ui->checkBox_PSH->checkState()==Qt::Checked)
        unit_rw.pressure_switch_high = true;
    else  if(ui->checkBox_PSH->checkState()==Qt::Unchecked)
              unit_rw.pressure_switch_high = false;
    m_pump_unit.WriteRWData(unit_rw);
}
void MainWindow::on_SpinBox_tap1_valueChanged(double value)
{
    ControlValve::RwData valve_rw = m_pump_unit.ReadOneValveRwData(0);
    valve_rw.open_percent_set = value;
    m_pump_unit.WriteOneValveRwData(0,valve_rw);

}
void MainWindow::on_SpinBox_tap2_valueChanged(double value)
{
    ControlValve::RwData valve_rw = m_pump_unit.ReadOneValveRwData(1);
    valve_rw.open_percent_set = value;
    m_pump_unit.WriteOneValveRwData(1,valve_rw);

}
void MainWindow::on_SpinBox_tap3_valueChanged(double value)
{
    ControlValve::RwData valve_rw = m_pump_unit.ReadOneValveRwData(2);
    valve_rw.open_percent_set = value;
    m_pump_unit.WriteOneValveRwData(2,valve_rw);

}
void MainWindow::mousePressEvent(QMouseEvent *e)
{
    QWidget *focus = QMainWindow::focusWidget();

    if(focus) focus->clearFocus();

    QMainWindow::mousePressEvent(e);
}
void MainWindow::paintEvent(QPaintEvent *event)
{

    int pump_installed = m_pump_unit.ReadSysProfile().duty_pumps + m_pump_unit.ReadSysProfile().standby_pumps;
    if(pump_installed<1) pump_installed = 1;

    QPainter painter(this);
    QPen pen;

    for(int i=0; i<pump_installed;i++)
    {
        m_label_pump[i]->setVisible(true);
        m_widget_pump[i]->setVisible(true);
        if(m_pump_unit.ReadOnePumpRoData(i).running)
                m_label_pump[i]->setPixmap(QPixmap::fromImage(m_img_pump_run));
        else if(m_pump_unit.ReadOnePumpRoData(i).fault)
                m_label_pump[i]->setPixmap(QPixmap::fromImage(m_img_pump_fault));
        else    m_label_pump[i]->setPixmap(QPixmap::fromImage(m_img_pump_stop));
    }

    for(int i=pump_installed; i<6;i++)
    {
        m_label_pump[i]->setVisible(false);
        m_widget_pump[i]->setVisible(false);
    }

    pen.setColor(QColor::fromRgb(180,180,180));
    pen.setWidth(2);
    painter.setPen(pen);
    QPoint pump_pos = m_label_pump[0]->pos();

    painter.drawLine(pump_pos + QPoint(-130,85),pump_pos + QPoint(0,85));
    painter.drawLine(pump_pos + QPoint(23,85),pump_pos + QPoint(229,85));
    painter.drawLine(pump_pos + QPoint(229,85),pump_pos + QPoint(229,-300));
    painter.drawLine(pump_pos + QPoint(229,85),pump_pos + QPoint(239,85));
    painter.drawLine(pump_pos + QPoint(229,-108),pump_pos + QPoint(239,-108));
    painter.drawLine(pump_pos + QPoint(229,-300),pump_pos + QPoint(239,-300));
    //压力罐
    painter.drawLine(pump_pos + QPoint(200,85),pump_pos + QPoint(200,75));
    painter.drawArc(QRect(pump_pos + QPoint(187,47), pump_pos + QPoint(213,73)),16*180,16*180);
    painter.drawLine(pump_pos + QPoint(187,60),pump_pos + QPoint(187,50));
    painter.drawLine(pump_pos + QPoint(214,60),pump_pos + QPoint(214,50));
    painter.drawArc(QRect(pump_pos + QPoint(187,37),pump_pos + QPoint(213,63)),0,16*180);

    ui->label_tap_1->setPixmap(QPixmap::fromImage(m_img_water_tap));
    ui->label_tap_2->setPixmap(QPixmap::fromImage(m_img_water_tap));
    ui->label_tap_3->setPixmap(QPixmap::fromImage(m_img_water_tap));

    for(int i=1;i<pump_installed;i++)
    {
       pump_pos = m_label_pump[i]->pos();
       painter.drawLine(pump_pos + QPoint(-30,85),pump_pos + QPoint(0,85));
       painter.drawLine(pump_pos + QPoint(23,85),pump_pos + QPoint(53,85));
       painter.drawLine(pump_pos + QPoint(-30,155),pump_pos + QPoint(-30,85));
       painter.drawLine(pump_pos + QPoint(53,155),pump_pos + QPoint(53,85));
    }

    QMainWindow::paintEvent(event);
}

