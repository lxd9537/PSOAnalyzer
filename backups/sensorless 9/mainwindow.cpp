#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_xls(nullptr)
{
    ui->setupUi(this);

    connect(ui->actionSet_database,&QAction::triggered,this,&MainWindow::on_SetDbPara_clicked);
    connect(ui->actionLoad,&QAction::triggered,this,&MainWindow::on_Load_clicked);
    connect(ui->actionSaveAs,&QAction::triggered,this,&MainWindow::on_SaveAs_clicked);
    connect(ui->actionGenerate_report,&QAction::triggered,this,&MainWindow::on_GenerateReport_clicked);
    connect(ui->actionGenerate_report_2,&QAction::triggered,this,&MainWindow::on_GenerateReport2_clicked);


    connect(ui->comboBox_pumpFamily,QOverload<const QString &>::of(&QComboBox::currentIndexChanged),this,&MainWindow::on_family_changed);
    connect(ui->comboBox_pumpModel,QOverload<const QString &>::of(&QComboBox::currentIndexChanged),this,&MainWindow::on_model_changed);
    connect(ui->comboBox_pumpSpeed,QOverload<const QString &>::of(&QComboBox::currentIndexChanged),this,&MainWindow::on_speed_changed);
    connect(ui->comboBox_motor,QOverload<const QString &>::of(&QComboBox::currentIndexChanged),this,&MainWindow::on_motor_changed);
    connect(ui->comboBox_loadprofile_app,QOverload<const QString &>::of(&QComboBox::currentIndexChanged),this,&MainWindow::on_loadprofile_app_changed);

    connect(ui->comboBox_control_method,QOverload<const QString &>::of(&QComboBox::currentIndexChanged),this,&MainWindow::on_method_1_changed);
    connect(ui->comboBox_control_method_2,QOverload<const QString &>::of(&QComboBox::currentIndexChanged),this,&MainWindow::on_method_2_changed);

    connect(ui->pushButton_connect,&QPushButton::clicked,this,&MainWindow::on_connect_clicked);
    connect(ui->pushButton_show_multi_pump_curve,&QPushButton::clicked,this,&MainWindow::on_multi_pump_clicked);
    connect(ui->pushButton_show_single_pump_curve,&QPushButton::clicked,this,&MainWindow::on_single_pump_clicked);
    connect(ui->pushButton_transport_curve,&QPushButton::clicked,this,&MainWindow::on_transport_clicked);

    connect(ui->pushButton_show_loadprofile,&QPushButton::clicked,this,&MainWindow::on_show_loadprofile_clicked);
    connect(ui->pushButton_edit_loadprofile,&QPushButton::clicked,this,&MainWindow::on_edit_loadprofile_clicked);
    connect(ui->pushButton_new_loadprofile,&QPushButton::clicked,this,&MainWindow::on_new_loadprofile_clicked);
    connect(ui->pushButton_delete_loadprofile,&QPushButton::clicked,this,&MainWindow::on_delete_loadprofile_clicked);


    m_dialog_getdbpara = new Dialog_GetDbPara();
    m_showLoadprofileDialog = new Dialog_ShowLoadProfile();
    m_editLoadprofileDialog = new Dialog_EditLoadProfile();

    m_loadprofile_data = new QStandardItemModel(20,3,this);


}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_dialog_getdbpara;
    //delete m_loadprofile_data;
    //delete m_dialog;
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

    /*
    ui->comboBox_control_method->setCurrentText(configIniRead->value("operation mode 1/control method").toString());
    ui->lineEdit_StageHz->setText(configIniRead->value("operation mode 1/staging Hz").toString());
    ui->lineEdit_DestageHz->setText(configIniRead->value("operation mode 1/destaging Hz").toString());
    ui->checkBox_power_curve->setChecked(configIniRead->value("operation mode 1/plot power curve").toString()=="yes"?true:false);
    ui->checkBox_effi_curve->setChecked(configIniRead->value("operation mode 1/plot efficiency curve").toString()=="yes"?true:false);
    ui->checkBox_vfd_curve->setChecked(configIniRead->value("operation mode 1/plot variable speed curve").toString()=="yes"?true:false);

    ui->comboBox_control_method_2->setCurrentText(configIniRead->value("operation mode 2/control method").toString());
    ui->lineEdit_StageHz_2->setText(configIniRead->value("operation mode 2/staging Hz").toString());
    ui->lineEdit_DestageHz_2->setText(configIniRead->value("operation mode 2/destaging Hz").toString());
    ui->checkBox_power_curve_2->setChecked(configIniRead->value("operation mode 2/plot power curve").toString()=="yes"?true:false);
    ui->checkBox_effi_curve_2->setChecked(configIniRead->value("operation mode 2/plot efficiency curve").toString()=="yes"?true:false);
    ui->checkBox_vfd_curve_2->setChecked(configIniRead->value("operation mode 2/plot variable speed curve").toString()=="yes"?true:false);
    */

    delete configIniRead;
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

    /*
    configIniWrite->setValue("operation mode 1/control method", ui->comboBox_control_method->currentText());
    configIniWrite->setValue("operation mode 1/staging Hz", ui->lineEdit_StageHz->text());
    configIniWrite->setValue("operation mode 1/destaging Hz", ui->lineEdit_DestageHz->text());
    configIniWrite->setValue("operation mode 1/plot power curve", ui->checkBox_power_curve->isChecked()?"yes":"no");
    configIniWrite->setValue("operation mode 1/plot efficiency curve", ui->checkBox_effi_curve->isChecked()?"yes":"no");
    configIniWrite->setValue("operation mode 1/plot variable speed curve", ui->checkBox_vfd_curve->isChecked()?"yes":"no");

    configIniWrite->setValue("operation mode 2/control method", ui->comboBox_control_method_2->currentText());
    configIniWrite->setValue("operation mode 2/staging Hz", ui->lineEdit_StageHz_2->text());
    configIniWrite->setValue("operation mode 2/destaging Hz", ui->lineEdit_DestageHz_2->text());
    configIniWrite->setValue("operation mode 2/plot power curve", ui->checkBox_power_curve_2->isChecked()?"yes":"no");
    configIniWrite->setValue("operation mode 2/plot efficiency curve", ui->checkBox_effi_curve_2->isChecked()?"yes":"no");
    configIniWrite->setValue("operation mode 2/plot variable speed curve", ui->checkBox_vfd_curve_2->isChecked()?"yes":"no");
    */

    delete configIniWrite;
    QMessageBox::information(nullptr,"Information","Save configuration succeed!");
}

void MainWindow::on_GenerateReport_clicked()
{
    //check database connection
    if(!m_pumpDb.getConnectStatus())
    {
        QMessageBox::warning(nullptr,"Warning", "Database is not connected!");
        return;
    }

    Dialog_ReportOption *dlg;
    dlg = new Dialog_ReportOption;
    if(dlg->exec()==QDialog::Accepted)
    {
        m_reportOption = dlg->getReportOption();
        fillReport();
    }
    delete dlg;
}

void MainWindow::on_GenerateReport2_clicked()
{
    //check database connection
    if(!m_pumpDb.getConnectStatus())
    {
        QMessageBox::warning(nullptr,"Warning", "Database is not connected!");
        return;
    }

    Dialog_ReportOption_2 *dlg;
    dlg = new Dialog_ReportOption_2;
    if(dlg->exec()==QDialog::Accepted)
    {
        m_reportOption_2 = dlg->getReportOption();
        fillReport2();
    }
    delete dlg;
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

void MainWindow::on_method_1_changed()
{
        ui->checkBox_power_curve->setDisabled(true);
        ui->checkBox_effi_curve->setDisabled(true);
        ui->checkBox_vfd_curve->setDisabled(true);
        ui->lineEdit_StageHz->setDisabled(true);
        ui->lineEdit_DestageHz->setDisabled(true);

        if(ui->comboBox_control_method->currentIndex() > 0)
        {
            ui->checkBox_power_curve->setEnabled(true);
            ui->checkBox_effi_curve->setEnabled(true);
        }

        if(ui->comboBox_control_method->currentIndex() >= 2
            && ui->comboBox_control_method->currentIndex() <= 5)
        {
            ui->lineEdit_StageHz->setEnabled(true);
            ui->lineEdit_DestageHz->setEnabled(true);
        }

        if(ui->comboBox_control_method->currentIndex()>=4
                && ui->comboBox_control_method->currentIndex()<=6)
            ui->checkBox_vfd_curve->setEnabled(true);
        else
            ui->checkBox_vfd_curve->setChecked(false);
}

void MainWindow::on_method_2_changed()
{
        ui->checkBox_power_curve_2->setDisabled(true);
        ui->checkBox_effi_curve_2->setDisabled(true);
        ui->checkBox_vfd_curve_2->setDisabled(true);
        ui->lineEdit_StageHz_2->setDisabled(true);
        ui->lineEdit_DestageHz_2->setDisabled(true);

        if(ui->comboBox_control_method_2->currentIndex() > 0)
        {
            ui->checkBox_power_curve_2->setEnabled(true);
            ui->checkBox_effi_curve_2->setEnabled(true);
        }

        if(ui->comboBox_control_method_2->currentIndex() >= 2
            && ui->comboBox_control_method_2->currentIndex() <= 5)
        {
            ui->lineEdit_StageHz_2->setEnabled(true);
            ui->lineEdit_DestageHz_2->setEnabled(true);
        }

        if(ui->comboBox_control_method_2->currentIndex()>=4
                && ui->comboBox_control_method_2->currentIndex()<=6)
            ui->checkBox_vfd_curve_2->setEnabled(true);
        else
            ui->checkBox_vfd_curve_2->setChecked(false);

}

void MainWindow::on_loadprofile_app_changed()
{
    if(m_pumpDb.getConnectStatus())
    {
        m_pumpDb.getLoadprofile(ui->comboBox_loadprofile_app->currentText(), m_loadprofile_data);
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
            ui->comboBox_pumpFamily->addItem(pump_family);

        QStringList motor_list;
        m_pumpDb.getMotorList(&motor_list);

        foreach(QString motor,motor_list)
            ui->comboBox_motor->addItem(motor);

        QStringList loadProfileAppList;
        m_pumpDb.getLoadprofileList(&loadProfileAppList);

        foreach(QString loadProfileApp, loadProfileAppList)
            ui->comboBox_loadprofile_app->addItem(loadProfileApp);

        m_pumpDb.getLoadprofile(ui->comboBox_loadprofile_app->currentText(), m_loadprofile_data);
     }
}

void MainWindow::on_single_pump_clicked()
{
    if(!get_pump_selection()) return;

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
    graph1->setPen(m_color[0][0]);
    graph1->setName("head");

    graph2 = ui->customPlot->addGraph(ui->customPlot->xAxis,ui->customPlot->yAxis);
    graph2->setData(x, p);
    graph2->setPen(m_color[0][1]);
    graph2->setName("power");

    graph3 = ui->customPlot->addGraph(ui->customPlot->xAxis,ui->customPlot->yAxis2);
    graph3->setData(x, e);
    graph3->setPen(m_color[0][2]);
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

void MainWindow::on_multi_pump_clicked()
{

    if(!get_system_profile()) return;

    //clear graph
    ui->customPlot_2->clearGraphs();
    ui->customPlot_2->clearTracerTexts();
    ui->customPlot_2->clearCrossPointText();

    //get operation mode
    if(!get_operation_mode())
        return;

     pointsOnCurve pointsOnCurve[2];

    //plot curves for operation mode
     for(int i=0; i<2; i++){
         switch (m_operationMode[i].controlMethod) {
         case none:break;
         case fullSpeed: generatePointsOnCurve_fullSpeed(&pointsOnCurve[i], m_operationMode[i]);break;
         case oneVfdC1: generatePointsOnCurve_oneVfdC1(&pointsOnCurve[i], m_operationMode[i]);break;
         case oneVfdC2: generatePointsOnCurve_oneVfdC2(&pointsOnCurve[i], m_operationMode[i]);break;
         case allVfdHzC1: generatePointsOnCurve_allVfdHzC1(&pointsOnCurve[i], m_operationMode[i]);break;
         case allVfdHzC2: generatePointsOnCurve_allVfdHzC2(&pointsOnCurve[i], m_operationMode[i]);break;
         case allVfdEffi: generatePointsOnCurve_allVfdEffi(&pointsOnCurve[i], m_operationMode[i]);break;
         }
     }

    //plot graph
    QCPGraph    *graph;
    QVector<double> flow(101), head(101), power(101), effi(101);

    //add tracer text for x Axis
    ui->customPlot_2->addTracerText(QColor(0,0,0),QFont("微软雅黑",9));

    //add graph
    for(int j=0; j<2; j++)
    {
        if(m_operationMode[j].controlMethod==none) continue;

        for (int i=0; i<101; i++)
        {
            flow[i]     = pointsOnCurve[j].flow_total[i];
            head[i]     = pointsOnCurve[j].head[i];
            power[i]    = pointsOnCurve[j].power_input_total[i];
            effi[i]     = pointsOnCurve[j].effi_total[i];
        }

        graph = ui->customPlot_2->addGraph(ui->customPlot_2->xAxis,ui->customPlot_2->yAxis);
        graph->setData(flow, head);
        graph->setPen(QPen(m_color[j][0],2));

        QString modeType;

        switch(m_operationMode[j].controlMethod)
        {
        case none:break;
        case fullSpeed: modeType="Full speed";break;
        case oneVfdC1: modeType="One VFD 1";break;
        case oneVfdC2: modeType="One VFD 2";break;
        case allVfdHzC1: modeType="All VFD 1";break;
        case allVfdHzC2: modeType="All VFD 2";break;
        case allVfdEffi: modeType="BES";break;
        }

        graph->setName(modeType + " control curve");

        ui->customPlot_2->addTracerText(m_color[j][0],QFont("微软雅黑",9));

        if(m_operationMode[j].plotPowerCurve)
        {
            graph = ui->customPlot_2->addGraph(ui->customPlot_2->xAxis,ui->customPlot_2->yAxis2);
            graph->setData(flow, power);
            graph->setPen(QPen(m_color[j][1],2));
            graph->setName(modeType + " power curve");

            ui->customPlot_2->addTracerText(m_color[j][1],QFont("微软雅黑",9));
        }

        if(m_operationMode[j].plotEfficiencyCurve)
        {
            graph = ui->customPlot_2->addGraph(ui->customPlot_2->xAxis,ui->customPlot_2->yAxis2);
            graph->setData(flow, effi);
            graph->setPen(QPen(m_color[j][2],2));
            graph->setName(modeType + " efficiency curve");

            ui->customPlot_2->addTracerText(m_color[j][2],QFont("微软雅黑",9));
        }
    }

    //add vfd cross point
    QVector<double>     cross_points_flow[2],*p1[2];
    QVector<double>     cross_points_head[2],*p2[2];
    QVector<QString>    cross_points_text[2],*p3[2];
    QColor              cross_points_color[2],*p4[2];
    bool                cross_points_enable[2];

    for (int j=0;j<2;j++)
    {
        cross_points_enable[j] = m_operationMode[j].plotVfdCurves;

        if(!cross_points_enable[j]) continue;

        QVector<double>     flow_2(20), head_2(20);


        for(int p=5;p<101;p+=5)
        {
            for (int i=0; i<101; ++i)
            {
                double f = m_pump.ReadPerformance().flow_max * pointsOnCurve[j].Hz_vfd[p] / 50 / 100 * i;
                flow[i] = f * pointsOnCurve[j].pumps[p];
                head[i] = m_pump.GetHeadFromFlow(pointsOnCurve[j].Hz_vfd[p],f);
            }
            graph = ui->customPlot_2->addGraph();
            graph->setData(flow, head);

            QPen pen;
            pen.setColor(m_color_2[j][pointsOnCurve[j].pumps[p]-1]);
            pen.setStyle(Qt::DashLine);
            pen.setWidth(1);
            graph->setPen(pen);
            graph->removeFromLegend();

            flow_2[p/5-1] = m_sysPara.designTotalFlow / 100 * p;
            head_2[p/5-1] = pointsOnCurve[j].head[p];
            cross_points_flow[j].append(flow_2[p/5-1]);
            cross_points_head[j].append(head_2[p/5-1]);

            QString point_infor;
            point_infor  = "flow: " + QString::number(flow_2[p/5-1]) + "\n";
            point_infor += "head: " + QString::number(head_2[p/5-1]) + "\n";
            point_infor += "pumps: " + QString::number(pointsOnCurve[j].pumps[p]) + "\n";
            point_infor += "power: " + QString::number(pointsOnCurve[j].power_input_total[p]) + "\n";
            point_infor += "efficiency: " + QString::number(pointsOnCurve[j].effi_total[p]) + "\n";
            point_infor += "frequency: " + QString::number(pointsOnCurve[j].Hz_vfd[p]);
            cross_points_text[j].append(point_infor);

            cross_points_color[j] = m_color[j][0];
        }

        //head curve with disc style
        graph = ui->customPlot_2->addGraph();
        graph->setData(flow_2, head_2);
        graph->setLineStyle(QCPGraph::lsNone);
        graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,m_color[j][0],6));
        graph->removeFromLegend();
    }

    for(int j=0;j<2;j++)
    {
        p1[j] = &cross_points_flow[j];
        p2[j] = &cross_points_head[j];
        p3[j] = &cross_points_text[j];
        p4[j] = &cross_points_color[j];
    }
    ui->customPlot_2->setVfdCrossPoints(p1,p2,p3,p4,cross_points_enable);

    //set legend
    ui->customPlot_2->legend->setFont(QFont("微软雅黑",9));
    ui->customPlot_2->legend->setVisible(true);

    //set axis
    ui->customPlot_2->xAxis->setLabel("Flow (m3 / h)");
    ui->customPlot_2->yAxis->setLabel("Head (m)");
    ui->customPlot_2->yAxis2->setLabel("Power (kW) / Efficiency (%)");

    double max_x = m_sysPara.designTotalFlow;
    double max_y = m_sysPara.headAtTotalFlow;
    double max_z = qMax(m_pump.ReadPerformance().power_max * m_sysPara.dutyPumps,90.0);

    ui->customPlot_2->xAxis->setRange(0, max_x * 1.4);
    ui->customPlot_2->yAxis->setRange(0, max_y * 1.2);
    ui->customPlot_2->yAxis2->setRange(0, max_z);

    ui->customPlot_2->xAxis->setLabelFont(QFont("微软雅黑",9));
    ui->customPlot_2->yAxis->setLabelFont(QFont("微软雅黑",9));
    ui->customPlot_2->yAxis2->setLabelFont(QFont("微软雅黑",9));

    ui->customPlot_2->xAxis->setVisible(true);
    ui->customPlot_2->yAxis->setVisible(true);
    ui->customPlot_2->yAxis2->setVisible(true);

    //set plot
    ui->customPlot_2->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                QCP::iSelectLegend | QCP::iSelectPlottables);

    //add tracer
    ui->customPlot_2->addTracer();

    //replot
    ui->customPlot_2->replot();

}


