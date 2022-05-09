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
    connect(ui->pumpButton1,&MyPushButton::clicked,this,&MainWindow::on_pumpButton_clicked);
    connect(ui->pumpButton2,&MyPushButton::clicked,this,&MainWindow::on_pumpButton_clicked);
    connect(ui->pumpButton3,&MyPushButton::clicked,this,&MainWindow::on_pumpButton_clicked);
    connect(ui->pumpButton4,&MyPushButton::clicked,this,&MainWindow::on_pumpButton_clicked);
    connect(ui->pumpButton5,&MyPushButton::clicked,this,&MainWindow::on_pumpButton_clicked);
    connect(ui->pumpButton6,&MyPushButton::clicked,this,&MainWindow::on_pumpButton_clicked);
    connect(&m_pump_unit,&PumpUnit::AnyPumpStatusChanged,m_operateDialog,&PumpOperateDialog::on_PumpStatusChanged);

    //timer
    connect(m_timer_100ms,&QTimer::timeout,this,&MainWindow::on_cycle_100ms);
    connect(m_timer_500ms,&QTimer::timeout,this,&MainWindow::on_cycle_500ms);
    connect(m_timer_1000ms,&QTimer::timeout,this,&MainWindow::on_cycle_1000ms);

    //speed up
    connect(ui->speedupButton_1x,&QPushButton::clicked,this,&MainWindow::on_speedupButton_1x);
    connect(ui->speedupButton_10x,&QPushButton::clicked,this,&MainWindow::on_speedupButton_10x);
    connect(ui->speedupButton_100x,&QPushButton::clicked,this,&MainWindow::on_speedupButton_100x);

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

    m_Edit_head[0]=ui->Edit_Head_P_1;
    m_Edit_head[1]=ui->Edit_Head_P_2;
    m_Edit_head[2]=ui->Edit_Head_P_3;
    m_Edit_head[3]=ui->Edit_Head_P_4;
    m_Edit_head[4]=ui->Edit_Head_P_5;
    m_Edit_head[5]=ui->Edit_Head_P_6;

    m_Pump_Operate[0]=ui->pumpButton1;
    m_Pump_Operate[1]=ui->pumpButton2;
    m_Pump_Operate[2]=ui->pumpButton3;
    m_Pump_Operate[3]=ui->pumpButton4;
    m_Pump_Operate[4]=ui->pumpButton5;
    m_Pump_Operate[5]=ui->pumpButton6;

    //set pump number
    ui->pumpButton1->SetPumpNumber(0);
    ui->pumpButton2->SetPumpNumber(1);
    ui->pumpButton3->SetPumpNumber(2);
    ui->pumpButton4->SetPumpNumber(3);
    ui->pumpButton5->SetPumpNumber(4);
    ui->pumpButton6->SetPumpNumber(5);

    //enable of connect button
    ui->connectButton->setEnabled(true);
    ui->disconnectButton->setEnabled(false);

    //simulator
    m_simulator_on =false;
    ui->startButton->setEnabled(true);
    ui->pauseButton->setEnabled(false);
    ui->stopButton->setEnabled(false);
    //ui->snapshotButton->setEnabled(false);

    //simulating time
    ui->speedupButton_1x->setEnabled(false);
    ui->speedupButton_10x->setEnabled(true);
    ui->speedupButton_100x->setEnabled(true);

    //load image
    bool load_result = true;
    load_result &= m_img_pump_run.load(QDir::currentPath()+tr("\\flygt_green.png"));
    load_result &= m_img_pump_fault.load(QDir::currentPath()+tr("\\flygt_red.png"));
    load_result &= m_img_pump_stop.load(QDir::currentPath()+tr("\\flygt_grey.png"));
    load_result &= m_img_level.load(QDir::currentPath()+tr("\\water_level.png"));

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
                m_modbusDevice[i]->setConnectionParameter(QModbusDevice::NetworkAddressParameter,"127.0.0.1");//m_settingsDialog->ReadCommSettings().IpAddr);
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
                ui->connectButton->clearFocus();
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
    ui->disconnectButton->clearFocus();

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
    else if(!m_pump_unit.ReadImportedFlag())
    {
        str = " System profile data is not imported!";
        not_any_imported = true;
    }

    if(!not_any_imported)
    {
        m_simulator_on = true;
        ui->startButton->setEnabled(false);
        ui->startButton->clearFocus();
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
    ui->pauseButton->clearFocus();
    ui->stopButton->setEnabled(true);
    //ui->snapshotButton->setEnabled(true);
}
void MainWindow::on_stopButton_clicked()
{
    m_simulator_on = false;
    m_simulating_total_sec = 0;
    ui->startButton->setEnabled(true);
    ui->pauseButton->setEnabled(false);
    ui->stopButton->setEnabled(false);
    ui->stopButton->clearFocus();
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
    static bool complete_flag = true;

    if(complete_flag)
    {
        complete_flag = false;
        if(m_simulating_hour > 23)
            m_simulating_hour = 23;
        if(m_simulator_on)
            m_pump_unit.Run_cycle(500,m_speed_up,m_simulating_hour);
        complete_flag = true;
    }
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

            str=QString::number(m_pump_unit.ReadOnePumpRoData(i).head,'f',1);
            str+=" m";
            m_Edit_head[i]->setText(str);

            if(m_pump_unit.ReadOnePumpRwData(i).Moa==Pump::Man)
            {
                m_Pump_Operate[i]->setStyleSheet("font: 8pt \"Arial\";color: rgb(200, 30, 30)");
                m_Pump_Operate[i]->setText(tr("Manu"));
            }
            else if(m_pump_unit.ReadOnePumpRwData(i).Moa==Pump::Auto)
            {
                m_Pump_Operate[i]->setStyleSheet("font: 8pt \"Arial\";color: rgb(30, 200, 30)");
                m_Pump_Operate[i]->setText(tr("Auto"));
            }
            else
            {
                m_Pump_Operate[i]->setStyleSheet("font: 8pt \"Arial\";color: rgb(100, 100, 100)");
                m_Pump_Operate[i]->setText(tr("Off"));
            }
        }

        //top display
        PumpUnit::RoData unit_ro = m_pump_unit.ReadRoData();

        str = QString::number(unit_ro.level,'f',1);
        str += " m";
        ui->Edit_level->setText(str);

        str = QString::number(unit_ro.in_flow,'f',1);
        str += " tph";
        ui->Edit_in_flow->setText(str);

        str = QString::number(unit_ro.flow_total,'f',1);
        str += " tph";
        ui->Edit_total_flow->setText(str);

        //imported check flag
        bool not_any_imported=false;
        str="";
        if(!m_pump_unit.ReadOnePumpImportedFlag(0))
        {
            str = "Pump performance data is not imported!";
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

    //ModbusRegUpdate
    ModbusRegUpdate();

    //system clock
    if(m_simulator_on)
        m_simulating_total_sec += m_speed_up;

    m_simulating_year = m_simulating_total_sec / 31536000;
    m_simulating_month = (m_simulating_total_sec % 31536000) / 2592000;
    m_simulating_day = (m_simulating_total_sec % 2592000)/ 86400;
    m_simulating_hour = (m_simulating_total_sec % 86400)/3600;
    m_simulating_min = (m_simulating_total_sec % 3600)/60;
    m_simulating_sec = m_simulating_total_sec % 60;

    QString str2(":");
    QString simulating_time = QString::number(qMin(99,m_simulating_year)) + str2
                              + QString::number(qMin(99,m_simulating_month)) + str2
                              + QString::number(qMin(99,m_simulating_day)) + str2
                              + QString::number(qMin(99,m_simulating_hour)) + str2
                              + QString::number(qMin(99,m_simulating_min)) + str2
                              + QString::number(qMin(99,m_simulating_sec));

    ui->Edit_simulating_time->setText(simulating_time);
}

