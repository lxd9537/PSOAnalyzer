#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_dialog_getdbpara = new Dialog_GetDbPara();

    connect(ui->actionSet_database,&QAction::triggered,this,&MainWindow::on_SetDbPara_clicked);
    connect(ui->pushButton_connect,&QPushButton::clicked,this,&MainWindow::on_connect_clicked);
    connect(ui->comboBox_pumpFamily,QOverload<const QString &>::of(&QComboBox::currentIndexChanged),this,&MainWindow::on_family_changed);
    connect(ui->comboBox_pumpModel,QOverload<const QString &>::of(&QComboBox::currentIndexChanged),this,&MainWindow::on_model_changed);
    connect(ui->comboBox_pumpSpeed,QOverload<const QString &>::of(&QComboBox::currentIndexChanged),this,&MainWindow::on_speed_changed);
    connect(ui->comboBox_motor,QOverload<const QString &>::of(&QComboBox::currentIndexChanged),this,&MainWindow::on_motor_changed);
    connect(ui->pushButton_update,&QPushButton::clicked,this,&MainWindow::on_multi_pump_clicked);
    connect(ui->pushButton_single_pump,&QPushButton::clicked,this,&MainWindow::on_single_pump_clicked);

    connect(ui->actionSaveAs,&QAction::triggered,this,&MainWindow::on_SaveAs_clicked);
    connect(ui->actionLoad,&QAction::triggered,this,&MainWindow::on_Load_clicked);

    for(int i=0;i<6;i++)
        for(int j=0;j<100;j++)
            m_pumpActualPerform[i][j] = {i+1,0,0,0,0,0,0,0};
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_dialog_getdbpara;
}

void MainWindow::on_SetDbPara_clicked()
{

    m_dialog_getdbpara->Initialization(m_pumpDb.getDbPara(),m_pumpDb.getTableName());

    if(m_dialog_getdbpara->exec()==QDialog::Accepted)
    {
        m_pumpDb.setDbPara(m_dialog_getdbpara->GetDbPara());
        m_pumpDb.setTableName(m_dialog_getdbpara->GetDbTableName());
    }

}

void MainWindow::on_connect_clicked()
{
    if(m_pumpDb.connnect())
    {
        ui->pushButton_connect->setEnabled(false);

        QStringList family_list;
        m_pumpDb.getPumpFamilyList(&family_list);

        foreach(QString pump_family,family_list)
        {
            ui->comboBox_pumpFamily->addItem(pump_family);
        }

        QStringList motor_list;
        m_pumpDb.getMotorList(&motor_list);

        foreach(QString motor,motor_list)
        {
            ui->comboBox_motor->addItem(motor);
        }
     }
}

void MainWindow::on_family_changed()
{
    if(m_pumpDb.getConnectStatus())
    {
        QString family = ui->comboBox_pumpFamily->currentText();
        QStringList model_list;
        m_pumpDb.getPumpModelList(family,&model_list);

        QString pump_model;
        ui->comboBox_pumpModel->clear();
        foreach(pump_model,model_list)
             ui->comboBox_pumpModel->addItem(pump_model);
     }
}

void MainWindow::on_model_changed()
{
    if(m_pumpDb.getConnectStatus())
    {

        QString family = ui->comboBox_pumpFamily->currentText();
        QString model = ui->comboBox_pumpModel->currentText();

        QList<int> speed_list;
        m_pumpDb.getPumpSpeedList(family,model, &speed_list);

        ui->comboBox_pumpSpeed->clear();
        foreach(int pump_speed,speed_list)
        {
            ui->comboBox_pumpSpeed->addItem(QString::number(pump_speed));
        }

    }
}