void MainWindow::on_transport_clicked()
{

    if(!get_system_profile()) return;

    //clear graph
    ui->customPlot_3->clearGraphs();
    ui->customPlot_3->clearTracerTexts();
    ui->customPlot_3->clearCrossPointText();

    pointsOnCurve pointsOnCurve[10];
    operationMode operationMode;
    operationMode.controlMethod = allVfdEffi;
    generatePointsOnCurve_specificEnergy(&pointsOnCurve,operationMode);

    //plot graph
    QCPGraph    *graph;

    //add tracer text for x Axis
    ui->customPlot_3->addTracerText(QColor(0,0,0),QFont("微软雅黑",9));

    //add graph Es
    double max_Es = 0;
    for(int j=0; j<m_sysPara.dutyPumps; j++)
    {
        graph = ui->customPlot_3->addGraph(ui->customPlot_3->xAxis,ui->customPlot_3->yAxis2);

        for (int i=(j+1)*5; i<101; i++)
        {
            if(pointsOnCurve[j].flow_total[i] > 0 && pointsOnCurve[j].energy_specific[i] > 0)
                graph->addData(pointsOnCurve[j].flow_total[i], pointsOnCurve[j].energy_specific[i]);
            if(pointsOnCurve[j].energy_specific[i] > max_Es)
                max_Es = pointsOnCurve[j].energy_specific[i];
        }

        graph->setPen(QPen(m_color[0][j],2));
        graph->setName(QString::number(j) + " Pumps-Es");

        ui->customPlot_3->addTracerText(m_color[0][j],QFont("微软雅黑",9));
    }

    //add system curve
    graph = ui->customPlot_3->addGraph(ui->customPlot_3->xAxis,ui->customPlot_3->yAxis);
    for(int i=0; i<101; i++)
    {
        graph->addData(m_sysPara.designTotalFlow/100*i, m_pumpSys.GetControlCurve(m_sysPara.designTotalFlow/100*i));
    }
    graph->setPen(QPen(m_color[1][5],2));
    graph->setName("System");

    ui->customPlot_3->addTracerText(m_color[1][5],QFont("微软雅黑",9));


    //add vfd cross point
    QVector<double>     cross_points_flow[2],*p1[2];
    QVector<double>     cross_points_head[2],*p2[2];
    QVector<QString>    cross_points_text[2],*p3[2];
    QColor              cross_points_color[2],*p4[2];
    bool                cross_points_enable[2];

    struct pointsOnCurve       pointsOnCurve_2[2];

    cross_points_enable[0] = true;
    cross_points_enable[1] = false;

    generatePointsOnCurve_bestEs(pointsOnCurve_2, operationMode);

    for (int j=0;j<2;j++)
    {
       if(!cross_points_enable[j]) continue;

       QVector<double>      flow(101), head(101), power(101), effi(101);
       QVector<double>      flow_2(20), head_2(20);

        for(int p=5;p<101;p+=5)
        {
            for (int i=0; i<101; ++i)
            {
                double f = m_pump.ReadPerformance().flow_max * pointsOnCurve_2[j].Hz_vfd[p] / 50 / 100 * i;
                flow[i] = f * pointsOnCurve_2[j].pumps[p];
                head[i] = m_pump.GetHeadFromFlow(pointsOnCurve_2[j].Hz_vfd[p],f);
            }
            graph = ui->customPlot_3->addGraph();
            graph->setData(flow, head);
            QPen pen;
            pen.setColor(m_color[j][3]);
            pen.setStyle(Qt::DashLine);
            pen.setWidth(1);
            graph->setPen(pen);
            graph->removeFromLegend();

            flow_2[p/5-1] = pointsOnCurve_2[j].flow_total[p];
            head_2[p/5-1] = pointsOnCurve_2[j].head[p];
            cross_points_flow[j].append(flow_2[p/5-1]);
            cross_points_head[j].append(head_2[p/5-1]);

            QString point_infor;
            point_infor  = "flow: " + QString::number(flow_2[p/5-1]) + "\n";
            point_infor += "head: " + QString::number(head_2[p/5-1]) + "\n";
            point_infor += "pumps: " + QString::number(pointsOnCurve_2[j].pumps[p]) + "\n";
            point_infor += "power: " + QString::number(pointsOnCurve_2[j].power_input_total[p]) + "\n";
            point_infor += "efficiency: " + QString::number(pointsOnCurve_2[j].effi_total[p]) + "\n";
            point_infor += "frequency: " + QString::number(pointsOnCurve_2[j].Hz_vfd[p]);
            cross_points_text[j].append(point_infor);
        }

        //head curve with disc style
        graph = ui->customPlot_3->addGraph();
        graph->setData(flow_2, head_2);
        graph->setLineStyle(QCPGraph::lsNone);
        graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,m_color[1][5],6));
        graph->removeFromLegend();
    }

    cross_points_color[0] = m_color[1][5];
    cross_points_color[1] = m_color[1][5];

    for(int j=0;j<2;j++)
    {
        p1[j] = &cross_points_flow[j];
        p2[j] = &cross_points_head[j];
        p3[j] = &cross_points_text[j];
        p4[j] = &cross_points_color[j];
    }
    ui->customPlot_3->setVfdCrossPoints(p1,p2,p3,p4,cross_points_enable);

    //set legend
    ui->customPlot_3->legend->setFont(QFont("微软雅黑",9));
    ui->customPlot_3->legend->setVisible(true);

    //set axis
    ui->customPlot_3->xAxis->setLabel("Flow (m3 / h)");
    ui->customPlot_3->yAxis->setLabel("Head (m)");
    ui->customPlot_3->yAxis2->setLabel("Power (kW) / Es (kWh/m3)");

    double max_x = m_sysPara.designTotalFlow * 1.3;
    double max_y = m_sysPara.headAtTotalFlow * 1.3;
    double max_z = max_Es * 1.3;

    ui->customPlot_3->xAxis->setRange(0, max_x);
    ui->customPlot_3->yAxis->setRange(0, max_y);
    ui->customPlot_3->yAxis2->setRange(0, max_z);

    ui->customPlot_3->xAxis->setLabelFont(QFont("微软雅黑",9));
    ui->customPlot_3->yAxis->setLabelFont(QFont("微软雅黑",9));
    ui->customPlot_3->yAxis2->setLabelFont(QFont("微软雅黑",9));

    ui->customPlot_3->xAxis->setVisible(true);
    ui->customPlot_3->yAxis->setVisible(true);
    ui->customPlot_3->yAxis2->setVisible(true);

    //set plot
    ui->customPlot_3->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                QCP::iSelectLegend | QCP::iSelectPlottables);

    //add tracer
    ui->customPlot_3->addTracer();

    //replot
    ui->customPlot_3->replot();

}


void MainWindow::on_show_loadprofile_clicked()
{
    if(m_pumpDb.getConnectStatus())
    {
        if(!m_pumpDb.getLoadprofile(ui->comboBox_loadprofile_app->currentText(), m_loadprofile_data))
            return;

        QString app = m_loadprofile_data->item(0,0)->text();
        m_showLoadprofileDialog->setData(m_loadprofile_data, app);
        m_showLoadprofileDialog->open();

    }
    else
    {
        QMessageBox::warning(nullptr,"Warning", "Database is not connected!");
        return;
    }
}

void MainWindow::on_edit_loadprofile_clicked()
{
    if(m_pumpDb.getConnectStatus())
    {
        m_pumpDb.getLoadprofile(ui->comboBox_loadprofile_app->currentText(), m_loadprofile_data);

        QString app = m_loadprofile_data->item(0,0)->text();
        m_editLoadprofileDialog->setData(m_loadprofile_data, app);

        if(m_editLoadprofileDialog->exec()==QDialog::Accepted)
        {
            m_pumpDb.updateLoadprofile(m_loadprofile_data);
        }
    }
    else
    {
        QMessageBox::warning(nullptr,"Warning", "Database is not connected!");
        return;
    }
}

void MainWindow::on_new_loadprofile_clicked()
{
    if(m_pumpDb.getConnectStatus())
    {

        bool isOK;
        QString newLoadprofileName = QInputDialog::getText(this, "Input Dialog","Please input your comment",
                                             QLineEdit::Normal, "new load profile", &isOK);
        if(isOK)
        {
            if(!(ui->comboBox_loadprofile_app->findText(newLoadprofileName)==-1))
            {
                QMessageBox::warning(nullptr,"Warning","The name you have input is already existing in the database!");
                return;
            }
        }

        if(m_pumpDb.newLoadprofile(newLoadprofileName))
        {
            ui->comboBox_loadprofile_app->addItem(newLoadprofileName);
            ui->comboBox_loadprofile_app->setCurrentIndex(ui->comboBox_loadprofile_app->findText(newLoadprofileName));
        }
    }
    else
    {
        QMessageBox::warning(nullptr,"Warning", "Database is not connected!");
        return;
    }
}