void MainWindow::on_speedupButton_1x()
{
    m_speed_up = 1;
    ui->speedupButton_1x->setEnabled(false);
    ui->speedupButton_10x->setEnabled(true);
    ui->speedupButton_100x->setEnabled(true);
    ui->speedupButton_1x->clearFocus();
    ui->speedupButton_10x->clearFocus();
    ui->speedupButton_100x->clearFocus();
    ui->groupBox_3->clearFocus();

}
void MainWindow::on_speedupButton_10x()
{
    m_speed_up = 10;
    ui->speedupButton_1x->setEnabled(true);
    ui->speedupButton_10x->setEnabled(false);
    ui->speedupButton_100x->setEnabled(true);
    ui->speedupButton_1x->clearFocus();
    ui->speedupButton_10x->clearFocus();
    ui->speedupButton_100x->clearFocus();
    ui->groupBox_3->clearFocus();

}
void MainWindow::on_speedupButton_100x()
{
    m_speed_up = 100;
    ui->speedupButton_1x->setEnabled(true);
    ui->speedupButton_10x->setEnabled(true);
    ui->speedupButton_100x->setEnabled(false);
    ui->speedupButton_1x->clearFocus();
    ui->speedupButton_10x->clearFocus();
    ui->speedupButton_100x->clearFocus();
    ui->groupBox_3->clearFocus();

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
    QBrush brush;
    QPoint pump_pos = m_label_pump[0]->pos();

    //tank background
    pen.setStyle(Qt::NoPen);
    brush.setColor(QColor::fromRgb(255,255,255,100));
    brush.setStyle(Qt::SolidPattern);
    painter.setBrush(brush);
    painter.setPen(pen);
    painter.drawRect(40,275,pump_installed*30+(pump_installed-1)*25+45,120);

    ui->line_3->setGeometry(ui->line_3->geometry().x(),
                            ui->line_3->geometry().y(),
                            pump_installed*30+(pump_installed-1)*25+47,
                            ui->line_3->geometry().height());

    ui->line_2->setGeometry(pump_installed*30+(pump_installed-1)*25+75,
                            ui->line_2->geometry().y(),
                            ui->line_2->geometry().width(),
                            ui->line_2->geometry().height());
    pen.setStyle(Qt::SolidLine);
    pen.setColor(Qt::black);
    pen.setWidth(2);
    painter.setPen(pen);
    brush.setColor(QColor::fromRgb(0,0,0,100));
    brush.setStyle(Qt::SolidPattern);
    painter.setBrush(brush);
    int line_x,line_y;
    QPoint triangle[3];

    line_x = ui->line_4->geometry().x() + ui->line_4->geometry().width();
    line_y = ui->line_4->geometry().y() + centralWidget()->geometry().y() + ui->line_4->geometry().height()/2;
    triangle[0] = QPoint(line_x,line_y);
    triangle[1] = QPoint(line_x-4,line_y-4);
    triangle[2] = QPoint(line_x-4,line_y+4);
    painter.drawPolygon(triangle,3);

    line_x = ui->line_6->geometry().x() + ui->line_6->geometry().width()/2;
    line_y = ui->line_6->geometry().y() + centralWidget()->geometry().y() + ui->line_6->geometry().height();
    triangle[0] = QPoint(line_x,line_y);
    triangle[1] = QPoint(line_x-4,line_y-4);
    triangle[2] = QPoint(line_x+4,line_y-4);
    painter.drawPolygon(triangle,3);

    int y_plus = centralWidget()->y();

    for(int i=0; i<pump_installed;i++)
    {
        m_label_pump[i]->setVisible(true);
        m_widget_pump[i]->setVisible(true);
        if(m_pump_unit.ReadOnePumpRoData(i).running)
                m_label_pump[i]->setPixmap(QPixmap::fromImage(m_img_pump_run));
        else if(m_pump_unit.ReadOnePumpRoData(i).fault)
                m_label_pump[i]->setPixmap(QPixmap::fromImage(m_img_pump_fault));
        else    m_label_pump[i]->setPixmap(QPixmap::fromImage(m_img_pump_stop));

        painter.drawLine(91+55*i,353+y_plus,91+55*i+10,353+y_plus);
        painter.drawLine(91+55*i+10,353+y_plus,91+55*i+10,232+y_plus);

    }

    for(int i=pump_installed; i<6;i++)
    {
        m_label_pump[i]->setVisible(false);
        m_widget_pump[i]->setVisible(false);
    }


    //water level
    double level_percent = 0;

    if(m_pump_unit.ReadSysProfile().well_height>0)
        level_percent = m_pump_unit.ReadRoData().level / m_pump_unit.ReadSysProfile().well_height * 100;

    ui->level_label->setPixmap(QPixmap::fromImage(m_img_level));
    ui->level_label->setGeometry(pump_pos.x() - 20,
                                 pump_pos.y() - 69 + 120 - static_cast<int>(level_percent/100*120),
                                 pump_installed*30+(pump_installed-1)*25+45,
                                 static_cast<int>(level_percent/100*120));

    ui->Edit_level->setGeometry(ui->level_label->geometry().x()+ui->level_label->geometry().width()/2-25,
                                ui->Edit_level->geometry().y(),
                                ui->Edit_level->geometry().width(),
                                ui->Edit_level->geometry().height());


    QMainWindow::paintEvent(event);
}