void MainWindow::on_speed_changed()
{
    if(m_pumpDb.getConnectStatus())
    {
        QString family = ui->comboBox_pumpFamily->currentText();
        QString model = ui->comboBox_pumpModel->currentText();
        int speed = ui->comboBox_pumpSpeed->currentText().toInt();

        double bigImp,smallImp;
        m_pumpDb.getPumpImpRange(family,model,speed,&bigImp,&smallImp);
        ui->lineEdit_impRange->setText(QString::number(smallImp,'f',1)
                                       + " - " + QString::number(bigImp,'f',1));
    }
}

void MainWindow::on_motor_changed()
{
    if(m_pumpDb.getConnectStatus())
    {
        double power_rate = ui->comboBox_motor->currentText().toDouble();
        QList<int> poles_list;
        m_pumpDb.getMotorPolesList(power_rate, &poles_list);

        ui->comboBox_motor_poles->clear();
        foreach(int poles, poles_list)
        {
            ui->comboBox_motor_poles->addItem(QString::number(poles));
        }
    }
}

void MainWindow::on_multi_pump_clicked()
{

    if(!get_system_config_from_ui()) return;
    show_multi_pump_curve();

}

void MainWindow::on_single_pump_clicked()
{
    if(!get_pump_config_from_ui()) return;
    show_single_pump_curve();
}

void MainWindow::on_SaveAs_clicked()
{
    QString filename;
    QFileDialog fileDialog(this);
    filename = fileDialog.getSaveFileName(this,"Configuration save as...",QDir::currentPath(),"*.ini");
    if(filename=="")
        return;

    QSettings *configIniWrite = new QSettings(filename, QSettings::IniFormat);

    configIniWrite->setValue("/pump/family", ui->comboBox_pumpFamily->currentText());
    configIniWrite->setValue("pump/model", ui->comboBox_pumpModel->currentText());
    configIniWrite->setValue("pump/speed", ui->comboBox_pumpSpeed->currentText());
    configIniWrite->setValue("pump/impRange", ui->lineEdit_impRange->text());
    configIniWrite->setValue("pump/impTrimed", ui->lineEdit_impTrimed->text());
    configIniWrite->setValue("pump/motorPower", ui->comboBox_motor->currentText());
    configIniWrite->setValue("pump/motorPoles", ui->comboBox_motor_poles->currentText());

    configIniWrite->setValue("system/dutyPumps", ui->comboBox_duty_pumps->currentText());
    configIniWrite->setValue("system/totalFlow", ui->lineEdit_total_flow->text());
    configIniWrite->setValue("system/headAtZeroFlow", ui->lineEdit_head_zero_flow->text());
    configIniWrite->setValue("system/headAtTotalFlow", ui->lineEdit_head_total_flow->text());
    configIniWrite->setValue("system/controlCurveShaping", ui->lineEdit_control_curve_shaping->text());

    delete configIniWrite;
    QMessageBox::information(nullptr,"Save configuration","Save configuration succeed!");
}
void MainWindow::on_Load_clicked()
{
    QString filename;
    QFileDialog fileDialog(this);
    filename = fileDialog.getOpenFileName(this,"Load configuration...",QDir::currentPath(),"*.ini");
    if(filename=="")
        return;

    if(!m_pumpDb.getConnectStatus())
        on_connect_clicked();

    QSettings *configIniRead = new QSettings(filename, QSettings::IniFormat);

    ui->comboBox_pumpFamily->setCurrentText(configIniRead->value("/pump/family").toString());
    ui->comboBox_pumpModel->setCurrentText(configIniRead->value("pump/model").toString());
    ui->comboBox_pumpSpeed->setCurrentText(configIniRead->value("pump/speed").toString());
    ui->lineEdit_impRange->setText(configIniRead->value("pump/impRange").toString());
    ui->lineEdit_impTrimed->setText(configIniRead->value("pump/impTrimed").toString());
    ui->comboBox_motor->setCurrentText(configIniRead->value("pump/motorPower").toString());
    ui->comboBox_motor_poles->setCurrentText(configIniRead->value("pump/motorPoles").toString());

    ui->comboBox_duty_pumps->setCurrentText(configIniRead->value("system/dutyPumps").toString());
    ui->lineEdit_total_flow->setText(configIniRead->value("system/totalFlow").toString());
    ui->lineEdit_head_zero_flow->setText(configIniRead->value("system/headAtZeroFlow").toString());
    ui->lineEdit_head_total_flow->setText(configIniRead->value("system/headAtTotalFlow").toString());
    ui->lineEdit_control_curve_shaping->setText(configIniRead->value("system/controlCurveShaping").toString());

    delete configIniRead;



}
void MainWindow::get_pump_selection()
{
    m_pumpSelection.family = ui->comboBox_pumpFamily->currentText();
    m_pumpSelection.model = ui->comboBox_pumpModel->currentText();
    m_pumpSelection.speed = ui->comboBox_pumpSpeed->currentText().toInt();
    m_pumpSelection.impTrimed = ui->lineEdit_impTrimed->text().toDouble();
    m_pumpSelection.motorPower = ui->comboBox_motor->currentText();
    m_pumpSelection.motorPoles = ui->comboBox_motor_poles->currentText().toInt();
}