void MainWindow::on_delete_loadprofile_clicked()
{
    if(m_pumpDb.getConnectStatus())
    {
        m_pumpDb.getLoadprofile(ui->comboBox_loadprofile_app->currentText(), m_loadprofile_data);

        QString app = m_loadprofile_data->item(0,0)->text();

        QMessageBox msg(this);
        msg.setWindowTitle("Confirmation");
        msg.setText("Are you sure to delete load profile " + app +"?");
        msg.setStandardButtons(QMessageBox::Yes|QMessageBox::No);

        if(msg.exec()==QMessageBox::Yes)
        {
            m_pumpDb.deleteLoadprofile(app);
            ui->comboBox_loadprofile_app->removeItem(ui->comboBox_loadprofile_app->findText(app));
        }
        else return;
    }
    else
    {
        QMessageBox::warning(nullptr,"Warning", "Database is not connected!");
        return;
    }

}

bool MainWindow::get_pump_selection()
{
    Pump::PerformData pumpPerformanceData = m_pump.ReadPerformance();
    Pump::MotorData motorData = m_pump.ReadMotorData();

    m_pumpSelection.family = ui->comboBox_pumpFamily->currentText();
    m_pumpSelection.model = ui->comboBox_pumpModel->currentText();
    m_pumpSelection.speed = ui->comboBox_pumpSpeed->currentText().toInt();
    m_pumpSelection.impTrimed = ui->lineEdit_impTrimed->text().toDouble();
    m_pumpSelection.motorPower = ui->comboBox_motor->currentText();
    m_pumpSelection.motorPoles = ui->comboBox_motor_poles->currentText().toInt();

    m_motorSelection.power_rate = ui->comboBox_motor->currentText().toDouble();
    m_motorSelection.poles = ui->comboBox_motor_poles->currentText().toInt();

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

bool MainWindow::get_system_profile()
{
    if(!get_pump_selection())
        return false;

    m_sysPara.dutyPumps = ui->comboBox_duty_pumps->currentText().toInt();
    m_sysPara.designTotalFlow = ui->lineEdit_total_flow->text().toDouble();
    m_sysPara.headAtZeroFlow = ui->lineEdit_head_zero_flow->text().toDouble();
    m_sysPara.headAtTotalFlow = ui->lineEdit_head_total_flow->text().toDouble();
    m_sysPara.controlCurveShape = ui->lineEdit_control_curve_shaping->text().toDouble();

    QString errorMessage;
    if(!m_pumpSys.SysParaValidation(m_sysPara,&m_pump,&errorMessage))
    {
        QMessageBox::warning(nullptr,"Warning",errorMessage);
        return false;
    }
    else
        m_pumpSys.WriteSysPara(m_sysPara);

    return true;
}

bool MainWindow::get_operation_mode()
{
    //operation 1
    m_operationMode[0].modeName = "operation mode 1";
    switch (ui->comboBox_control_method_2->currentIndex()) {
    case 0: m_operationMode[0].controlMethod = none;break;
    case 1: m_operationMode[0].controlMethod = fullSpeed;break;
    case 2: m_operationMode[0].controlMethod = oneVfdC1;break;
    case 3: m_operationMode[0].controlMethod = oneVfdC2;break;
    case 4: m_operationMode[0].controlMethod = allVfdHzC1;break;
    case 5: m_operationMode[0].controlMethod = allVfdHzC2;break;
    case 6: m_operationMode[0].controlMethod = allVfdEffi;break;
    }

    if(ui->lineEdit_StageHz_2->text().toDouble()>=0 && ui->lineEdit_StageHz_2->text().toDouble()<=50)
        m_operationMode[0].stagingHz = ui->lineEdit_StageHz_2->text().toDouble();
    else {
        QMessageBox::warning(this,"Warning","please check staging Hz of operation mode 1!");
        return false;
    }

    if(ui->lineEdit_DestageHz_2->text().toDouble()>=0 && ui->lineEdit_DestageHz_2->text().toDouble()<=50)
        m_operationMode[0].destagingHz = ui->lineEdit_DestageHz_2->text().toDouble();
    else {
        QMessageBox::warning(this,"Warning","please check destaging Hz of operation mode 1!");
        return false;
    }

    m_operationMode[0].plotPowerCurve = ui->checkBox_power_curve_2->isChecked();
    m_operationMode[0].plotEfficiencyCurve = ui->checkBox_effi_curve_2->isChecked();
    m_operationMode[0].plotVfdCurves = ui->checkBox_vfd_curve_2->isChecked();


    //operation 2
    m_operationMode[1].modeName = "operation mode 2";
    switch (ui->comboBox_control_method->currentIndex()) {
    case 0: m_operationMode[1].controlMethod = none;break;
    case 1: m_operationMode[1].controlMethod = fullSpeed;break;
    case 2: m_operationMode[1].controlMethod = oneVfdC1;break;
    case 3: m_operationMode[1].controlMethod = oneVfdC2;break;
    case 4: m_operationMode[1].controlMethod = allVfdHzC1;break;
    case 5: m_operationMode[1].controlMethod = allVfdHzC2;break;
    case 6: m_operationMode[1].controlMethod = allVfdEffi;break;
    }

    if(ui->lineEdit_StageHz->text().toDouble()>=0 && ui->lineEdit_StageHz->text().toDouble()<=50)
        m_operationMode[1].stagingHz = ui->lineEdit_StageHz->text().toDouble();
    else {
        QMessageBox::warning(this,"Warning","please check staging Hz of operation mode 2!");
        return false;
    }

    if(ui->lineEdit_DestageHz->text().toDouble()>=0 && ui->lineEdit_DestageHz->text().toDouble()<=50)
        m_operationMode[1].destagingHz = ui->lineEdit_DestageHz->text().toDouble();
    else {
        QMessageBox::warning(this,"Warning","please check destaging Hz of operation mode 2!");
        return false;
    }

    m_operationMode[1].plotPowerCurve = ui->checkBox_power_curve->isChecked();
    m_operationMode[1].plotEfficiencyCurve = ui->checkBox_effi_curve->isChecked();
    m_operationMode[1].plotVfdCurves = ui->checkBox_vfd_curve->isChecked();

    return true;
}

bool MainWindow::calcuSynchroSpeedPerform(bool isVfd, int pumps,double flow, double head, pumpActualPerform *perform)
{
    if(pumps <1) return 0;

    //get head
    bool   eoc;
    double head_calcu = m_pump.GetHeadFromFlow(50,flow / pumps,&eoc);

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
    double Hz_calcu;

    if(isVfd)
        Hz_calcu = m_pump.CalcuHz(flow / pumps,head);
    else
        Hz_calcu = 50;

    //recheck eoc
    if(m_pump.ReadPerformance().flow_max * Hz_calcu/50 < flow / pumps)
    {
        perform->infor = 1;
        return 0;
    }

    //calculate axis power
    double axis_power_calcu = m_pump.GetPowerFromFlow(Hz_calcu,flow / pumps);

    //calculate input power
    if(axis_power_calcu > m_pump.ReadMotorData().power_rate)
    {
        perform->infor = 3;
        return 0;
    }

    double input_power;

    double effi_motor = m_pump.GetMotorEffi(axis_power_calcu,Hz_calcu / 50);

    if(effi_motor > 0)
        input_power = axis_power_calcu / effi_motor * 100;
    else
    {
        perform->infor = 4;
        return 0;
    }

    //calculate efficiency
    Q_ASSERT(pumps>0);
    Q_ASSERT(axis_power_calcu>0);
    Q_ASSERT(input_power>0);

    double effi_pump = (isVfd ? head:head_calcu) * flow / pumps * 9.8 / axis_power_calcu / 36;
    double effi_total = (isVfd ? head:head_calcu) * flow / pumps * 9.8 / input_power / 36;
    if(effi_total < 0 || effi_total > 100)
    {
        perform->infor = 5;
        return 0;
    }

    //calculate specific energy
    double energy_specific;
    if(flow > 0)
        energy_specific = input_power * pumps / flow;
    else
        energy_specific = 0;

    //return value
    if(perform)
    {
        perform->pumps = pumps;
        perform->flow_total = flow;
        perform->flow_fix = 0;
        perform->flow_vfd = flow;
        perform->head = isVfd ? head:head_calcu;
        perform->Hz_fix = 50;
        perform->Hz_vfd = Hz_calcu;
        perform->power_axis_total = axis_power_calcu * pumps;
        perform->power_axis_fix = axis_power_calcu;
        perform->power_axis_vfd = axis_power_calcu;
        perform->power_input_total = input_power * pumps;
        perform->power_input_fix = input_power;
        perform->power_input_vfd = input_power;
        perform->effi_pump_fix = effi_pump;
        perform->effi_pump_vfd = effi_pump;
        perform->effi_motor_fix = effi_motor;
        perform->effi_motor_vfd = effi_motor;
        perform->effi_total = effi_motor * effi_pump / 100;
        perform->energy_specific = energy_specific;
    }
    else
    {
        perform->infor = 6;
        return 0;
    }

    return 1;
}

bool MainWindow::calcuOneVfdPerform(int pumps,double flow, double head, pumpActualPerform *perform)
{
    if(pumps <1) return 0;

    if(pumps ==1)
    {
        //get head
        bool   eoc;
        double head_calcu = m_pump.GetHeadFromFlow(50,flow,&eoc);

        //end of curve
        if(eoc)
        {
            perform->infor = 1;
            return 0;
        }

        //above curve
        if(head > head_calcu)
        {
            perform->infor = 2;
            return 0;
        }

        //calculate speed
        double Hz_calcu = m_pump.CalcuHz(flow,head);

        //recheck eoc
        if(m_pump.ReadPerformance().flow_max * Hz_calcu/50 < flow)
        {
            perform->infor = 1;
            return 0;
        }

        //calculate axis power
        double axis_power_calcu = m_pump.GetPowerFromFlow(Hz_calcu,flow);

        //calculate input power
        if(axis_power_calcu > m_pump.ReadMotorData().power_rate)
        {
            perform->infor = 3;
            return 0;
        }

        double input_power;
        double effi_motor = m_pump.GetMotorEffi(axis_power_calcu,Hz_calcu/50);

        if(effi_motor > 0)
            input_power = axis_power_calcu / effi_motor * 100;
        else
        {
            perform->infor = 4;
            return 0;
        }

        //calculate efficiency
        Q_ASSERT(pumps>0);
        Q_ASSERT(axis_power_calcu>0);
        Q_ASSERT(input_power>0);

        double effi_pump = head * flow * 9.8 / axis_power_calcu / 36;

        if(effi_pump < 0 || effi_pump > 100)
        {
            perform->infor = 5;
            return 0;
        }

        //return value
        if(perform)
        {
            perform->pumps = 1;
            perform->flow_total = flow;
            perform->flow_fix = 0;
            perform->flow_vfd = flow;
            perform->head = head;
            perform->Hz_fix = 50;
            perform->Hz_vfd = Hz_calcu;
            perform->power_axis_total = axis_power_calcu;
            perform->power_axis_fix = 0;
            perform->power_axis_vfd = axis_power_calcu;
            perform->power_input_total = input_power;
            perform->power_input_fix = 0;
            perform->power_input_vfd = input_power;
            perform->effi_pump_fix = 0;
            perform->effi_pump_vfd = effi_pump;
            perform->effi_motor_fix = 0;
            perform->effi_motor_vfd = effi_motor;
            perform->effi_total = effi_motor * effi_pump / 100;
        }
        else
        {
            perform->infor = 6;
            return 0;
        }
        return 1;
    }
    else
    {
        //get flow
        bool   eoc;
        double non_vfd_flow = m_pump.GetFlowFromHead(50,head,&eoc);

        if(non_vfd_flow > flow / (pumps-1))
            non_vfd_flow = flow / (pumps-1);

        //end of curve
        if(eoc)
        {
            perform->infor = 1;
            return 0;
        }

        //above curve
        double max_head = m_pump.GetHeadFromFlow(50,flow / pumps,&eoc);
        if(head > max_head)
        {
            perform->infor = 2;
            return 0;
        }

        //calcu head
        double head_calcu = m_pump.GetHeadFromFlow(50,non_vfd_flow,&eoc);

        //calculate speed
        double vfd_flow = flow - non_vfd_flow * (pumps-1);
        double Hz_calcu;
        double axis_power_calcu_1;
        double axis_power_calcu_2;

        if(vfd_flow <= 0)
            Hz_calcu = m_pump.CalcuHz(0,head);
        else
            Hz_calcu = m_pump.CalcuHz(vfd_flow,head);

        //recheck eoc
        if(m_pump.ReadPerformance().flow_max * Hz_calcu/50 < vfd_flow)
        {
            perform->infor = 1;
            return 0;
        }

        //calculate axis power
        axis_power_calcu_1 = m_pump.GetPowerFromFlow(50,non_vfd_flow);
        axis_power_calcu_2 = m_pump.GetPowerFromFlow(Hz_calcu,vfd_flow);

        //calculate input power
        if(axis_power_calcu_1 > m_pump.ReadMotorData().power_rate)
        {
            perform->infor = 3;
            return 0;
        }

        double motor_effi_1 = m_pump.GetMotorEffi(axis_power_calcu_1,50);
        double motor_effi_2 = m_pump.GetMotorEffi(axis_power_calcu_2,Hz_calcu);

        double input_power_1;
        double input_power_2;

        if(motor_effi_1 > 0 && motor_effi_2 > 0)
        {
            input_power_1 = axis_power_calcu_1 / motor_effi_1 * 100;
            input_power_2 = axis_power_calcu_2 / motor_effi_2 * 100;
        }
        else
        {
            perform->infor = 4;
            return 0;
        }

        //calculate efficiency
        Q_ASSERT(pumps>1);
        Q_ASSERT(input_power_1>0);
        Q_ASSERT(input_power_2>0);
        Q_ASSERT(axis_power_calcu_1>0);
        Q_ASSERT(axis_power_calcu_2>0);

        double effi_pump_fix = head_calcu * non_vfd_flow * 9.8 / axis_power_calcu_1 * (pumps-1) / 36;
        double effi_pump_vfd = head * vfd_flow * 9.8 / axis_power_calcu_2 / 36;
        double effi_total = head * (non_vfd_flow * (pumps-1) + vfd_flow) * 9.8 / (axis_power_calcu_1 * (pumps-1) + axis_power_calcu_2) / 36;

        if(effi_total < 0 || effi_total > 100)
        {
            perform->infor = 5;
            return 0;
        }

        //return value
        if(perform)
        {
            perform->pumps = pumps;
            perform->flow_total = flow;
            perform->flow_fix = non_vfd_flow;
            perform->flow_vfd = vfd_flow;
            perform->head = head_calcu;
            perform->Hz_fix = 50;
            perform->Hz_vfd = Hz_calcu;
            perform->power_axis_total = axis_power_calcu_1 * (pumps - 1) + axis_power_calcu_2;
            perform->power_axis_fix = axis_power_calcu_1;
            perform->power_axis_vfd = axis_power_calcu_2;
            perform->power_input_total = input_power_1 * (pumps - 1) + input_power_2;
            perform->power_input_fix = input_power_1;
            perform->power_input_vfd = input_power_2;
            perform->effi_pump_fix = effi_pump_fix;
            perform->effi_pump_vfd = effi_pump_vfd;
            perform->effi_motor_fix = motor_effi_1;
            perform->effi_motor_vfd = motor_effi_2;
            perform->effi_total = effi_total;

        }
        else
        {
            perform->infor = 6;
            return 0;
        }
        return 1;

    }
}

bool MainWindow::generatePerformList(pumpActualPerform (*perform)[10][101], operationMode operationMode)
{
    double              flow_step = m_sysPara.designTotalFlow / 100;
    double              current_flow = 0;
    double              required_head;

    for(int i=0;i<101;i++)
    {
        required_head = m_pumpSys.GetControlCurve(current_flow);

        for(int j=0;j<m_pumpSys.ReadSysPara().dutyPumps;j++)
        {
            (*perform)[j][i] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

            bool isVfd;
            if(operationMode.controlMethod==fullSpeed)
                isVfd = false;
            else
                isVfd = true;
            bool result;
            if(operationMode.controlMethod==oneVfdC1 || operationMode.controlMethod==oneVfdC2)
                result = calcuOneVfdPerform(j+1,current_flow, required_head, &(*perform)[j][i]);
            else
                result = calcuSynchroSpeedPerform(isVfd, j+1,current_flow, required_head, &(*perform)[j][i]);

            if(j==(m_pumpSys.ReadSysPara().dutyPumps-1)
                    &&!result
                    &&((*perform)[j][i].infor==1||(*perform)[j][i].infor==2||(*perform)[j][i].infor==3))
            {
                QString infor1 = "Out of capacity of the desinated pump or motor!\n\n";
                    infor1 += "system flow: " + QString::number(current_flow) + "\n";
                    infor1 += "system required head: " + QString::number(required_head);
                    QMessageBox::warning(nullptr,"Warning",infor1);
                    return false;
            }
        }
        current_flow += flow_step;
    }
    return true;
}

bool MainWindow::generatePointsOnCurve_fullSpeed(pointsOnCurve *pointsOnCurve, operationMode &operationMode)
{
    pumpActualPerform performList[10][101];

    if(!generatePerformList(&performList,operationMode))
        return false;

    pointsOnCurve->pumps[0]=1;
    pointsOnCurve->flow_total[0]=0;
    pointsOnCurve->flow_fix[0]=0;
    pointsOnCurve->flow_vfd[0]=0;
    pointsOnCurve->head[0]=performList[0][0].head;
    pointsOnCurve->Hz_fix[0]=performList[0][0].Hz_fix;
    pointsOnCurve->Hz_vfd[0]=performList[0][0].Hz_vfd;
    pointsOnCurve->power_axis_total[0]=performList[0][0].power_axis_total;
    pointsOnCurve->power_axis_fix[0]=performList[0][0].power_axis_fix;
    pointsOnCurve->power_axis_vfd[0]=performList[0][0].power_axis_vfd;
    pointsOnCurve->power_input_total[0]=performList[0][0].power_input_total;
    pointsOnCurve->power_input_fix[0]=performList[0][0].power_input_fix;
    pointsOnCurve->power_input_vfd[0]=performList[0][0].power_input_vfd;
    pointsOnCurve->effi_pump_fix[0]=0;
    pointsOnCurve->effi_pump_vfd[0]=0;
    pointsOnCurve->effi_motor_fix[0]=performList[0][0].effi_motor_fix;
    pointsOnCurve->effi_motor_vfd[0]=performList[0][0].effi_motor_vfd;
    pointsOnCurve->effi_total[0]=0;

    for(int i=1;i<101;i++)
    {

        pointsOnCurve->pumps[i]=pointsOnCurve->pumps[i-1];

        while((pointsOnCurve->pumps[i] < m_sysPara.dutyPumps)
              && (performList[pointsOnCurve->pumps[i]-1][i].effi_total <= 0.01))
        {
            pointsOnCurve->pumps[i]++;
        }
        pointsOnCurve->flow_total[i]        =	performList[pointsOnCurve->pumps[i]-1][i].	flow_total	;
        pointsOnCurve->flow_fix[i]          =	performList[pointsOnCurve->pumps[i]-1][i].	flow_fix	;
        pointsOnCurve->flow_vfd[i]          =	performList[pointsOnCurve->pumps[i]-1][i].	flow_vfd	;
        pointsOnCurve->head[i]              =	performList[pointsOnCurve->pumps[i]-1][i].	head	;
        pointsOnCurve->Hz_fix[i]            =	performList[pointsOnCurve->pumps[i]-1][i].	Hz_fix	;
        pointsOnCurve->Hz_vfd[i]            =	performList[pointsOnCurve->pumps[i]-1][i].	Hz_vfd	;
        pointsOnCurve->power_axis_total[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	power_axis_total;
        pointsOnCurve->power_axis_fix[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	power_axis_fix	;
        pointsOnCurve->power_axis_vfd[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	power_axis_vfd	;
        pointsOnCurve->power_input_total[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	power_input_total;
        pointsOnCurve->power_input_fix[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	power_input_fix	;
        pointsOnCurve->power_input_vfd[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	power_input_vfd	;
        pointsOnCurve->effi_pump_fix[i]     =	performList[pointsOnCurve->pumps[i]-1][i].	effi_pump_fix	;
        pointsOnCurve->effi_pump_vfd[i]     =	performList[pointsOnCurve->pumps[i]-1][i].	effi_pump_vfd	;
        pointsOnCurve->effi_motor_fix[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	effi_motor_fix	;
        pointsOnCurve->effi_motor_vfd[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	effi_motor_vfd	;
        pointsOnCurve->effi_total[i]        =	performList[pointsOnCurve->pumps[i]-1][i].	effi_total	;

    }

    //check motor size
    for(int i=1;i<101;i++)
    {
        int pumps = pointsOnCurve->pumps[i];
        if(performList[pumps-1][i].power_axis_fix > m_pump.ReadMotorData().power_rate)
        {
            QString infor = "Axis power of the pump is greater than rated power of the motor!\n\n";
            infor += "system flow: " + QString::number(performList[pumps-1][i].flow_total) + "\n";
            infor += "pump flow: " + QString::number(performList[pumps-1][i].flow_fix / pumps) + "\n";
            infor += "pump head: " + QString::number(performList[pumps-1][i].head) + "\n";
            infor += "axis power: " + QString::number(performList[pumps-1][i].power_axis_fix);
            QMessageBox::warning(nullptr,"Warning",infor);
            return false;
        }
    }
    return true;
}

bool MainWindow::generatePointsOnCurve_oneVfdC1(pointsOnCurve *pointsOnCurve, operationMode &operationMode)
{
    pumpActualPerform performList[10][101];

    if(!generatePerformList(&performList,operationMode))
        return false;

    //flow rising
    pointsOnCurve->pumps[0]=1;
    pointsOnCurve->flow_total[0]=0;
    pointsOnCurve->flow_fix[0]=0;
    pointsOnCurve->flow_vfd[0]=0;
    pointsOnCurve->head[0]=performList[0][0].head;
    pointsOnCurve->Hz_fix[0]=performList[0][0].Hz_fix;
    pointsOnCurve->Hz_vfd[0]=performList[0][0].Hz_vfd;
    pointsOnCurve->power_axis_total[0]=performList[0][0].power_axis_total;
    pointsOnCurve->power_axis_fix[0]=performList[0][0].power_axis_fix;
    pointsOnCurve->power_axis_vfd[0]=performList[0][0].power_axis_vfd;
    pointsOnCurve->power_input_total[0]=performList[0][0].power_input_total;
    pointsOnCurve->power_input_fix[0]=performList[0][0].power_input_fix;
    pointsOnCurve->power_input_vfd[0]=performList[0][0].power_input_vfd;
    pointsOnCurve->effi_pump_fix[0]=0;
    pointsOnCurve->effi_pump_vfd[0]=0;
    pointsOnCurve->effi_motor_fix[0]=performList[0][0].effi_motor_fix;
    pointsOnCurve->effi_motor_vfd[0]=performList[0][0].effi_motor_vfd;
    pointsOnCurve->effi_total[0]=0;

    for(int i=1;i<101;i++)
    {

        pointsOnCurve->pumps[i] = pointsOnCurve->pumps[i-1];

        while(pointsOnCurve->pumps[i] < m_sysPara.dutyPumps
              && (performList[pointsOnCurve->pumps[i]-1][i].effi_total <= 0.01
                    || performList[pointsOnCurve->pumps[i]-1][i].Hz_vfd >= (operationMode.stagingHz-0.01)))
        {
            pointsOnCurve->pumps[i]++;
        }
        pointsOnCurve->flow_total[i]        =	performList[pointsOnCurve->pumps[i]-1][i].	flow_total	;
        pointsOnCurve->flow_fix[i]          =	performList[pointsOnCurve->pumps[i]-1][i].	flow_fix	;
        pointsOnCurve->flow_vfd[i]          =	performList[pointsOnCurve->pumps[i]-1][i].	flow_vfd	;
        pointsOnCurve->head[i]              =	performList[pointsOnCurve->pumps[i]-1][i].	head	;
        pointsOnCurve->Hz_fix[i]            =	performList[pointsOnCurve->pumps[i]-1][i].	Hz_fix	;
        pointsOnCurve->Hz_vfd[i]            =	performList[pointsOnCurve->pumps[i]-1][i].	Hz_vfd	;
        pointsOnCurve->power_axis_total[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	power_axis_total;
        pointsOnCurve->power_axis_fix[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	power_axis_fix	;
        pointsOnCurve->power_axis_vfd[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	power_axis_vfd	;
        pointsOnCurve->power_input_total[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	power_input_total;
        pointsOnCurve->power_input_fix[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	power_input_fix	;
        pointsOnCurve->power_input_vfd[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	power_input_vfd	;
        pointsOnCurve->effi_pump_fix[i]     =	performList[pointsOnCurve->pumps[i]-1][i].	effi_pump_fix	;
        pointsOnCurve->effi_pump_vfd[i]     =	performList[pointsOnCurve->pumps[i]-1][i].	effi_pump_vfd	;
        pointsOnCurve->effi_motor_fix[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	effi_motor_fix	;
        pointsOnCurve->effi_motor_vfd[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	effi_motor_vfd	;
        pointsOnCurve->effi_total[i]        =	performList[pointsOnCurve->pumps[i]-1][i].	effi_total	;
    }

    //check motor size
    for(int i=1;i<101;i++)
    {
        int pumps = pointsOnCurve->pumps[i];
        bool bigger = performList[pumps-1][i].power_axis_fix > performList[pumps-1][i].power_axis_vfd;

        if((bigger ? performList[pumps-1][i].power_axis_fix: performList[pumps-1][i].power_axis_vfd) > m_pump.ReadMotorData().power_rate)
        {
            QString infor = "Axis power of the pump is greater than rated power of the motor!\n\n";
            infor += "system flow: " + QString::number(performList[pumps-1][i].flow_total) + "\n";

            double bigger_flow;
            if(bigger && pumps > 1)
               bigger_flow = performList[pumps-1][i].flow_fix / (pumps-1);
            else
               bigger_flow = performList[pumps-1][i].flow_vfd;

            infor += "pump flow: " + QString::number(bigger_flow) + "\n";
            infor += "pump head: " + QString::number(performList[pumps-1][i].head) + "\n";
            infor += "axis power: " + QString::number(bigger ? performList[pumps-1][i].power_axis_fix: performList[pumps-1][i].power_axis_vfd);
            QMessageBox::warning(nullptr,"Warning",infor);
            return false;
        }
    }
    return true;
}

bool MainWindow::generatePointsOnCurve_oneVfdC2(pointsOnCurve *pointsOnCurve, operationMode &operationMode)
{
    pumpActualPerform performList[10][101];

    if(!generatePerformList(&performList,operationMode))
        return false;

    //flow falling
    int qty = 1;
    while(qty < m_sysPara.dutyPumps
          && (performList[qty-1][100].Hz_vfd >= (operationMode.stagingHz-0.01)
          || performList[qty-1][100].effi_total <= 0.01))
    {
        qty++;
    }
    pointsOnCurve->pumps[100]             =	performList[qty-1][100].	pumps	;
    pointsOnCurve->flow_total[100]        =	performList[qty-1][100].	flow_total	;
    pointsOnCurve->flow_fix[100]          =	performList[qty-1][100].	flow_fix	;
    pointsOnCurve->flow_vfd[100]          =	performList[qty-1][100].	flow_vfd	;
    pointsOnCurve->head[100]              =	performList[qty-1][100].	head	;
    pointsOnCurve->Hz_fix[100]            =	performList[qty-1][100].	Hz_fix	;
    pointsOnCurve->Hz_vfd[100]            =	performList[qty-1][100].	Hz_vfd	;
    pointsOnCurve->power_axis_total[100]  =	performList[qty-1][100].	power_axis_total;
    pointsOnCurve->power_axis_fix[100]    =	performList[qty-1][100].	power_axis_fix	;
    pointsOnCurve->power_axis_vfd[100]    =	performList[qty-1][100].	power_axis_vfd	;
    pointsOnCurve->power_input_total[100] =	performList[qty-1][100].	power_input_total;
    pointsOnCurve->power_input_fix[100]   =	performList[qty-1][100].	power_input_fix	;
    pointsOnCurve->power_input_vfd[100]	  =	performList[qty-1][100].	power_input_vfd	;
    pointsOnCurve->effi_pump_fix[100]     =	performList[qty-1][100].	effi_pump_fix	;
    pointsOnCurve->effi_pump_vfd[100]     =	performList[qty-1][100].	effi_pump_vfd	;
    pointsOnCurve->effi_motor_fix[100]	  =	performList[qty-1][100].	effi_motor_fix	;
    pointsOnCurve->effi_motor_vfd[100]	  =	performList[qty-1][100].	effi_motor_vfd	;
    pointsOnCurve->effi_total[100]        =	performList[qty-1][100].	effi_total	;

    for(int i=99;i>0;i--)
    {
        pointsOnCurve->pumps[i] = pointsOnCurve->pumps[i+1];

        while(pointsOnCurve->pumps[i] > 1
              && performList[pointsOnCurve->pumps[i]-2][i].effi_total > 0.01
              && performList[pointsOnCurve->pumps[i]-1][i].Hz_vfd <=operationMode.destagingHz
              && performList[pointsOnCurve->pumps[i]-2][i].Hz_vfd <operationMode.stagingHz)
        {
            pointsOnCurve->pumps[i]--;
        }
        pointsOnCurve->flow_total[i]        =	performList[pointsOnCurve->pumps[i]-1][i].	flow_total	;
        pointsOnCurve->flow_fix[i]          =	performList[pointsOnCurve->pumps[i]-1][i].	flow_fix	;
        pointsOnCurve->flow_vfd[i]          =	performList[pointsOnCurve->pumps[i]-1][i].	flow_vfd	;
        pointsOnCurve->head[i]              =	performList[pointsOnCurve->pumps[i]-1][i].	head	;
        pointsOnCurve->Hz_fix[i]            =	performList[pointsOnCurve->pumps[i]-1][i].	Hz_fix	;
        pointsOnCurve->Hz_vfd[i]            =	performList[pointsOnCurve->pumps[i]-1][i].	Hz_vfd	;
        pointsOnCurve->power_axis_total[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	power_axis_total;
        pointsOnCurve->power_axis_fix[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	power_axis_fix	;
        pointsOnCurve->power_axis_vfd[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	power_axis_vfd	;
        pointsOnCurve->power_input_total[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	power_input_total;
        pointsOnCurve->power_input_fix[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	power_input_fix	;
        pointsOnCurve->power_input_vfd[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	power_input_vfd	;
        pointsOnCurve->effi_pump_fix[i]     =	performList[pointsOnCurve->pumps[i]-1][i].	effi_pump_fix	;
        pointsOnCurve->effi_pump_vfd[i]     =	performList[pointsOnCurve->pumps[i]-1][i].	effi_pump_vfd	;
        pointsOnCurve->effi_motor_fix[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	effi_motor_fix	;
        pointsOnCurve->effi_motor_vfd[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	effi_motor_vfd	;
        pointsOnCurve->effi_total[i]        =	performList[pointsOnCurve->pumps[i]-1][i].	effi_total	;
    }

    pointsOnCurve->pumps[0]             	=	    pointsOnCurve->pumps[1]             	;
    pointsOnCurve->flow_total[0]        	=	    pointsOnCurve->flow_total[1]        	;
    pointsOnCurve->flow_fix[0]          	=	    pointsOnCurve->flow_fix[1]          	;
    pointsOnCurve->flow_vfd[0]          	=	    pointsOnCurve->flow_vfd[1]          	;
    pointsOnCurve->head[0]              	=	    pointsOnCurve->head[1]              	;
    pointsOnCurve->Hz_fix[0]            	=	    pointsOnCurve->Hz_fix[1]            	;
    pointsOnCurve->Hz_vfd[0]            	=	    pointsOnCurve->Hz_vfd[1]            	;
    pointsOnCurve->power_axis_total[0]  	=	    pointsOnCurve->power_axis_total[1]  	;
    pointsOnCurve->power_axis_fix[0]    	=	    pointsOnCurve->power_axis_fix[1]    	;
    pointsOnCurve->power_axis_vfd[0]    	=	    pointsOnCurve->power_axis_vfd[1]    	;
    pointsOnCurve->power_input_total[0] 	=	    pointsOnCurve->power_input_total[1] 	;
    pointsOnCurve->power_input_fix[0]   	=	    pointsOnCurve->power_input_fix[1]   	;
    pointsOnCurve->power_input_vfd[0]   	=	    pointsOnCurve->power_input_vfd[1]   	;
    pointsOnCurve->effi_pump_fix[0]     	=	    pointsOnCurve->effi_pump_fix[1]     	;
    pointsOnCurve->effi_pump_vfd[0]     	=	    pointsOnCurve->effi_pump_vfd[1]     	;
    pointsOnCurve->effi_motor_fix[0]    	=	    pointsOnCurve->effi_motor_fix[1]    	;
    pointsOnCurve->effi_motor_vfd[0]    	=	    pointsOnCurve->effi_motor_vfd[1]    	;
    pointsOnCurve->effi_total[0]        	=	    pointsOnCurve->effi_total[1]        	;

    //check motor size
    for(int i=1;i<101;i++)
    {
        int pumps = pointsOnCurve->pumps[i];
        bool bigger = performList[pumps-1][i].power_axis_fix > performList[pumps-1][i].power_axis_vfd;

        if((bigger ? performList[pumps-1][i].power_axis_fix: performList[pumps-1][i].power_axis_vfd) > m_pump.ReadMotorData().power_rate)
        {
            QString infor = "Axis power of the pump is greater than rated power of the motor!\n\n";
            infor += "system flow: " + QString::number(performList[pumps-1][i].flow_total) + "\n";

            double bigger_flow;
            if(bigger && pumps > 1)
               bigger_flow = performList[pumps-1][i].flow_fix / (pumps-1);
            else
               bigger_flow = performList[pumps-1][i].flow_vfd;

            infor += "pump flow: " + QString::number(bigger_flow) + "\n";
            infor += "pump head: " + QString::number(performList[pumps-1][i].head) + "\n";
            infor += "axis power: " + QString::number(bigger ? performList[pumps-1][i].power_axis_fix: performList[pumps-1][i].power_axis_vfd);
            QMessageBox::warning(nullptr,"Warning",infor);
            return false;
        }
    }
    return true;
}

bool MainWindow::generatePointsOnCurve_allVfdHzC1(pointsOnCurve *pointsOnCurve, operationMode &operationMode)
{
    pumpActualPerform performList[10][101];

    if(!generatePerformList(&performList,operationMode))
        return false;

    //flow rising
    pointsOnCurve->pumps[0]=1;
    pointsOnCurve->flow_total[0]=0;
    pointsOnCurve->flow_fix[0]=0;
    pointsOnCurve->flow_vfd[0]=0;
    pointsOnCurve->head[0]=performList[0][0].head;
    pointsOnCurve->Hz_fix[0]=performList[0][0].Hz_fix;
    pointsOnCurve->Hz_vfd[0]=performList[0][0].Hz_vfd;
    pointsOnCurve->power_axis_total[0]=performList[0][0].power_axis_total;
    pointsOnCurve->power_axis_fix[0]=performList[0][0].power_axis_fix;
    pointsOnCurve->power_axis_vfd[0]=performList[0][0].power_axis_vfd;
    pointsOnCurve->power_input_total[0]=performList[0][0].power_input_total;
    pointsOnCurve->power_input_fix[0]=performList[0][0].power_input_fix;
    pointsOnCurve->power_input_vfd[0]=performList[0][0].power_input_vfd;
    pointsOnCurve->effi_pump_fix[0]=0;
    pointsOnCurve->effi_pump_vfd[0]=0;
    pointsOnCurve->effi_motor_fix[0]=performList[0][0].effi_motor_fix;
    pointsOnCurve->effi_motor_vfd[0]=performList[0][0].effi_motor_vfd;
    pointsOnCurve->effi_total[0]=0;

    for(int i=1;i<101;i++)
    {

        pointsOnCurve->pumps[i] = pointsOnCurve->pumps[i-1];

        while(pointsOnCurve->pumps[i] < m_sysPara.dutyPumps
              && (performList[pointsOnCurve->pumps[i]-1][i].effi_total < 0.01
                    || performList[pointsOnCurve->pumps[i]-1][i].Hz_vfd > (operationMode.stagingHz - 0.01)))
        {
            pointsOnCurve->pumps[i]++;
        }
        pointsOnCurve->flow_total[i]        =	performList[pointsOnCurve->pumps[i]-1][i].	flow_total	;
        pointsOnCurve->flow_fix[i]          =	performList[pointsOnCurve->pumps[i]-1][i].	flow_fix	;
        pointsOnCurve->flow_vfd[i]          =	performList[pointsOnCurve->pumps[i]-1][i].	flow_vfd	;
        pointsOnCurve->head[i]              =	performList[pointsOnCurve->pumps[i]-1][i].	head	;
        pointsOnCurve->Hz_fix[i]            =	performList[pointsOnCurve->pumps[i]-1][i].	Hz_fix	;
        pointsOnCurve->Hz_vfd[i]            =	performList[pointsOnCurve->pumps[i]-1][i].	Hz_vfd	;
        pointsOnCurve->power_axis_total[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	power_axis_total;
        pointsOnCurve->power_axis_fix[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	power_axis_fix	;
        pointsOnCurve->power_axis_vfd[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	power_axis_vfd	;
        pointsOnCurve->power_input_total[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	power_input_total;
        pointsOnCurve->power_input_fix[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	power_input_fix	;
        pointsOnCurve->power_input_vfd[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	power_input_vfd	;
        pointsOnCurve->effi_pump_fix[i]     =	performList[pointsOnCurve->pumps[i]-1][i].	effi_pump_fix	;
        pointsOnCurve->effi_pump_vfd[i]     =	performList[pointsOnCurve->pumps[i]-1][i].	effi_pump_vfd	;
        pointsOnCurve->effi_motor_fix[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	effi_motor_fix	;
        pointsOnCurve->effi_motor_vfd[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	effi_motor_vfd	;
        pointsOnCurve->effi_total[i]        =	performList[pointsOnCurve->pumps[i]-1][i].	effi_total	;
    }

    //check motor size
    for(int i=1;i<101;i++)
    {
        int pumps = pointsOnCurve->pumps[i];
        if(performList[pumps-1][i].power_axis_vfd > m_pump.ReadMotorData().power_rate)
        {
            QString infor = "Axis power of the pump is greater than rated power of the motor!\n\n";
            infor += "system flow: " + QString::number(performList[pumps-1][i].flow_total) + "\n";
            infor += "pump flow: " + QString::number(performList[pumps-1][i].flow_vfd / pumps) + "\n";
            infor += "pump head: " + QString::number(performList[pumps-1][i].head) + "\n";
            infor += "axis power: " + QString::number(performList[pumps-1][i].power_axis_vfd);
            QMessageBox::warning(nullptr,"Warning",infor);
            return false;
        }
    }

    return true;
}

bool MainWindow::generatePointsOnCurve_allVfdHzC2(pointsOnCurve *pointsOnCurve, operationMode &operationMode)
{
    pumpActualPerform performList[10][101];

    if(!generatePerformList(&performList,operationMode))
        return false;

    //flow falling
    int qty = 1;
    while(qty < m_sysPara.dutyPumps
          && (performList[qty-1][100].Hz_vfd >= (operationMode.stagingHz-0.01)
          || performList[qty-1][100].effi_total <= 0.01))
    {
        qty++;
    }
    pointsOnCurve->pumps[100]             =	performList[qty-1][100].	pumps	;
    pointsOnCurve->flow_total[100]        =	performList[qty-1][100].	flow_total	;
    pointsOnCurve->flow_fix[100]          =	performList[qty-1][100].	flow_fix	;
    pointsOnCurve->flow_vfd[100]          =	performList[qty-1][100].	flow_vfd	;
    pointsOnCurve->head[100]              =	performList[qty-1][100].	head	;
    pointsOnCurve->Hz_fix[100]            =	performList[qty-1][100].	Hz_fix	;
    pointsOnCurve->Hz_vfd[100]            =	performList[qty-1][100].	Hz_vfd	;
    pointsOnCurve->power_axis_total[100]  =	performList[qty-1][100].	power_axis_total;
    pointsOnCurve->power_axis_fix[100]    =	performList[qty-1][100].	power_axis_fix	;
    pointsOnCurve->power_axis_vfd[100]    =	performList[qty-1][100].	power_axis_vfd	;
    pointsOnCurve->power_input_total[100] =	performList[qty-1][100].	power_input_total;
    pointsOnCurve->power_input_fix[100]   =	performList[qty-1][100].	power_input_fix	;
    pointsOnCurve->power_input_vfd[100]	  =	performList[qty-1][100].	power_input_vfd	;
    pointsOnCurve->effi_pump_fix[100]     =	performList[qty-1][100].	effi_pump_fix	;
    pointsOnCurve->effi_pump_vfd[100]     =	performList[qty-1][100].	effi_pump_vfd	;
    pointsOnCurve->effi_motor_fix[100]	  =	performList[qty-1][100].	effi_motor_fix	;
    pointsOnCurve->effi_motor_vfd[100]	  =	performList[qty-1][100].	effi_motor_vfd	;
    pointsOnCurve->effi_total[100]        =	performList[qty-1][100].	effi_total	;

    for(int i=99;i>0;i--)
    {

        pointsOnCurve->pumps[i] = pointsOnCurve->pumps[i+1];

        while(pointsOnCurve->pumps[i] > 1
              && performList[pointsOnCurve->pumps[i]-2][i].effi_total > 0.01
              && performList[pointsOnCurve->pumps[i]-1][i].Hz_vfd < (operationMode.destagingHz + 0.01)
              && performList[pointsOnCurve->pumps[i]-2][i].Hz_vfd < (operationMode.stagingHz + 0.01))
        {
            pointsOnCurve->pumps[i]--;
        }
        pointsOnCurve->flow_total[i]        =	performList[pointsOnCurve->pumps[i]-1][i].	flow_total	;
        pointsOnCurve->flow_fix[i]          =	performList[pointsOnCurve->pumps[i]-1][i].	flow_fix	;
        pointsOnCurve->flow_vfd[i]          =	performList[pointsOnCurve->pumps[i]-1][i].	flow_vfd	;
        pointsOnCurve->head[i]              =	performList[pointsOnCurve->pumps[i]-1][i].	head	;
        pointsOnCurve->Hz_fix[i]            =	performList[pointsOnCurve->pumps[i]-1][i].	Hz_fix	;
        pointsOnCurve->Hz_vfd[i]            =	performList[pointsOnCurve->pumps[i]-1][i].	Hz_vfd	;
        pointsOnCurve->power_axis_total[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	power_axis_total;
        pointsOnCurve->power_axis_fix[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	power_axis_fix	;
        pointsOnCurve->power_axis_vfd[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	power_axis_vfd	;
        pointsOnCurve->power_input_total[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	power_input_total;
        pointsOnCurve->power_input_fix[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	power_input_fix	;
        pointsOnCurve->power_input_vfd[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	power_input_vfd	;
        pointsOnCurve->effi_pump_fix[i]     =	performList[pointsOnCurve->pumps[i]-1][i].	effi_pump_fix	;
        pointsOnCurve->effi_pump_vfd[i]     =	performList[pointsOnCurve->pumps[i]-1][i].	effi_pump_vfd	;
        pointsOnCurve->effi_motor_fix[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	effi_motor_fix	;
        pointsOnCurve->effi_motor_vfd[i]	=	performList[pointsOnCurve->pumps[i]-1][i].	effi_motor_vfd	;
        pointsOnCurve->effi_total[i]        =	performList[pointsOnCurve->pumps[i]-1][i].	effi_total	;
    }
    pointsOnCurve->pumps[0]             	=	    pointsOnCurve->pumps[1]             	;
    pointsOnCurve->flow_total[0]        	=	    pointsOnCurve->flow_total[1]        	;
    pointsOnCurve->flow_fix[0]          	=	    pointsOnCurve->flow_fix[1]          	;
    pointsOnCurve->flow_vfd[0]          	=	    pointsOnCurve->flow_vfd[1]          	;
    pointsOnCurve->head[0]              	=	    pointsOnCurve->head[1]              	;
    pointsOnCurve->Hz_fix[0]            	=	    pointsOnCurve->Hz_fix[1]            	;
    pointsOnCurve->Hz_vfd[0]            	=	    pointsOnCurve->Hz_vfd[1]            	;
    pointsOnCurve->power_axis_total[0]  	=	    pointsOnCurve->power_axis_total[1]  	;
    pointsOnCurve->power_axis_fix[0]    	=	    pointsOnCurve->power_axis_fix[1]    	;
    pointsOnCurve->power_axis_vfd[0]    	=	    pointsOnCurve->power_axis_vfd[1]    	;
    pointsOnCurve->power_input_total[0] 	=	    pointsOnCurve->power_input_total[1] 	;
    pointsOnCurve->power_input_fix[0]   	=	    pointsOnCurve->power_input_fix[1]   	;
    pointsOnCurve->power_input_vfd[0]   	=	    pointsOnCurve->power_input_vfd[1]   	;
    pointsOnCurve->effi_pump_fix[0]     	=	    pointsOnCurve->effi_pump_fix[1]     	;
    pointsOnCurve->effi_pump_vfd[0]     	=	    pointsOnCurve->effi_pump_vfd[1]     	;
    pointsOnCurve->effi_motor_fix[0]    	=	    pointsOnCurve->effi_motor_fix[1]    	;
    pointsOnCurve->effi_motor_vfd[0]    	=	    pointsOnCurve->effi_motor_vfd[1]    	;
    pointsOnCurve->effi_total[0]        	=	    pointsOnCurve->effi_total[1]        	;

    //check motor size
    for(int i=1;i<101;i++)
    {
        int pumps = pointsOnCurve->pumps[i];
        if(performList[pumps-1][i].power_axis_vfd > m_pump.ReadMotorData().power_rate)
        {
            QString infor = "Axis power of the pump is greater than rated power of the motor!\n\n";
            infor += "system flow: " + QString::number(performList[pumps-1][i].flow_total) + "\n";
            infor += "pump flow: " + QString::number(performList[pumps-1][i].flow_vfd / pumps) + "\n";
            infor += "pump head: " + QString::number(performList[pumps-1][i].head) + "\n";
            infor += "axis power: " + QString::number(performList[pumps-1][i].power_axis_vfd);
            QMessageBox::warning(nullptr,"Warning",infor);
            return false;
        }
    }
    return true;
}

bool MainWindow::generatePointsOnCurve_allVfdEffi(pointsOnCurve *pointsOnCurve, operationMode &operationMode)
{
    pumpActualPerform performList[10][101];

    if(!generatePerformList(&performList,operationMode))
        return false;

    pointsOnCurve->pumps[0]=1;
    pointsOnCurve->flow_total[0]=0;
    pointsOnCurve->flow_fix[0]=0;
    pointsOnCurve->flow_vfd[0]=0;
    pointsOnCurve->head[0]=performList[0][0].head;
    pointsOnCurve->Hz_fix[0]=performList[0][0].Hz_fix;
    pointsOnCurve->Hz_vfd[0]=performList[0][0].Hz_vfd;
    pointsOnCurve->power_axis_total[0]=performList[0][0].power_axis_total;
    pointsOnCurve->power_axis_fix[0]=performList[0][0].power_axis_fix;
    pointsOnCurve->power_axis_vfd[0]=performList[0][0].power_axis_vfd;
    pointsOnCurve->power_input_total[0]=performList[0][0].power_input_total;
    pointsOnCurve->power_input_fix[0]=performList[0][0].power_input_fix;
    pointsOnCurve->power_input_vfd[0]=performList[0][0].power_input_vfd;
    pointsOnCurve->effi_pump_fix[0]=0;
    pointsOnCurve->effi_pump_vfd[0]=0;
    pointsOnCurve->effi_motor_fix[0]=performList[0][0].effi_motor_fix;
    pointsOnCurve->effi_motor_vfd[0]=performList[0][0].effi_motor_vfd;
    pointsOnCurve->effi_total[0]=0;

    for(int i=1;i<101;i++)
    {
        pointsOnCurve->pumps[i]=0;
        pointsOnCurve->flow_total[i]=0;
        pointsOnCurve->flow_fix[i]=0;
        pointsOnCurve->flow_vfd[i]=0;
        pointsOnCurve->head[i]=0;
        pointsOnCurve->Hz_fix[i]=0;
        pointsOnCurve->Hz_vfd[i]=0;
        pointsOnCurve->power_axis_total[i]=0;
        pointsOnCurve->power_axis_fix[i]=0;
        pointsOnCurve->power_axis_vfd[i]=0;
        pointsOnCurve->power_input_total[i]=0;
        pointsOnCurve->power_input_fix[i]=0;
        pointsOnCurve->power_input_vfd[i]=0;
        pointsOnCurve->effi_pump_fix[i]=0;
        pointsOnCurve->effi_pump_vfd[i]=0;
        pointsOnCurve->effi_motor_fix[i]=0;
        pointsOnCurve->effi_motor_vfd[i]=0;
        pointsOnCurve->effi_total[i]=0;

        for(int j=0;j<m_sysPara.dutyPumps;j++)
        {
            if(performList[j][i].effi_total > pointsOnCurve->effi_total[i])
            {
                pointsOnCurve->pumps[i]             =	performList[j][i].	pumps	;
                pointsOnCurve->flow_total[i]        =	performList[j][i].	flow_total	;
                pointsOnCurve->flow_fix[i]          =	performList[j][i].	flow_fix	;
                pointsOnCurve->flow_vfd[i]          =	performList[j][i].	flow_vfd	;
                pointsOnCurve->head[i]              =	performList[j][i].	head	;
                pointsOnCurve->Hz_fix[i]            =	performList[j][i].	Hz_fix	;
                pointsOnCurve->Hz_vfd[i]            =	performList[j][i].	Hz_vfd	;
                pointsOnCurve->power_axis_total[i]	=	performList[j][i].	power_axis_total;
                pointsOnCurve->power_axis_fix[i]	=	performList[j][i].	power_axis_fix	;
                pointsOnCurve->power_axis_vfd[i]	=	performList[j][i].	power_axis_vfd	;
                pointsOnCurve->power_input_total[i]	=	performList[j][i].	power_input_total;
                pointsOnCurve->power_input_fix[i]	=	performList[j][i].	power_input_fix	;
                pointsOnCurve->power_input_vfd[i]	=	performList[j][i].	power_input_vfd	;
                pointsOnCurve->effi_pump_fix[i]     =	performList[j][i].	effi_pump_fix	;
                pointsOnCurve->effi_pump_vfd[i]     =	performList[j][i].	effi_pump_vfd	;
                pointsOnCurve->effi_motor_fix[i]	=	performList[j][i].	effi_motor_fix	;
                pointsOnCurve->effi_motor_vfd[i]	=	performList[j][i].	effi_motor_vfd	;
                pointsOnCurve->effi_total[i]        =	performList[j][i].	effi_total	;
            }
        }
    }

    //check motor size
    for(int i=1;i<101;i++)
    {
        int pumps = pointsOnCurve->pumps[i];
        if(performList[pumps-1][i].power_axis_vfd > m_pump.ReadMotorData().power_rate)
        {
            QString infor = "Axis power of the pump is greater than rated power of the motor!\n\n";
            infor += "system flow: " + QString::number(performList[pumps-1][i].flow_total) + "\n";
            infor += "pump flow: " + QString::number(performList[pumps-1][i].flow_vfd / pumps) + "\n";
            infor += "pump head: " + QString::number(performList[pumps-1][i].head) + "\n";
            infor += "axis power: " + QString::number(performList[pumps-1][i].power_axis_vfd);
            QMessageBox::warning(nullptr,"Warning",infor);
            return false;
        }
    }
    return true;
}

bool MainWindow::generatePointsOnCurve_specificEnergy(pointsOnCurve (*pointsOnCurve)[10], operationMode &operationMode)
{
    pumpActualPerform performList[10][101];

    if(!generatePerformList(&performList,operationMode))
        return false;

    m_lowestEsAllCurve.Es = 999;

    m_lowestEsCurve[0].pumps             = 1;
    m_lowestEsCurve[0].Es                = 0;
    m_lowestEsCurve[0].flow              = 0;
    m_lowestEsCurve[0].head              = 0;
    m_lowestEsCurve[0].input_power_total = 0;
    m_lowestEsCurve[0].Hz                = 0;

    for(int i=1;i<101;i++)
    {
        double lowestEs = 999;
        int pumpsWithLowestEs = 1;

        for(int j=0;j<m_sysPara.dutyPumps;j++)
        {
            (*pointsOnCurve)[j].flow_total[i] = performList[j][i].flow_total;
            (*pointsOnCurve)[j].head[i] = performList[j][i].head;
            (*pointsOnCurve)[j].energy_specific[i] = performList[j][i].energy_specific;

            if((*pointsOnCurve)[j].energy_specific[i] < lowestEs && (*pointsOnCurve)[j].energy_specific[i] > 0)
            {
                pumpsWithLowestEs = j;
                lowestEs = (*pointsOnCurve)[j].energy_specific[i];
            }
        }
        m_lowestEsCurve[i].pumps                = pumpsWithLowestEs + 1;
        m_lowestEsCurve[i].Es                   = performList[pumpsWithLowestEs][i].energy_specific;
        m_lowestEsCurve[i].flow                 = performList[pumpsWithLowestEs][i].flow_total;
        m_lowestEsCurve[i].head                 = performList[pumpsWithLowestEs][i].head;
        m_lowestEsCurve[i].input_power_total    = performList[pumpsWithLowestEs][i].power_input_total;
        m_lowestEsCurve[i].Hz                   = performList[pumpsWithLowestEs][i].Hz_vfd;

        if(m_lowestEsCurve[i].Es < m_lowestEsAllCurve.Es && m_lowestEsCurve[i].Es > 0)
        {
            m_lowestEsAllCurve.pumps                 = m_lowestEsCurve[i].pumps;
            m_lowestEsAllCurve.Es                    = m_lowestEsCurve[i].Es;
            m_lowestEsAllCurve.flow                  = m_lowestEsCurve[i].flow;
            m_lowestEsAllCurve.head                  = m_lowestEsCurve[i].head;
            m_lowestEsAllCurve.input_power_total     = m_lowestEsCurve[i].input_power_total;
            m_lowestEsAllCurve.Hz                    = m_lowestEsCurve[i].Hz;
        }
    }
    return true;
}

bool MainWindow::generatePointsOnCurve_bestEs(pointsOnCurve *pointsOnCurve, operationMode &operationMode)
{
    pumpActualPerform performList[10][101];

    if(!generatePerformList(&performList,operationMode))
        return false;

    pointsOnCurve->pumps[0]             = 1;
    pointsOnCurve->flow_total[0]        = 0;
    pointsOnCurve->head[0]              = 0;
    pointsOnCurve->Hz_vfd[0]            = 0;
    pointsOnCurve->power_input_total[0] = 0;
    pointsOnCurve->energy_specific[0]   = 0;

    for(int i=1;i<101;i++)
    {
        if( i < (m_lowestEsAllCurve.flow / m_sysPara.designTotalFlow * 100))
        {
            pointsOnCurve->pumps[i]             = m_lowestEsAllCurve.pumps;
            pointsOnCurve->flow_total[i]        = m_lowestEsAllCurve.flow;
            pointsOnCurve->head[i]              = m_lowestEsAllCurve.head;
            pointsOnCurve->Hz_vfd[i]            = m_lowestEsAllCurve.Hz;
            pointsOnCurve->power_input_total[i] = m_lowestEsAllCurve.input_power_total;
            pointsOnCurve->energy_specific[i]   = m_lowestEsAllCurve.Es;
        }
        else
        {
            pointsOnCurve->pumps[i]             = m_lowestEsCurve[i].pumps;
            pointsOnCurve->flow_total[i]        = m_lowestEsCurve[i].flow;
            pointsOnCurve->head[i]              = m_lowestEsCurve[i].head;
            pointsOnCurve->Hz_vfd[i]            = m_lowestEsCurve[i].Hz;
            pointsOnCurve->power_input_total[i] = m_lowestEsCurve[i].input_power_total;
            pointsOnCurve->energy_specific[i]   = m_lowestEsCurve[i].Es;
        }
    }

    //check motor size
    for(int i=1;i<101;i++)
    {
        int pumps = pointsOnCurve->pumps[i];
        if(performList[pumps-1][i].power_axis_vfd > m_pump.ReadMotorData().power_rate)
        {
            QString infor = "Axis power of the pump is greater than rated power of the motor!\n\n";
            infor += "system flow: " + QString::number(performList[pumps-1][i].flow_total) + "\n";
            infor += "pump flow: " + QString::number(performList[pumps-1][i].flow_vfd / pumps) + "\n";
            infor += "pump head: " + QString::number(performList[pumps-1][i].head) + "\n";
            infor += "axis power: " + QString::number(performList[pumps-1][i].power_axis_vfd);
            QMessageBox::warning(nullptr,"Warning",infor);
            return false;
        }
    }
    return true;
}

bool MainWindow::generatePointsOnCurve_fixedSpeed(pointsOnCurve *pointsOnCurve, double Hz)
{
    pointsOnCurve->pumps[0]             =1;
    pointsOnCurve->flow_total[0]        =0;
    pointsOnCurve->head[0]              =0;
    pointsOnCurve->Hz_vfd[0]            =Hz;
    pointsOnCurve->power_input_total[0] =0;
    pointsOnCurve->energy_specific[0]   =0;

    double head[10];
    double flow[10];
    double power_input_total[10];
    double energy_specific[10];

    for(int j=0;j < m_sysPara.dutyPumps; j++)
    {
        flow[j] = m_pumpSys.CalcuSystemFlow(&m_pump, j+1, Hz, &(head[j]));
        double power = m_pump.GetPowerFromFlow(Hz, flow[j] / (j+1));
        power_input_total[j] = power / m_pump.GetMotorEffi(power,Hz/50) * 100 * (j+1);
        energy_specific[j] = power_input_total[j] / flow[j];
    }

    for(int i=1;i<101;i++)
    {
        int pumps=1;

        while((m_sysPara.designTotalFlow / 100.0 * i) > (m_pump.ReadPerformance().flow_max * Hz / 50) * pumps)
            pumps++;

        if(pumps > m_sysPara.dutyPumps)
            pumps = m_sysPara.dutyPumps;

        pointsOnCurve->pumps[i]             =pumps;
        pointsOnCurve->flow_total[i]        =flow[pumps-1];
        pointsOnCurve->head[i]              =head[pumps-1];
        pointsOnCurve->Hz_vfd[i]            =Hz;
        pointsOnCurve->power_input_total[i] =power_input_total[pumps-1];
        pointsOnCurve->energy_specific[i]   =energy_specific[pumps-1];

    }

    return true;
}

void MainWindow::fillReport()
{

    //plot curve
    ui->comboBox_control_method->setCurrentIndex(0);

    ui->comboBox_control_method_2->setCurrentIndex(6);
    ui->checkBox_vfd_curve_2->setChecked(true);
    ui->checkBox_effi_curve_2->setChecked(true);
    ui->checkBox_power_curve_2->setChecked(false);

    on_multi_pump_clicked();

    //get cost and days
    double days_per_year;
    double cost_per_kWh;

    if(ui->lineEdit_days_per_year->text().toDouble()>0 and ui->lineEdit_days_per_year->text().toDouble()<=366)
        days_per_year = ui->lineEdit_days_per_year->text().toDouble();
    else
        days_per_year = 365;

    if(ui->lineEdit_cost_per_kwh->text().toDouble()>0)
        cost_per_kWh = ui->lineEdit_days_per_year->text().toDouble();
    else
        cost_per_kWh = 1;

    //read all content from excel template
    QString xlsFile = QDir::toNativeSeparators(QDir::currentPath()
                        + "/Cost Analysis Report Template.xlsx");
    if(m_xls.isNull())
        m_xls.reset(new ExcelBase);

    if(!m_xls->open(xlsFile))
    {
        QMessageBox::warning(nullptr,"Warning","Open report template failed!");
        return;
    }
    m_xls->setCurrentSheet(1);
    m_xls->readAll(m_datas,"A1:T300");

    //fill information
    m_datas[3][3] = ui->lineEdit_client->text();
    m_datas[4][3] = ui->lineEdit_project->text();
    m_datas[3][11] = ui->lineEdit_report_number->text();
    m_datas[4][11] = QDateTime::currentDateTime().date().toString();

    //fill pump and system parameter
    if(!get_pump_selection()) return;
    if(!get_system_profile()) return;
    if(!get_operation_mode()) return;

    m_datas[8][4] = m_pumpSelection.family;
    m_datas[9][4] = m_pumpSelection.model;
    m_datas[10][4] = m_motorSelection.power_rate;
    m_datas[11][4] = m_pumpSelection.speed;
    m_datas[12][4] = m_pumpSelection.impTrimed;
    m_datas[8][9] = m_sysPara.dutyPumps;
    m_datas[9][9] = m_sysPara.designTotalFlow;
    m_datas[10][9] = m_sysPara.headAtZeroFlow;
    m_datas[11][9] = m_sysPara.headAtTotalFlow;
    m_datas[12][9] = m_sysPara.controlCurveShape;

    //fill loading profile
    if(!m_pumpDb.getLoadprofile(ui->comboBox_loadprofile_app->currentText(), m_loadprofile_data))
        return;

    if(m_loadprofile_data->rowCount() > 0)
        m_datas[16][3] = m_loadprofile_data->item(0,0)->text();

    for (int i = 1, j = 18, n = 0; i < 21; i++, j++)
    {
        if(m_loadprofile_data->rowCount() > n)
        {
            if(m_loadprofile_data->item(n,1)->text().toInt() == i * 5 )
            {
                m_datas[j][3] = m_loadprofile_data->item(n,2)->text().toDouble();
                n++;
            }
            else
            {
                m_datas[j][3] = 0;
            }
        }
    }

    //full fixed speed
    pointsOnCurve pointsOnCurve_1;
    operationMode operationMode_1;
    operationMode_1.controlMethod = fullSpeed;
    generatePointsOnCurve_fullSpeed(&pointsOnCurve_1, operationMode_1);

    m_datas[45][4] = days_per_year;
    m_datas[46][4] = cost_per_kWh;

    double totalEnergy = 0;
    double totalCost = 0;

    for(int i=0; i<20; i++)
    {
        m_datas[51+i][3] = m_datas[18+i][3];
        if(m_datas[51+i][2]>0)
        {
            m_datas[51+i][4] = pointsOnCurve_1.pumps[5*(i+1)];
            m_datas[51+i][5] = pointsOnCurve_1.flow_total[5*(i+1)];
            m_datas[51+i][6] = pointsOnCurve_1.head[5*(i+1)];
            m_datas[51+i][7] = pointsOnCurve_1.effi_pump_fix[5*(i+1)];
            m_datas[51+i][8] = pointsOnCurve_1.power_axis_fix[5*(i+1)];
            m_datas[51+i][9] = pointsOnCurve_1.effi_motor_fix[5*(i+1)];
            m_datas[51+i][10] = pointsOnCurve_1.effi_total[5*(i+1)];

            if(pointsOnCurve_1.effi_motor_fix[5*(i+1)]>0)
                m_datas[51+i][11] = pointsOnCurve_1.power_axis_fix[5*(i+1)]
                        / pointsOnCurve_1.effi_motor_fix[5*(i+1)]
                        * 100
                        * m_datas[18+i][3].toDouble()
                        * pointsOnCurve_1.pumps[5*(i+1)];

            m_datas[51+i][12] = m_datas[51+i][11].toDouble() * m_datas[46][4].toDouble();

            totalEnergy += m_datas[51+i][11].toDouble();
            totalCost   += m_datas[51+i][12].toDouble();
        }
    }

    m_datas[47][4] = m_datas[45][4].toDouble() * totalEnergy;
    m_datas[48][4] = m_datas[45][4].toDouble() * totalCost;

    /*
    //all vfd Hz staging
    pointsOnCurve pointsOnCurve_2;
    operationMode operationMode_2;
    operationMode_2.controlMethod = allVfdHzC1;
    operationMode_2.stagingHz = m_reportOption.stagingHz;
    operationMode_2.destagingHz = m_reportOption.destagingHz;
    generatePointsOnCurve_allVfdHzC1(&pointsOnCurve_2, operationMode_2);

    pointsOnCurve pointsOnCurve_3;
    operationMode operationMode_3;
    operationMode_3.controlMethod = allVfdHzC2;
    operationMode_3.stagingHz = m_reportOption.stagingHz;
    operationMode_3.destagingHz = m_reportOption.destagingHz;

    generatePointsOnCurve_allVfdHzC2(&pointsOnCurve_3, operationMode_3);

    m_datas[84][4] = m_reportOption.stagingHz;
    m_datas[85][4] = m_reportOption.destagingHz;
    m_datas[86][4] = m_reportOption.daysInYear;
    m_datas[87][4] = m_reportOption.CNYPerKWh;

    totalEnergy = 0;
    totalCost = 0;

    for(int i=0; i<20; i++)
    {
        m_datas[93+i][2] = m_datas[22+i][3];
        if(m_datas[93+i][2]>0)
        {
            m_datas[93+i][3] = QString::number(pointsOnCurve_2.pumps[5*(i+1)])+" /"+QString::number(pointsOnCurve_3.pumps[5*(i+1)]);
            m_datas[93+i][4] = QString::number(pointsOnCurve_2.Hz_vfd[5*(i+1)],'f',0)+"/"+QString::number(pointsOnCurve_3.Hz_vfd[5*(i+1)],'f',0);
            m_datas[93+i][5] = QString::number(pointsOnCurve_2.flow_total[5*(i+1)],'f',1)+"/"+QString::number(pointsOnCurve_3.flow_total[5*(i+1)],'f',1);
            m_datas[93+i][6] = QString::number(pointsOnCurve_2.head[5*(i+1)],'f',1)+"/"+QString::number(pointsOnCurve_3.head[5*(i+1)],'f',1);
            m_datas[93+i][7] = QString::number(pointsOnCurve_2.effi_pump_vfd[5*(i+1)],'f',1)+"/"+QString::number(pointsOnCurve_3.effi_pump_vfd[5*(i+1)],'f',1);
            m_datas[93+i][8] = QString::number(pointsOnCurve_2.power_axis_vfd[5*(i+1)],'f',1)+"/"+QString::number(pointsOnCurve_3.power_axis_vfd[5*(i+1)],'f',1);
            m_datas[93+i][9] = QString::number(pointsOnCurve_2.effi_motor_vfd[5*(i+1)],'f',1)+"/"+QString::number(pointsOnCurve_3.effi_motor_vfd[5*(i+1)],'f',1);
            m_datas[93+i][10] = QString::number(pointsOnCurve_2.effi_total[5*(i+1)],'f',1)+"/"+QString::number(pointsOnCurve_3.effi_total[5*(i+1)],'f',1);

            if(pointsOnCurve_2.effi_motor_vfd[5*(i+1)]>0 && pointsOnCurve_3.effi_motor_vfd[5*(i+1)]>0)
                m_datas[93+i][11] = QString::number(
                        (pointsOnCurve_2.power_axis_vfd[5*(i+1)]/ pointsOnCurve_2.effi_motor_vfd[5*(i+1)] * pointsOnCurve_2.pumps[5*(i+1)]
                        +pointsOnCurve_3.power_axis_vfd[5*(i+1)] / pointsOnCurve_3.effi_motor_vfd[5*(i+1)] * pointsOnCurve_3.pumps[5*(i+1)])
                        * 100 / 2 * m_datas[22+i][3].toDouble(),'f',1);

            m_datas[93+i][12] = m_datas[93+i][11].toDouble() * m_datas[87][4].toDouble();

            totalEnergy += m_datas[93+i][11].toDouble();
            totalCost   += m_datas[93+i][12].toDouble();
        }
    }

    m_datas[88][4] = m_datas[86][4].toDouble() * totalEnergy;
    m_datas[89][4] = m_datas[86][4].toDouble() * totalCost;
    */

    //all vfd effi
    pointsOnCurve pointsOnCurve_4;
    operationMode operationMode_4;
    operationMode_4.controlMethod = allVfdEffi;
    generatePointsOnCurve_allVfdEffi(&pointsOnCurve_4, operationMode_4);

    m_datas[78][4] = days_per_year;
    m_datas[79][4] = cost_per_kWh;

    pumpActualPerform performList[10][101];

    if(!generatePerformList(&performList,operationMode_4))
        return;

    for(int i=0;i<20;i++)
    {
        m_datas[85+i][3] = pointsOnCurve_4.flow_total[5*(i+1)];

        for(int j=0;j<m_pumpSys.ReadSysPara().dutyPumps;j++)
        {
            if(performList[j][5*(i+1)].effi_total < 0.01)
                m_datas[85+i][4+j] = "EOC";
            else
                m_datas[85+i][4+j] = performList[j][5*(i+1)].effi_total;
        }
        m_xls->setCellBkColor(86+i,4+pointsOnCurve_4.pumps[5*(i+1)],8);
    }

    totalEnergy = 0;
    totalCost = 0;

    for(int i=0; i<20; i++)
    {
        m_datas[113+i][2] = m_datas[18+i][3];
        if(m_datas[113+i][2]>0)
        {
            m_datas[113+i][3] = pointsOnCurve_4.pumps[5*(i+1)];
            m_datas[113+i][4] = pointsOnCurve_4.Hz_vfd[5*(i+1)];
            m_datas[113+i][5] = pointsOnCurve_4.flow_total[5*(i+1)];
            m_datas[113+i][6] = pointsOnCurve_4.head[5*(i+1)];
            m_datas[113+i][7] = pointsOnCurve_4.effi_pump_vfd[5*(i+1)];
            m_datas[113+i][8] = pointsOnCurve_4.power_axis_vfd[5*(i+1)];
            m_datas[113+i][9] = pointsOnCurve_4.effi_motor_vfd[5*(i+1)];
            m_datas[113+i][10] = pointsOnCurve_4.effi_total[5*(i+1)];

            if(pointsOnCurve_4.effi_motor_vfd[5*(i+1)]>0)
                m_datas[113+i][11] = pointsOnCurve_4.power_axis_vfd[5*(i+1)]
                        / pointsOnCurve_4.effi_motor_vfd[5*(i+1)]
                        * 100
                        * m_datas[18+i][3].toDouble()
                        * pointsOnCurve_4.pumps[5*(i+1)];

            m_datas[113+i][12] = m_datas[113+i][11].toDouble() * m_datas[79][4].toDouble();

            totalEnergy += m_datas[113+i][11].toDouble();
            totalCost   += m_datas[113+i][12].toDouble();
        }
    }

    m_datas[80][4] = m_datas[78][4].toDouble() * totalEnergy;
    m_datas[81][4] = m_datas[78][4].toDouble() * totalCost;

    /*
    //one vfd
    pointsOnCurve pointsOnCurve_5;
    operationMode operationMode_5;
    operationMode_5.controlMethod = oneVfdC1;
    operationMode_5.stagingHz = m_reportOption.stagingHz;
    operationMode_5.destagingHz = m_reportOption.destagingHz;
    generatePointsOnCurve_oneVfdC1(&pointsOnCurve_5, operationMode_5);

    pointsOnCurve pointsOnCurve_6;
    operationMode operationMode_6;
    operationMode_6.controlMethod = oneVfdC2;
    operationMode_6.stagingHz = m_reportOption.stagingHz;
    operationMode_6.destagingHz = m_reportOption.destagingHz;
    generatePointsOnCurve_oneVfdC2(&pointsOnCurve_6, operationMode_6);

    m_datas[183][4] = m_reportOption.stagingHz;
    m_datas[184][4] = m_reportOption.destagingHz;
    m_datas[185][4] = m_reportOption.daysInYear;
    m_datas[186][4] = m_reportOption.CNYPerKWh;

    totalEnergy = 0;
    totalCost = 0;

    for(int i=0; i<20; i++)
    {
        m_datas[192+i][2] = m_datas[22+i][3];
        if(m_datas[192+i][2]>0)
        {
            m_datas[192+i][3] = QString::number(pointsOnCurve_5.pumps[5*(i+1)])+" /"+QString::number(pointsOnCurve_6.pumps[5*(i+1)]);
            m_datas[192+i][4] = QString::number(pointsOnCurve_5.Hz_vfd[5*(i+1)],'f',0)+"/"+QString::number(pointsOnCurve_6.Hz_vfd[5*(i+1)],'f',0);
            m_datas[192+i][5] = QString::number(pointsOnCurve_5.flow_total[5*(i+1)],'f',1)+"/"+QString::number(pointsOnCurve_6.flow_total[5*(i+1)],'f',1);
            m_datas[192+i][6] = QString::number(pointsOnCurve_5.head[5*(i+1)],'f',1)+"/"+QString::number(pointsOnCurve_6.head[5*(i+1)],'f',1);
            m_datas[192+i][7] = "N/A";
            m_datas[192+i][8] = "N/A";
            m_datas[192+i][9] = "N/A";
            m_datas[192+i][10] = QString::number(pointsOnCurve_5.effi_total[5*(i+1)],'f',1)+"/"+QString::number(pointsOnCurve_6.effi_total[5*(i+1)],'f',1);

            double input_power_1 = pointsOnCurve_5.power_input_total[5*(i+1)];
            double input_power_2 = pointsOnCurve_6.power_input_total[5*(i+1)];

            m_datas[192+i][11] = QString::number((input_power_1 + input_power_2) / 2 * m_datas[192+i][2].toDouble(),'f',1);
            m_datas[192+i][12] = m_datas[192+i][11].toDouble() * m_datas[187][4].toDouble();

            totalEnergy += m_datas[192+i][11].toDouble();
            totalCost   += m_datas[192+i][12].toDouble();
        }
    }

    m_datas[188][4] = m_datas[186][4].toDouble() * totalEnergy;
    m_datas[189][4] = m_datas[186][4].toDouble() * totalCost;
    */

    //insert curve
    QPixmap map = ui->customPlot_2->grab();
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setPixmap(map);
    m_xls->paste(140,1);

    //delete blank rowss
    for(int i=51;i<51+20;i++)
    {
        if(m_datas[i][3].toDouble() < 0.01)
            m_xls->hideOneRow(i+1);
    }

    for(int i=85,j=18;i<85+20;i++,j++)
    {
        if(m_datas[j][3].toDouble() < 0.01)
            m_xls->hideOneRow(i+1);
    }

    for(int i=113;i<113+20;i++)
    {
        if(m_datas[i][2].toDouble() < 0.01)
            m_xls->hideOneRow(i+1);
    }


    //save as to report
    QFileDialog fileDialog(this);
    QString saveAsName = fileDialog.getSaveFileName(this,"Report save as...", QDir::currentPath(),"*.pdf");
    saveAsName.replace('/','\\');
    if(saveAsName.isEmpty())
        return;

    m_xls->writeCurrentSheet(m_datas);
    m_xls->saveAsPDF(saveAsName);
    m_xls->close();
}

void MainWindow::fillReport2()
{
    //read all content from excel template
    QString xlsFile = QDir::toNativeSeparators(QDir::currentPath()
                        + "/Cost Analysis Report Template 2.xlsx");
    if(m_xls.isNull())
        m_xls.reset(new ExcelBase);

    if(!m_xls->open(xlsFile))
    {
        QMessageBox::warning(nullptr,"Warning","Open report template failed!");
        return;
    }
    m_xls->setCurrentSheet(1);
    m_xls->readAll(m_datas,"A1:R150");

    //fill pump and system parameter
    if(!get_pump_selection()) return;
    if(!get_system_profile()) return;
    if(!get_operation_mode()) return;

    m_datas[8][4] = m_pumpSelection.family;
    m_datas[9][4] = m_pumpSelection.model;
    m_datas[10][4] = m_motorSelection.power_rate;
    m_datas[11][4] = m_pumpSelection.speed;
    m_datas[12][4] = m_pumpSelection.impTrimed;
    m_datas[8][8] = m_sysPara.dutyPumps;
    m_datas[9][8] = m_sysPara.designTotalFlow;
    m_datas[10][8] = m_sysPara.headAtZeroFlow;
    m_datas[11][8] = m_sysPara.headAtTotalFlow;
    m_datas[12][8] = m_sysPara.controlCurveShape;

    //fill loading profile
    if(!m_pumpDb.getLoadprofile(ui->comboBox_loadprofile_app->currentText(), m_loadprofile_data))
        return;

    if(m_loadprofile_data->rowCount() > 0)
        m_datas[20][3] = m_loadprofile_data->item(0,0)->text();

    for (int i = 1, j = 22, n = 0; i < 21; i++, j++)
    {
        if(m_loadprofile_data->rowCount() > n)
        {
            if(m_loadprofile_data->item(n,1)->text().toInt() == i * 5 )
            {
                m_datas[j][3] = m_loadprofile_data->item(n,2)->text().toDouble();
                n++;
            }
            else
            {
                m_datas[j][3] = 0;
            }
        }
    }

    //fixed speed
    pointsOnCurve pointsOnCurve_1;
    operationMode operationMode_1;
    operationMode_1.controlMethod = fullSpeed;
    generatePointsOnCurve_fixedSpeed(&pointsOnCurve_1, m_reportOption_2.perferedHz);

    m_datas[51][4] = m_reportOption_2.daysInYear;
    m_datas[52][4] = m_reportOption_2.CNYPerKWh;

    double totalEnergy = 0;
    double totalCost = 0;

    for(int i=0; i<20; i++)
    {
        m_datas[57+i][2] = m_datas[22+i][3];
        if(m_datas[57+i][2]>0)
        {
            m_datas[57+i][3] = QString::number(pointsOnCurve_1.pumps[5*(i+1)]);
            m_datas[57+i][4] = QString::number(pointsOnCurve_1.Hz_vfd[5*(i+1)],'f',0);
            m_datas[57+i][5] = QString::number(5 * (i+1) / 100.0
                      * m_sysPara.designTotalFlow,'f',1);
            m_datas[57+i][6] = QString::number(pointsOnCurve_1.flow_total[5*(i+1)],'f',1);
            m_datas[57+i][7] = QString::number(pointsOnCurve_1.head[5*(i+1)],'f',1);
            m_datas[57+i][8] = QString::number(pointsOnCurve_1.power_input_total[5*(i+1)],'f',1);
            m_datas[57+i][9] = QString::number(5 * (i+1) / 100.0
                      * m_sysPara.designTotalFlow
                      / pointsOnCurve_1.flow_total[5*(i+1)]
                      * m_datas[57+i][2].toDouble() ,'f',2);
            m_datas[57+i][10] = QString::number(pointsOnCurve_1.energy_specific[5*(i+1)],'f',4);
            m_datas[57+i][11] = QString::number(5 * (i+1) / 100.0
                      * pointsOnCurve_1.power_input_total[5*(i+1)]
                      * m_sysPara.designTotalFlow
                      / pointsOnCurve_1.flow_total[5*(i+1)]
                      * m_datas[57+i][2].toDouble(),'f',1);
            m_datas[57+i][12] = m_datas[57+i][11].toDouble() * m_datas[52][4].toDouble();

            totalEnergy += m_datas[57+i][10].toDouble();
            totalCost   += m_datas[57+i][11].toDouble();
        }
    }

    m_datas[53][4] = m_datas[51][4].toDouble() * totalEnergy;
    m_datas[54][4] = m_datas[51][4].toDouble() * totalCost;

    //all vfd lowest Es
    pointsOnCurve pointsOnCurve[10];
    struct pointsOnCurve pointsOnCurve_2;
    operationMode operationMode;
    operationMode.controlMethod = allVfdEffi;

    generatePointsOnCurve_specificEnergy(&pointsOnCurve,operationMode);
    generatePointsOnCurve_bestEs(&pointsOnCurve_2, operationMode);

    m_datas[87][4] = m_reportOption_2.daysInYear;
    m_datas[88][4] = m_reportOption_2.CNYPerKWh;

    totalEnergy = 0;
    totalCost = 0;

    for(int i=0; i<20; i++)
    {
        m_datas[93+i][2] = m_datas[22+i][3];
        if(m_datas[93+i][2]>0)
        {
            m_datas[93+i][3] = QString::number(pointsOnCurve_2.pumps[5*(i+1)]);
            m_datas[93+i][4] = QString::number(pointsOnCurve_2.Hz_vfd[5*(i+1)],'f',0);
            m_datas[93+i][5] = QString::number(5 * (i+1) / 100.0
                      * m_sysPara.designTotalFlow,'f',1);
            m_datas[93+i][6] = QString::number(pointsOnCurve_2.flow_total[5*(i+1)],'f',1);
            m_datas[93+i][7] = QString::number(pointsOnCurve_2.head[5*(i+1)],'f',1);
            m_datas[93+i][8] = QString::number(pointsOnCurve_2.power_input_total[5*(i+1)],'f',1);
            m_datas[93+i][9] = QString::number(5 * (i+1) / 100.0
                      * m_sysPara.designTotalFlow
                      / pointsOnCurve_2.flow_total[5*(i+1)]
                      * m_datas[93+i][2].toDouble() ,'f',2);
            m_datas[93+i][10] = QString::number(pointsOnCurve_2.energy_specific[5*(i+1)],'f',4);
            m_datas[93+i][11] = QString::number(5 * (i+1) / 100.0
                      * pointsOnCurve_2.power_input_total[5*(i+1)]
                      * m_sysPara.designTotalFlow
                      / pointsOnCurve_2.flow_total[5*(i+1)]
                      * m_datas[93+i][2].toDouble(),'f',1);
            m_datas[93+i][12] = m_datas[93+i][11].toDouble() * m_datas[88][4].toDouble();

            totalEnergy += m_datas[93+i][11].toDouble();
            totalCost   += m_datas[93+i][12].toDouble();
        }
    }

    m_datas[89][4] = m_datas[87][4].toDouble() * totalEnergy;
    m_datas[90][4] = m_datas[87][4].toDouble() * totalCost;

    //hide blank rows
    for(int i=57;i<57+20;i++)
    {
        if(m_datas[i][2].toDouble() < 0.01)
            m_xls->hideOneRow(i+1);
    }

    for(int i=93;i<93+20;i++)
    {
        if(m_datas[i][2].toDouble() < 0.01)
            m_xls->hideOneRow(i+1);
    }

    //save as to report
    QFileDialog fileDialog(this);
    QString saveAsName = fileDialog.getSaveFileName(this,"Report save as...", QDir::currentPath(),"*.xlsx");
    saveAsName.replace('/','\\');
    if(saveAsName.isEmpty())
        return;

    m_xls->writeCurrentSheet(m_datas);
    m_xls->saveAs(saveAsName);
    m_xls->close();
}