void MainWindow::ModbusRegUpdate()
{
    for(int i = 0; i < m_pump_unit.ReadSysProfile().total_installed;i++)
    {
        if(!m_modbusDevice[i])
            continue;
        if(m_pump_unit.ReadSysProfile().vfd_model==PumpUnit::Hydrovar)
        {
            //command from PLC
            quint16 wRead[2];
            Pump::RwData rw_data = m_pump_unit.ReadOnePumpRwData(i);

            m_modbusDevice[i]->data(QModbusDataUnit::HoldingRegisters,49,&wRead[0]);
            m_modbusDevice[i]->data(QModbusDataUnit::HoldingRegisters,234,&wRead[1]);

            if(wRead[0]==1)
                rw_data.run_request = true;
            else
                rw_data.run_request = false;

            rw_data.auto_Hz = static_cast<double>(wRead[1])/10.0;
            m_pump_unit.WriteOnePumpRwData(i,rw_data);

            //feedback to PLC
            quint16 wWrite[13];
            Pump::RoData ro_data = m_pump_unit.ReadOnePumpRoData(i);
            wWrite[0] = static_cast<quint16>(ro_data.Hz*10);
            wWrite[1] = (static_cast<quint16>(ro_data.running)<<i);

            m_modbusDevice[i]->setData(QModbusDataUnit::HoldingRegisters,51,wWrite[0]);
            m_modbusDevice[i]->setData(QModbusDataUnit::HoldingRegisters,89,wWrite[1]);
        }
    }
}