void MainWindow::get_motor_selection()
{
    m_motorSelection.power_rate = ui->comboBox_motor->currentText().toDouble();
    m_motorSelection.poles = ui->comboBox_motor_poles->currentText().toInt();
}

void MainWindow::get_system_design()
{
    m_sysPara.dutyPumps = ui->comboBox_duty_pumps->currentText().toInt();
    m_sysPara.designTotalFlow = ui->lineEdit_total_flow->text().toDouble();
    m_sysPara.headAtZeroFlow = ui->lineEdit_head_zero_flow->text().toDouble();
    m_sysPara.headAtTotalFlow = ui->lineEdit_head_total_flow->text().toDouble();
    m_sysPara.controlCurveShape = ui->lineEdit_control_curve_shaping->text().toDouble();
}

void MainWindow::show_multi_pump_curve()
{
    double max_x=0;
    double max_y=0;
    QCPGraph    *graph;

    //double max_y2 = 0;
    QVector<double> x(101), y(101);

    //clear graph
    ui->customPlot_2->clearGraphs();
    ui->customPlot_2->clearTracerTexts();

    // control curve
     max_y = m_pump.ReadPerformance().head_max;
     max_x = m_pump.ReadPerformance().flow_max * m_sysPara.dutyPumps;

     for (int i=0; i<101; i++)
     {
         x[i] = m_sysPara.designTotalFlow / 100 * i;
         y[i] = m_pumpSys.GetControlCurve(x[i]);

         if(y[i]>max_y) max_y=y[100];
         if(x[i]>max_x) max_x=x[100];
     }

     graph = ui->customPlot_2->addGraph();
     graph->setData(x, y);
     graph->setPen(QPen(m_color[0],2));
     graph->setName("control");

    //pump curve
    for(int j=0; j<m_sysPara.dutyPumps; j++)
    {
        for (int i=0; i<101; ++i)
        {
            double s_flow = m_pump.ReadPerformance().flow_max / 100 * i;
            x[i] = s_flow * (j+1);
            y[i] = m_pump.GetHeadFromFlow(50,s_flow);
        }
        graph = ui->customPlot_2->addGraph();
        graph->setData(x, y);
        graph->setPen(QPen(m_color[j+1],2));
        graph->setName(QString::number(j+1) + "-head");
    }

    //efficiency curve
    if(!get_best_effi()) return;

    if(ui->checkBox_EffiCurve->isChecked())
    {
        for(int j=0; j<m_sysPara.dutyPumps; j++)
        {
            for(int i=0; i<101; i++)
            {
                x[i] = m_sysPara.designTotalFlow / 100 * i;
                y[i] = m_pumpActualPerform[j][i].efficiency;
                if(y[i]<0.01) x[i]=0;
            }

            graph = ui->customPlot_2->addGraph(ui->customPlot_2->xAxis,ui->customPlot_2->yAxis2);
            graph->setData(x, y);
            graph->setPen(QPen(m_color[j+1],2));
            graph->setName(QString::number(j+1) + "-effi.");
        }
    }

    //best efficiency
    if(ui->checkBox_BES->isChecked())
    {
            for(int i=0; i<101; i++)
            {
                x[i] = m_sysPara.designTotalFlow / 100 * i;
                y[i] = m_best_effi[i];
                if(y[i]<0.01) x[i]=0;
            }

            graph = ui->customPlot_2->addGraph(ui->customPlot_2->xAxis,ui->customPlot_2->yAxis2);
            graph->setData(x, y);
            graph->setPen(QPen(m_color[m_sysPara.dutyPumps+1],2));
            graph->setName("best effi.");
    }

    //fix efficiency
    if(!get_fix_effi()) return;

    if(ui->checkBox_FF->isChecked())
    {
        for(int i=0; i<101; i++)
        {
            x[i] = m_sysPara.designTotalFlow / 100 * i;
            y[i] = m_fix_effi_1[i];
            if(y[i]<0.01) x[i]=0;
        }

        graph = ui->customPlot_2->addGraph(ui->customPlot_2->xAxis,ui->customPlot_2->yAxis2);
        graph->setData(x, y);
        graph->setPen(QPen(m_color[m_sysPara.dutyPumps+2],2));
        graph->setName("fix effi. 1");

        for(int i=0; i<101; i++)
        {
            x[i] = m_sysPara.designTotalFlow / 100 * i;
            y[i] = m_fix_effi_2[i];
            if(y[i]<0.01) x[i]=0;
        }

        graph = ui->customPlot_2->addGraph(ui->customPlot_2->xAxis,ui->customPlot_2->yAxis2);
        graph->setData(x, y);
        graph->setPen(QPen(m_color[m_sysPara.dutyPumps+3],2));
        graph->setName("fix effi. 2");
    }

    //vfd head curve
    if(ui->checkBox_VfCurve->isChecked())
    {
        QVector<double> x2(20), y2(20);
        QVector<double> load_points_x;
        QVector<double> load_points_y;
        QVector<QString> load_points_text;

        for(int p=5;p<101;p+=5)
        {
            for (int i=0; i<101; ++i)
            {
                double s_flow = m_pump.ReadPerformance().flow_max*m_best_Hz[p]/50/ 100 * i;
                x[i] = s_flow * m_best_pump_qty[p];
                y[i] = m_pump.GetHeadFromFlow(m_best_Hz[p],s_flow);
            }
            graph = ui->customPlot_2->addGraph();
            graph->setData(x, y);
            QPen pen;
            pen.setColor(m_color[ m_best_pump_qty[p]-1]);
            pen.setStyle(Qt::DashLine);
            pen.setWidth(1);
            graph->setPen(pen);
            graph->removeFromLegend();

            x2[p/5-1] = m_sysPara.designTotalFlow / 100 * p;
            y2[p/5-1] = m_pumpSys.GetControlCurve(x2[p/5-1]);
            load_points_x.append(x2[p/5-1]);
            load_points_y.append(y2[p/5-1]);
            QString point_infor;
            point_infor = "flow: " + QString::number(x2[p/5-1]) + "\n";
            point_infor += "head: " + QString::number(y2[p/5-1]) + "\n";
            point_infor += "pumps: " + QString::number(m_best_pump_qty[p]) + "\n";
            double power = m_pumpActualPerform[m_best_pump_qty[p]-1][p].input_power;
            point_infor += "power: " + QString::number(power) + "\n";
            point_infor += "efficiency: " + QString::number(m_best_effi[p]) + "\n";
            point_infor += "frequency: " + QString::number(m_best_Hz[p]);
            load_points_text.append(point_infor);
        }

        graph = ui->customPlot_2->addGraph();
        graph->setData(x2, y2);
        graph->setLineStyle(QCPGraph::lsNone);
        QCPScatterStyle scatterStyle(QCPScatterStyle::ssDisc,m_color[m_sysPara.dutyPumps],6);
        graph->setScatterStyle(scatterStyle);
        graph->removeFromLegend();

        ui->customPlot_2->setLoadPoints(load_points_x,load_points_y,load_points_text,m_color[m_sysPara.dutyPumps]);
        ui->customPlot_2->SetPlotVfCurve(true);
    }
    else
        ui->customPlot_2->SetPlotVfCurve(false);


    //set legend
    ui->customPlot_2->legend->setFont(QFont("微软雅黑",9));
    ui->customPlot_2->legend->setVisible(true);

    // set axis
    ui->customPlot_2->xAxis->setLabel("Flow (m3 / h)");
    ui->customPlot_2->yAxis->setLabel("Head (m)");
    ui->customPlot_2->yAxis2->setLabel("Efficiency (%)");

    ui->customPlot_2->xAxis->setRange(0, max_x*1.2);
    ui->customPlot_2->yAxis->setRange(0, max_y*1.2);
    ui->customPlot_2->yAxis2->setRange(0, 100);

    ui->customPlot_2->xAxis->setLabelFont(QFont("微软雅黑",9));
    ui->customPlot_2->yAxis->setLabelFont(QFont("微软雅黑",9));
    ui->customPlot_2->yAxis2->setLabelFont(QFont("微软雅黑",9));

    ui->customPlot_2->xAxis->setVisible(true);
    ui->customPlot_2->yAxis->setVisible(true);
    ui->customPlot_2->yAxis2->setVisible(true);

    //set plot
    ui->customPlot_2->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                      QCP::iSelectLegend | QCP::iSelectPlottables);

    //add tracer text
    ui->customPlot_2->addTracerText(QColor(0,0,0),QFont("微软雅黑",9));//x

    int tracerTextQty=m_sysPara.dutyPumps+1;

    if(ui->checkBox_EffiCurve->isChecked())
        tracerTextQty += m_sysPara.dutyPumps;

    if(ui->checkBox_BES->isChecked())
        tracerTextQty ++;

    if(ui->checkBox_FF->isChecked())
        tracerTextQty +=2;

    for(int i=0;i < tracerTextQty; i++)
    {
        ui->customPlot_2->addTracerText(ui->customPlot_2->graph(i)->pen().color(),QFont("微软雅黑",9));
    }

    //add tracer
    ui->customPlot_2->addTracer();

    //replot
    ui->customPlot_2->replot();
}

void MainWindow::show_single_pump_curve()
{
    double max_x = 0;
    double max_y = 0;
    double max_y2 = 0;
    QVector<double> x(101), y(101),p(101),e(101);

    //clear graph
    ui->customPlot->clearGraphs();
    ui->customPlot->clearTracerTexts();

    //add graph
    QCPGraph *graph1,*graph2,*graph3;
    for (int i=0; i<101; ++i)
    {
        double s_flow = m_pump.ReadPerformance().flow_max / 100 * i;
        x[i] = s_flow;
        y[i] = m_pump.GetHeadFromFlow(50,s_flow);
        p[i] = m_pump.GetPowerFromFlow(50,s_flow);

        if(p[i]>0)
            e[i] = x[i] * y[i] * 9.8 / p[i] / 36;

        if(y[i]>max_y) max_y=y[i];
        if(p[i]>max_y) max_y=p[i];
        if(e[i]>max_y2) max_y2=e[i];
    }

    graph1 = ui->customPlot->addGraph(ui->customPlot->xAxis,ui->customPlot->yAxis);
    graph1->setData(x, y);
    graph1->setPen(m_color[0]);
    graph1->setName("head");

    graph2 = ui->customPlot->addGraph(ui->customPlot->xAxis,ui->customPlot->yAxis);
    graph2->setData(x, p);
    graph2->setPen(m_color[1]);
    graph2->setName("power");

    graph3 = ui->customPlot->addGraph(ui->customPlot->xAxis,ui->customPlot->yAxis2);
    graph3->setData(x, e);
    graph3->setPen(m_color[2]);
    graph3->setName("effi.");

    //set legend
    ui->customPlot->legend->setFont(QFont("微软雅黑",9));
    ui->customPlot->legend->setVisible(true);

    //set axises
    ui->customPlot->xAxis->setLabel("Flow(m3/h)");
    ui->customPlot->yAxis->setLabel("Head(m) / Power(kW)");
    ui->customPlot->yAxis2->setLabel("Efficiency(%)");

    max_x = x[100];
    ui->customPlot->xAxis->setRange(0, max_x*1.2);
    ui->customPlot->yAxis->setRange(0, max_y*1.2);
    ui->customPlot->yAxis2->setRange(0, max_y2*1.2);

    ui->customPlot->xAxis->setLabelFont(QFont("微软雅黑",9));
    ui->customPlot->yAxis->setLabelFont(QFont("微软雅黑",9));
    ui->customPlot->yAxis2->setLabelFont(QFont("微软雅黑",9));

    ui->customPlot->xAxis->setVisible(true);
    ui->customPlot->yAxis->setVisible(true);
    ui->customPlot->yAxis2->setVisible(true);


    // set plot
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                      QCP::iSelectLegend | QCP::iSelectPlottables);

    //add tracer text

    ui->customPlot->addTracerText(QColor(0,0,0),QFont("微软雅黑",9));

    ui->customPlot->addTracerText(graph1->pen().color(),QFont("微软雅黑",9));
    ui->customPlot->addTracerText(graph2->pen().color(),QFont("微软雅黑",9));
    ui->customPlot->addTracerText(graph3->pen().color(),QFont("微软雅黑",9));

    //add tracer
    ui->customPlot->addTracer();

    //replot
    ui->customPlot->replot();
}

bool MainWindow::get_system_config_from_ui()
{
    if(!get_pump_config_from_ui())
        return false;
    get_system_design();

    QString errorMessage;
    if(!m_pumpSys.SysParaValidation(m_sysPara,&m_pump,&errorMessage))
    {
        QMessageBox::warning(nullptr,"Get system parameter failed",errorMessage);
        return false;
    }
    else
        m_pumpSys.WriteSysPara(m_sysPara);

    return true;
}

bool MainWindow::get_pump_config_from_ui()
{
    Pump::PerformData pumpPerformanceData = m_pump.ReadPerformance();
    Pump::MotorData motorData = m_pump.ReadMotorData();

    get_pump_selection();
    get_motor_selection();

    if(!m_pumpDb.getPumpData(m_pumpSelection,&pumpPerformanceData))
        return false;
    else
        m_pump.WritePerformance_2(pumpPerformanceData);

    if(!m_pumpDb.getMotorData(m_motorSelection,&motorData))
        return false;
    else
        m_pump.WriteMotorData(motorData);

    return true;
}

bool MainWindow::get_actual_perform(int pumps,double flow, double head, pumpActualPerform *perform)
{
    if(pumps <1) return 0;

    //get head
    double head_calcu;
    bool   eoc;
    head_calcu = m_pump.GetHeadFromFlow(50,flow / pumps,&eoc);

    //end of curve
    if(eoc)
    {
        perform->infor = 1;
        return 0;
    }

    //above curve
    if(head>head_calcu)
    {
        perform->infor = 2;
        return 0;
    }

    //calculate speed
    double Hz_calcu = m_pump.CalcuHz(flow / pumps,head);

    //calculate axis power
    double axis_power_calcu = m_pump.GetPowerFromFlow(Hz_calcu,flow / pumps);

    //calculate input power
    if(axis_power_calcu > m_pump.ReadMotorData().power_rate)
    {
        perform->infor = 3;
        return 0;
    }

    double input_power;
    double motor_effi = m_pump.GetMotorEffi(axis_power_calcu,Hz_calcu);

    if(motor_effi > 0)
        input_power = axis_power_calcu / motor_effi * 100;
    else
    {
        perform->infor = 4;
        return 0;
    }

    //calculate efficiency
    double pump_effi = head * flow / pumps * 9.8 / input_power / 36;
    if(pump_effi < 0 || pump_effi > 100)
    {
        perform->infor = 5;
        return 0;
    }

    //return value

    if(perform)
    {
        perform->Hz = Hz_calcu;
        perform->head = head;
        perform->pumps = pumps;
        perform->axis_power = axis_power_calcu;
        perform->efficiency = pump_effi;
        perform->total_flow = flow;
        perform->input_power = input_power;
    }
    else
    {
        perform->infor = 6;
        return 0;
    }

    return 1;
}

bool MainWindow::get_actual_perform_list()
{
    double flow_step = m_sysPara.designTotalFlow / 100;
    double current_flow = 0;
    double current_head;

    for(int i=0;i<101;i++)
    {
        current_head = m_pumpSys.GetControlCurve(current_flow);

        for(int j=0;j<m_pumpSys.ReadSysPara().dutyPumps;j++)
        {
            m_pumpActualPerform[j][i] = {0,0,0,0,0,0,0,0};
            bool result=get_actual_perform(j+1,current_flow,current_head,&m_pumpActualPerform[j][i]);
            if(j==(m_pumpSys.ReadSysPara().dutyPumps-1)
                    &&!result
                    &&(m_pumpActualPerform[j][i].infor==1||m_pumpActualPerform[j][i].infor==2||m_pumpActualPerform[j][i].infor==3))
            {
                QString infor1 = "Out of capacity of the designed pump or motor!\n\n";
                    infor1 += "system flow: " + QString::number(current_flow) + "\n";
                    infor1 += "system head: " + QString::number(current_head);
                    QMessageBox::warning(nullptr,"pump sizing information",infor1);
                    return false;
            }


        }
        current_flow += flow_step;
    }
    return true;
}

bool MainWindow::get_best_effi()
{
    if(!get_actual_perform_list())
        return false;

    m_best_pump_qty[0]=1;

    for(int i=1;i<101;i++)
    {
        m_best_effi[i] = 0;
        m_best_Hz[i] = 0;
        m_best_pump_qty[i] = 1;

        for(int j=0;j<m_sysPara.dutyPumps;j++)
        {
            if(m_pumpActualPerform[j][i].efficiency > m_best_effi[i])
            {
                m_best_effi[i] = m_pumpActualPerform[j][i].efficiency;
                m_best_pump_qty[i] = j+1;
                m_best_Hz[i] = m_pumpActualPerform[j][i].Hz;
            }
        }
    }
    //check motor size
    for(int i=1;i<101;i++)
    {
        int pumps = m_best_pump_qty[i];
        if(m_pumpActualPerform[pumps-1][i].axis_power > m_pump.ReadMotorData().power_rate)
        {
            QString infor = "Axis power of the pump is greater than rated power of the motor!\n\n";
            infor += "system flow: " + QString::number(m_pumpActualPerform[pumps-1][i].total_flow) + "\n";
            infor += "pump flow: " + QString::number(m_pumpActualPerform[pumps-1][i].total_flow/m_pumpActualPerform[pumps-1][i].pumps) + "\n";
            infor += "pump head: " + QString::number(m_pumpActualPerform[pumps-1][i].head) + "\n";
            infor += "axis power: " + QString::number(m_pumpActualPerform[pumps-1][i].axis_power);
            QMessageBox::warning(nullptr,"motor sizing information",infor);
            return false;

        }
    }
    return true;
}

bool MainWindow::get_fix_effi()
{
    if(!get_actual_perform_list())
        return false;

    //flow rising
    m_fix_pump_qty_1[0]=1;

    for(int i=1;i<101;i++)
    {

        m_fix_pump_qty_1[i]=m_fix_pump_qty_1[i-1];

        while((m_fix_pump_qty_1[i] < m_sysPara.dutyPumps)
              && (m_pumpActualPerform[m_fix_pump_qty_1[i]-1][i].Hz >=50
                || m_pumpActualPerform[m_fix_pump_qty_1[i]-1][i].efficiency <= 0.01))
        {
            m_fix_pump_qty_1[i]++;
        }

        m_fix_effi_1[i] = m_pumpActualPerform[m_fix_pump_qty_1[i]-1][i].efficiency;
        m_fix_Hz_1[i] = m_pumpActualPerform[m_fix_pump_qty_1[i]-1][i].Hz;
    }
    //check motor size
    for(int i=1;i<101;i++)
    {
        int pumps = m_fix_pump_qty_1[i];
        if(m_pumpActualPerform[pumps-1][i].axis_power > m_pump.ReadMotorData().power_rate)
        {
            QString infor = "Axis power of the pump is greater than rated power of the motor!\n\n";
            infor += "system flow: " + QString::number(m_pumpActualPerform[pumps-1][i].total_flow) + "\n";
            infor += "pump flow: " + QString::number(m_pumpActualPerform[pumps-1][i].total_flow/m_pumpActualPerform[pumps-1][i].pumps) + "\n";
            infor += "pump head: " + QString::number(m_pumpActualPerform[pumps-1][i].head) + "\n";
            infor += "axis power: " + QString::number(m_pumpActualPerform[pumps-1][i].axis_power);
            QMessageBox::warning(nullptr,"motor sizing information",infor);
            return false;

        }
    }

    //flow droping
    m_fix_pump_qty_2[100]=m_fix_pump_qty_1[100];
    m_fix_effi_2[100] = m_fix_effi_1[100];
    m_fix_Hz_2[100] =  m_fix_Hz_1[100];

    for(int i=99;i>0;i--)
    {

        m_fix_pump_qty_2[i]=m_fix_pump_qty_2[i+1];

        while((m_fix_pump_qty_2[i] > 1)
              && m_pumpActualPerform[m_fix_pump_qty_2[i]-1][i].Hz < 30
                && m_pumpActualPerform[m_fix_pump_qty_2[i]-1][i-1].Hz <50)
        {
            m_fix_pump_qty_2[i]--;
        }

        m_fix_effi_2[i] = m_pumpActualPerform[m_fix_pump_qty_2[i]-1][i].efficiency;
        m_fix_Hz_2[i] = m_pumpActualPerform[m_fix_pump_qty_2[i]-1][i].Hz;
    }
    m_fix_pump_qty_2[0]= m_fix_pump_qty_2[1];
    m_fix_effi_2[0] = m_fix_effi_2[1];
    m_fix_Hz_2[0] =  m_fix_Hz_2[1];

    //check motor size
    for(int i=1;i<101;i++)
    {
        int pumps = m_fix_pump_qty_2[i];
        if(m_pumpActualPerform[pumps-1][i].axis_power > m_pump.ReadMotorData().power_rate)
        {
            QString infor = "Axis power of the pump is greater than rated power of the motor!\n\n";
            infor += "system flow: " + QString::number(m_pumpActualPerform[pumps-1][i].total_flow) + "\n";
            infor += "pump flow: " + QString::number(m_pumpActualPerform[pumps-1][i].total_flow/m_pumpActualPerform[pumps-1][i].pumps) + "\n";
            infor += "pump head: " + QString::number(m_pumpActualPerform[pumps-1][i].head) + "\n";
            infor += "axis power: " + QString::number(m_pumpActualPerform[pumps-1][i].axis_power);
            QMessageBox::warning(nullptr,"motor sizing information",infor);
            return false;

        }
    }
    return true;
}
