#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pump.h"
#include "piping.h"
#include <QComboBox>
#include "qcustomplot.h"
#include "global.h"
#include "math.h"


extern Pump g_pump[10];
extern int  g_model_qty;
extern int  g_current_model;
extern CurveChoose g_pump_curve_choose;
extern SensorlessCurveChoose g_sensorless_curve_choose;
extern int g_pump_total_qty;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_timer_replot = new QTimer();

    connect(ui->comboBox_pumpmodel_qty, SIGNAL(currentTextChanged(QString)), this, SLOT(on_comboBox_pumpmodel_qty_currentIndexChanged(QString)));
    connect(ui->comboBox_model_number, SIGNAL(currentTextChanged(QString)), this, SLOT(on_comboBox_model_number_currentIndexChanged(QString)));
    connect(ui->comboBox_pump_qty, SIGNAL(currentTextChanged(QString)), this, SLOT(on_comboBox_pump_qty_currentIndexChanged(QString)));
    connect(ui->comboBox_choose_pump_curve, SIGNAL(currentTextChanged(QString)), this, SLOT(on_comboBox_choose_pump_curve_currentTextChange(QString)));
    connect(ui->lineEdit_Hz, SIGNAL(editingFinished()), this, SLOT(on_lineEdit_Hz_editingFinished()));
    connect(ui->comboBox_model_number_2, SIGNAL(currentTextChanged(QString)), this, SLOT(on_comboBox_pump_number_2_currentTextChanged(QString)));
    connect(ui->comboBox_choose_sensorless_curve, SIGNAL(currentTextChanged(QString)), this, SLOT(on_comboBox_choose_pump_curve_currentTextChange(QString)));
    connect(ui->comboBox_model_number_3, SIGNAL(currentTextChanged(QString)), this, SLOT(on_comboBox_pump_number_3_currentTextChanged(QString)));
    //connect(m_timer_replot,SIGNAL(timeout()),this,SLOT(on_timer_replot()));

    //on_lineEdit_Hz_editingFinished()
    //buttons don't need a connect function because they are automatically connected


    for (int i=0; i<POINTS_MAX; i++)
    {
        m_test_point_ellipse[i] = new QCPItemEllipse(ui->customPlot);
        m_test_point_ellipse[i]->setVisible(false);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_comboBox_pumpmodel_qty_currentIndexChanged(const QString &arg1)
{
    int model_qty_input = arg1.toInt();
    if(model_qty_input > 0 && model_qty_input <= 10)
    {
        g_model_qty = model_qty_input;
        ui->comboBox_model_number->clear();
        ui->comboBox_model_number_2->clear();
        ui->comboBox_model_number_3->clear();
        for (int i=1; i<=g_model_qty; i++)
        {
            ui->comboBox_model_number->addItem(QString::number(i));
            ui->comboBox_model_number_2->addItem(QString::number(i));
            ui->comboBox_model_number_3->addItem(QString::number(i));
        }
    }
}


void MainWindow::on_comboBox_model_number_currentIndexChanged(const QString &arg1)
{
    int current_pump_input = arg1.toInt();
    if(current_pump_input > 0 && current_pump_input <= g_model_qty)
    {
        g_current_model = current_pump_input;
        ui->comboBox_model_number_2->setCurrentText(QString::number(g_current_model));
        ui->comboBox_model_number_3->setCurrentText(QString::number(g_current_model));
        ui->comboBox_pump_qty->setCurrentText(QString::number(g_pump[g_current_model].GetPumpQty()));
    }
}


void MainWindow::on_comboBox_choose_pump_curve_currentTextChanged(const QString &arg1)
{
    if(arg1 == QString("Flow - Head"))
        g_pump_curve_choose = FlowHead;
    else if(arg1 == QString("Flow - Power"))
        g_pump_curve_choose = FlowPower;
    else if(arg1 == QString("Flow - Efficiency"))
        g_pump_curve_choose = FlowEffi;
    else if(arg1 == QString("Flow - NPSHR"))
        g_pump_curve_choose = FlowNpshr;
    else if(arg1 == QString("Power - Efficiency"))
        g_pump_curve_choose = PowerEffi;
}

void MainWindow::on_pushButton_import_pump_curve_clicked()
{
    /* read text from csv file, store to csv model */
    QString filename;
    QFileDialog fileDialog(this);
    filename = fileDialog.getOpenFileName(this,"Open Pump And Motor Curve file",QDir::currentPath(),"*.csv");
    if(filename=="")
        return;
    QxtCsvModel csvModel(this);
    csvModel.setSource(filename,false,',',nullptr);

    if(csvModel.rowCount() < 20 || csvModel.columnCount() < 5)
    {
        QMessageBox::information(NULL, "Warning", "Please check CSV file integrity!");
        return;
    }

    /* read pump and motor performance data from csv model */
    PumpPerformTestData pump_data;
    MotorPerformTestData motor_data;
    pump_data.vfd_installed = csvModel.text(0, 1).toInt();
    pump_data.Hz_max = csvModel.text(0, 3).toInt();
    pump_data.Hz_min = csvModel.text(0, 5).toInt();
    pump_data.flow_max = csvModel.text(1, 3).toDouble();
    pump_data.flow_min = csvModel.text(1, 1).toDouble();
    for(int i=0; i<POINTS_MAX; i++)
    {
        pump_data.flow[i] = csvModel.text(i+4, 1).toDouble();
        pump_data.head[i] = csvModel.text(i+4, 2).toDouble();
        pump_data.power[i] = csvModel.text(i+4, 3).toDouble();
        pump_data.NPSHR[i] = csvModel.text(i+4, 4).toDouble();
    }

    motor_data.power_rated = csvModel.text(2, 1).toDouble();
    for(int i=0; i<MOTOR_POINTS_MAX; i++)
    {
        motor_data.power[i] = csvModel.text(i+15, 1).toDouble();
        motor_data.efficiency[i] = csvModel.text(i+15, 2).toDouble();
    }

    /* generate pump model from pump performance data */
    Q_ASSERT(g_current_model > 0);
    Q_ASSERT(g_current_model <= 4);
    QString message;
    if (!g_pump[g_current_model].ImportPumpMotorModel(pump_data, motor_data, &message))
    {
        QMessageBox::information(NULL, "Warning", message);
        return;
    }
}

void MainWindow::on_pushButton_plot_pump_curve_clicked()
{
    /* check if pump model imported */
    if(!g_pump[g_current_model].IsPumpMotorImported())
    {
        QMessageBox::information(NULL,"Warning","Pump curve isn't imported!");
        return;
    }

    /* choose curve */
    double (Pump::*CalcuY)(double, double, bool*) = &Pump::GetPumpHeadFromFlow;
    QString label, name;
    if (g_pump_curve_choose == FlowHead)
    {
        CalcuY = &Pump::GetPumpHeadFromFlow;
        label = QString("Head(m)");
        name = QString("head");
    }
    else if (g_pump_curve_choose == FlowPower)
    {
        if (!m_assembly_plot_flag)
            CalcuY = &Pump::GetPumpPowerFromFlow;
        else
            CalcuY = &Pump::GetAssemblyPowerFromFlow;
        label = QString("Power(kW)");
        name = QString("power");
    }
    else if  (g_pump_curve_choose == FlowEffi)
    {
        if (!m_assembly_plot_flag)
            CalcuY = &Pump::GetPumpEffiFromFlow;
        else
            CalcuY = &Pump::GetAssemblyEffiFromFlow;
        label = QString("Efficiency(%)");
        name = QString("efficiency");
    }
    else if  (g_pump_curve_choose == FlowNpshr)
    {
        CalcuY = &Pump::GetPumpNpshrFromFlow;
        label = QString("NPSHR(m)");
        name = QString("npshr");
    }
    else if  (g_pump_curve_choose == PowerEffi)
    {
        CalcuY = &Pump::GetMotorEffiFromPower;
        label = QString("Power(kW)");
        name = QString("efficiency(%)");
    }

    //curve data
    double max_x = 0;
    double max_y = 0;
    QVector<double> x(101), y(101);

    //clear graph
    ui->customPlot->clearGraphs();
    ui->customPlot->clearTracerTexts();

    //add graph
    QCPGraph *graph1;
    bool EOC;
    if (g_pump_curve_choose != PowerEffi)
    {
        double flow_step = (g_pump[g_current_model].GetPumpMotorModel().flow_max
                            - g_pump[g_current_model].GetPumpMotorModel().flow_min) / 100;
        double Hz = g_pump[g_current_model].GetPumpMotorModel().Hz_max;
        double flow = g_pump[g_current_model].GetPumpMotorModel().flow_min;
        for (int i=0; i<101; ++i)
        {
            x[i] = flow;
            y[i] = (g_pump[g_current_model].*CalcuY)(Hz, flow, &EOC);
            if(y[i]>max_y)
                max_y=y[i];
            flow += flow_step;
        }
    }
    else
    {
        double power_step = g_pump[g_current_model].GetPumpMotorModel().power_rated / 100;
        double Hz = g_pump[g_current_model].GetPumpMotorModel().Hz_max;
        double power = power_step * 25;
        for (int i=0; i<101; ++i)
        {
            x[i] = power;
            y[i] = g_pump[g_current_model].GetMotorEffiFromPower(Hz, power);
            if(y[i]>max_y)
                max_y=y[i];
            power += power_step;
        }
    }

    graph1 = ui->customPlot->addGraph(ui->customPlot->xAxis,ui->customPlot->yAxis);
    graph1->setData(x, y);
    graph1->setPen(QColor(Qt::red));
    graph1->setName(name);

    //set legend
    ui->customPlot->legend->setFont(QFont("微软雅黑",9));
    ui->customPlot->legend->setVisible(true);

    //set axises
    if (g_pump_curve_choose != PowerEffi)
        ui->customPlot->xAxis->setLabel("Flow(m3/h)");
    else
        ui->customPlot->xAxis->setLabel("Power(kW)");
    ui->customPlot->yAxis->setLabel(label);

    max_x = x[100];
    ui->customPlot->xAxis->setRange(0, max_x*1.2);
    ui->customPlot->yAxis->setRange(0, max_y*1.2);
    ui->customPlot->xAxis->setLabelFont(QFont("微软雅黑",9));
    ui->customPlot->yAxis->setLabelFont(QFont("微软雅黑",9));
    ui->customPlot->xAxis->setVisible(true);
    ui->customPlot->yAxis->setVisible(true);

    //set plot
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                    QCP::iSelectLegend | QCP::iSelectPlottables);
    //add tracer text
    ui->customPlot->addTracerText(QColor(0,0,0),QFont("微软雅黑",9));
    ui->customPlot->addTracerText(graph1->pen().color(),QFont("微软雅黑",9));

    //add tracer
    ui->customPlot->addTracer();

    //test data point
    double test_data_x[POINTS_MAX], test_data_y[POINTS_MAX];
    QPen pen(Qt::red, 1);
    QBrush brush(Qt::red);
    double x_radius = (ui->customPlot->xAxis->pixelToCoord(1)-ui->customPlot->xAxis->pixelToCoord(0))*3;
    double y_radius = (ui->customPlot->yAxis->pixelToCoord(0)-ui->customPlot->yAxis->pixelToCoord(1))*3;
    double *test_data_y_source;

    if (g_pump_curve_choose == FlowHead)
        test_data_y_source = g_pump[g_current_model].GetPumpPerformTestData().head;
    else if (g_pump_curve_choose == FlowPower)
        test_data_y_source = g_pump[g_current_model].GetPumpPerformTestData().power;
    else if (g_pump_curve_choose == FlowNpshr)
        test_data_y_source = g_pump[g_current_model].GetPumpPerformTestData().NPSHR;
    else if (g_pump_curve_choose == PowerEffi)
        test_data_y_source = g_pump[g_current_model].GetMotorPerformTestData().efficiency;

    if ((g_pump_curve_choose == FlowHead
            || g_pump_curve_choose == FlowPower
            || g_pump_curve_choose == FlowNpshr) && !m_assembly_plot_flag)
    {
        for (int i=0; i<POINTS_MAX; i++)
        {
            test_data_x[i] = g_pump[g_current_model].GetPumpPerformTestData().flow[i];
            test_data_y[i] = test_data_y_source[i];
            m_test_point_ellipse[i]->setPen(pen);
            m_test_point_ellipse[i]->setBrush(brush);
            m_test_point_ellipse[i]->setVisible(false);
            m_test_point_ellipse[i]->topLeft->setType(QCPItemPosition::ptPlotCoords);
            m_test_point_ellipse[i]->bottomRight->setType(QCPItemPosition::ptPlotCoords);
            m_test_point_ellipse[i]->topLeft->setCoords(test_data_x[i] - x_radius, test_data_y[i] + y_radius);
            m_test_point_ellipse[i]->bottomRight->setCoords(test_data_x[i] + x_radius, test_data_y[i] - y_radius);
            m_test_point_ellipse[i]->setVisible(true);
        }
    }
    else if ((g_pump_curve_choose == PowerEffi) && !m_assembly_plot_flag)
    {
        for (int i=0; i<MOTOR_POINTS_MAX; i++)
        {
            test_data_x[i] = g_pump[g_current_model].GetMotorPerformTestData().power[i];
            test_data_y[i] = test_data_y_source[i];
            m_test_point_ellipse[i]->setPen(pen);
            m_test_point_ellipse[i]->setBrush(brush);
            m_test_point_ellipse[i]->setVisible(false);
            m_test_point_ellipse[i]->topLeft->setType(QCPItemPosition::ptPlotCoords);
            m_test_point_ellipse[i]->bottomRight->setType(QCPItemPosition::ptPlotCoords);
            m_test_point_ellipse[i]->topLeft->setCoords(test_data_x[i] - x_radius, test_data_y[i] + y_radius);
            m_test_point_ellipse[i]->bottomRight->setCoords(test_data_x[i] + x_radius, test_data_y[i] - y_radius);
            m_test_point_ellipse[i]->setVisible(true);
        }
    }
    else
        for (int i=0; i<POINTS_MAX; i++)
            m_test_point_ellipse[i]->setVisible(false);

    //replot
    ui->customPlot->replot();

    m_assembly_plot_flag = false;

}

void MainWindow::on_pushButton_plot_assembly_curve_clicked()
{
    m_assembly_plot_flag = true;
    on_pushButton_plot_pump_curve_clicked();
}

void MainWindow::on_lineEdit_Hz_editingFinished()
{
    double Hz = ui->lineEdit_Hz->text().toDouble();
    if (Hz <= g_pump[g_current_model].GetPumpMotorModel().Hz_max &&
            Hz >= g_pump[g_current_model].GetPumpMotorModel().Hz_min)
    {
        m_sensorless_Hz = Hz;
    }
    else
    {
        ui->lineEdit_Hz->setText(QString::number(g_pump[g_current_model].GetPumpMotorModel().Hz_max));
        QMessageBox::information(NULL, "Warning", "Hz input is out of range!");
    }
}


void MainWindow::on_comboBox_pump_number_2_currentTextChanged(const QString &arg1)
{
    int current_pump_input = arg1.toInt();
    if(current_pump_input > 0 && current_pump_input <= g_model_qty)
    {
        g_current_model = current_pump_input;
        ui->comboBox_model_number->setCurrentText(QString::number(g_current_model));
        ui->comboBox_model_number_3->setCurrentText(QString::number(g_current_model));
        ui->comboBox_pump_qty->setCurrentText(QString::number(g_pump[g_current_model].GetPumpQty()));
    }
}


void MainWindow::on_comboBox_choose_sensorless_curve_currentTextChanged(const QString &arg1)
{
    if(arg1 == QString("Power - Flow"))
        g_sensorless_curve_choose = PowerFlow;
    else if(arg1 == QString("Power - Head"))
        g_sensorless_curve_choose = PowerHead;
}


void MainWindow::on_pushButton_plot_sensorless_curve_clicked()
{
    /* check if pump model imported */
    if(!g_pump[g_current_model].IsPumpMotorImported())
    {
        QMessageBox::information(NULL,"Warning","Pump curve isn't imported!");
        return;
    }

    /* get Hz input */
    on_lineEdit_Hz_editingFinished();
    if (m_sensorless_Hz > g_pump[g_current_model].GetPumpMotorModel().Hz_max
            || m_sensorless_Hz < g_pump[g_current_model].GetPumpMotorModel().Hz_min)
    {
        QMessageBox::information(NULL, "Warning", "Hz input is out of range!");
        return;
    }

    /* choose curve */
    double (Pump::*CalcuY)(double, double, bool*) = &Pump::GetAssemblyFlowFromPower;
    QString label, name;
    if (g_sensorless_curve_choose == PowerFlow)
    {
        CalcuY = &Pump::GetAssemblyFlowFromPower;
        label = QString("Flow(m3/h)");
        name = QString("flow");
    }
    else if (g_sensorless_curve_choose == PowerHead)
    {
        CalcuY = &Pump::GetAssemblyHeadFromPower;
        label = QString("Head(m)");
        name = QString("head");
    }

    //curve data
    double max_x = 0;
    double max_y = 0;
    QVector<double> x(101), y(101);

    //clear graph
    ui->customPlot_2->clearGraphs();
    ui->customPlot_2->clearTracerTexts();

    //add graph
    QCPGraph *graph1;
    bool EOC;
    double related_speed = m_sensorless_Hz / g_pump[g_current_model].GetPumpMotorModel().Hz_max;
    double power_min = g_pump[g_current_model].GetAssemblyPowerFromFlow(m_sensorless_Hz, 0);
    double power_max = g_pump[g_current_model].GetAssemblyPowerFromFlow(m_sensorless_Hz,
                     g_pump[g_current_model].GetPumpMotorModel().flow_max * related_speed);
    double power_step = (power_max - power_min) / 100;
    double power = power_min;
    for (int i=0; i<101; ++i)
    {
        x[i] = power;
        y[i] = (g_pump[g_current_model].*CalcuY)(m_sensorless_Hz, power, &EOC);
        if(y[i]>max_y)
            max_y=y[i];
        power += power_step;
    }

    graph1 = ui->customPlot_2->addGraph(ui->customPlot_2->xAxis,ui->customPlot_2->yAxis);
    graph1->setData(x, y);
    graph1->setPen(QColor(Qt::red));
    graph1->setName(name);

    //set legend
    ui->customPlot_2->legend->setFont(QFont("微软雅黑",9));
    ui->customPlot_2->legend->setVisible(true);

    //set axises
    ui->customPlot_2->xAxis->setLabel("Power(kW)");
    ui->customPlot_2->yAxis->setLabel(label);

    max_x = x[100];
    ui->customPlot_2->xAxis->setRange(0, max_x*1.2);
    ui->customPlot_2->yAxis->setRange(0, max_y*1.2);
    ui->customPlot_2->xAxis->setLabelFont(QFont("微软雅黑",9));
    ui->customPlot_2->yAxis->setLabelFont(QFont("微软雅黑",9));
    ui->customPlot_2->xAxis->setVisible(true);
    ui->customPlot_2->yAxis->setVisible(true);

    //set plot
    ui->customPlot_2->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                    QCP::iSelectLegend | QCP::iSelectPlottables);
    //add tracer text
    ui->customPlot_2->addTracerText(QColor(0,0,0),QFont("微软雅黑",9));
    ui->customPlot_2->addTracerText(graph1->pen().color(),QFont("微软雅黑",9));

    //add tracer
    ui->customPlot_2->addTracer();

    //replot
    ui->customPlot_2->replot();

}


void MainWindow::on_comboBox_pump_number_3_currentTextChanged(const QString &arg1)
{
    int current_pump_input = arg1.toInt();
    if(current_pump_input > 0 && current_pump_input <= g_model_qty)
    {
        g_current_model = current_pump_input;
        ui->comboBox_model_number->setCurrentText(QString::number(g_current_model));
        ui->comboBox_model_number_2->setCurrentText(QString::number(g_current_model));
        ui->comboBox_pump_qty->setCurrentText(QString::number(g_pump[g_current_model].GetPumpQty()));
    }

}


void MainWindow::on_pushButton_plot_vfd_curve_clicked()
{
    /* check if pump model imported */
    if(!g_pump[g_current_model].IsPumpMotorImported())
    {
        QMessageBox::information(NULL,"Warning","水泵、电机曲线未导入");
        return;
    }

    //curve data
    double max_x = 0;
    double max_y = 0;
    double max_y2 = 0;

    //clear graph
    ui->customPlot_3->clearGraphs();
    ui->customPlot_3->clearTracerTexts();

    //add graph at maximum Hz
    QCPGraph *graph1, *graph2, *graph3;
    bool EOC;
    double flow_step = (g_pump[g_current_model].GetPumpMotorModel().flow_max
                        - g_pump[g_current_model].GetPumpMotorModel().flow_min) / 100;
    double Hz_1 = g_pump[g_current_model].GetPumpMotorModel().Hz_max;
    double Hz_2 = ui->lineEdit_Hz_calculated->text().toDouble();
    if (Hz_2 > g_pump[g_current_model].GetPumpMotorModel().Hz_max
            || Hz_2 < g_pump[g_current_model].GetPumpMotorModel().Hz_min)
    {
        QMessageBox::information(NULL, "Warning", "计算出的频率超限!");
        return;
    }

    QVector<double> x(101), y(101);
    QVector<double> y_2((int)(Hz_2/Hz_1*101)), y_3((int)(Hz_2/Hz_1*101));

    double flow = g_pump[g_current_model].GetPumpMotorModel().flow_min;
    for (int i=0; i<101; ++i)
    {
        x[i] = flow;
        y[i] = g_pump[g_current_model].GetPumpHeadFromFlow(Hz_1, flow, &EOC);
        if (i < (int)(Hz_2/Hz_1*101)) {
            y_2[i] = g_pump[g_current_model].GetPumpHeadFromFlow(Hz_2, flow, &EOC);
            y_3[i] = g_pump[g_current_model].GetAssemblyPowerFromFlow(Hz_2, flow, &EOC);
            if(y_3[i]>max_y2)
                max_y2=y_3[i];
        }
        if(y[i]>max_y)
            max_y=y[i];

        flow += flow_step;
    }

    graph1 = ui->customPlot_3->addGraph(ui->customPlot_3->xAxis,ui->customPlot_3->yAxis);
    graph1->setData(x, y);
    graph1->setPen(QPen(QBrush(QColor(Qt::black)), 1, Qt::DotLine));
    graph1->setName(QString("head at ") + QString::number(Hz_1, 'f', 1) + QString(" Hz"));

    graph2 = ui->customPlot_3->addGraph(ui->customPlot_3->xAxis,ui->customPlot_3->yAxis);
    graph2->setData(x, y_2);
    graph2->setPen(QPen(QBrush(QColor(Qt::red)), 1, Qt::SolidLine));
    graph2->setName(QString("head at ") + QString::number(Hz_2, 'f', 1) + QString(" Hz"));

    graph3 = ui->customPlot_3->addGraph(ui->customPlot_3->xAxis,ui->customPlot_3->yAxis2);
    graph3->setData(x, y_3);
    graph3->setPen(QPen(QBrush(QColor(Qt::blue)), 1, Qt::SolidLine));
    graph3->setName(QString("power at ") + QString::number(Hz_2, 'f', 1) + QString(" Hz"));

    //set legend
    ui->customPlot_3->legend->setFont(QFont("微软雅黑",9));
    ui->customPlot_3->legend->setVisible(true);

    //set axises
    ui->customPlot_3->xAxis->setLabel("Flow(m3/h)");
    ui->customPlot_3->yAxis->setLabel("Head(m)");
    ui->customPlot_3->yAxis2->setLabel("Power(kW)");

    max_x = x[100];
    ui->customPlot_3->xAxis->setRange(0, max_x*1.2);
    ui->customPlot_3->yAxis->setRange(0, max_y*1.2);
    ui->customPlot_3->yAxis2->setRange(0, max_y2*1.2);
    ui->customPlot_3->xAxis->setLabelFont(QFont("微软雅黑",9));
    ui->customPlot_3->yAxis->setLabelFont(QFont("微软雅黑",9));
    ui->customPlot_3->yAxis2->setLabelFont(QFont("微软雅黑",9));
    ui->customPlot_3->xAxis->setVisible(true);
    ui->customPlot_3->yAxis->setVisible(true);
    ui->customPlot_3->yAxis2->setVisible(true);

    //set plot
    ui->customPlot_3->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                    QCP::iSelectLegend | QCP::iSelectPlottables);
    //add tracer text
    ui->customPlot_3->addTracerText(QColor(Qt::black),QFont("微软雅黑",9));
    ui->customPlot_3->addTracerText(QColor(Qt::black),QFont("微软雅黑",9));
    ui->customPlot_3->addTracerText(QColor(Qt::red),QFont("微软雅黑",9));
    ui->customPlot_3->addTracerText(QColor(Qt::blue),QFont("微软雅黑",9));

    //add tracer
    ui->customPlot_3->addTracer();

}


void MainWindow::on_pushButton_Hz_calculation_clicked()
{
    ui->lineEdit_Hz_calculated->setText("");

    /* check if pump model imported */
    if(!g_pump[g_current_model].IsPumpMotorImported())
    {
        QMessageBox::information(NULL,"Warning","Pump curve isn't imported!");
        return;
    }

    /* get target flow and head from input */
    double flow = ui->lineEdit_flow->text().toDouble();
    if (flow < g_pump[g_current_model].GetPumpMotorModel().flow_min
            || flow > g_pump[g_current_model].GetPumpMotorModel().flow_max)
    {
        QMessageBox::information(NULL, "Warning", "Flow input is out of range!");
        return;
    }

    double head = ui->lineEdit_head->text().toDouble();
    if (head <= 0)
    {
        QMessageBox::information(NULL, "Warning", "Head input should not below zero!");
        return;
    }

    /* calculation of Hz */
    bool upper_limit_flag, lower_limit_flag;
    double Hz = g_pump[g_current_model].CalcuHz(flow, head, &upper_limit_flag, &lower_limit_flag);
    if (upper_limit_flag)
    {
        QMessageBox::information(NULL, "Warning", "Upper limit exceeded!");
        return;
    }
    if (lower_limit_flag)
    {
        QMessageBox::information(NULL, "Warning", "Lower limit exceeded!");
        return;
    }
    ui->lineEdit_Hz_calculated->setText(QString::number(Hz, 'f', 1));

}


void MainWindow::on_comboBox_pump_qty_currentTextChanged(const QString &arg1)
{
    int pump_qty_input = arg1.toInt();
    if (pump_qty_input > 0 && pump_qty_input <= 8)
    {
        int total_pumps = 0;
        for (int i=1; i<=g_model_qty; i++)
        {
            if (i != g_current_model)
                total_pumps += g_pump[i].GetPumpQty();
        }
        total_pumps += pump_qty_input;
        if (total_pumps > 8)
        {
            QMessageBox::information(NULL, "Warning", "水泵总数不能超过8台！");
            ui->comboBox_pump_qty->setCurrentText(QString::number(g_pump[g_current_model].GetPumpQty()));
        }
        else
            g_pump[g_current_model].SetPumpQty(pump_qty_input);
    }

    g_pump_total_qty = 0;
    for (int i=1; i<=g_model_qty; i++) {
        g_pump_total_qty += g_pump[i].GetPumpQty();
    }
    return;
}


void MainWindow::on_pushButton_import_piping_curve_clicked()
{
    /* read text from csv file, store to csv model */
    QString filename;
    QFileDialog fileDialog(this);
    filename = fileDialog.getOpenFileName(this,"Open Piping Curve file",QDir::currentPath(),"*.csv");
    if(filename=="")
        return;
    QxtCsvModel csvModel(this);
    csvModel.setSource(filename,false,',',nullptr);

    if(csvModel.rowCount() < 2 || csvModel.columnCount() < 5)
    {
        QMessageBox::information(NULL, "Warning", "Please check CSV file integrity!");
        return;
    }

    /* get point number */
    int point_numbers;
    PipeCurveType curve_type;

    curve_type = (PipeCurveType) csvModel.text(1, 0).toInt();
    point_numbers = csvModel.text(1, 1).toInt();

    Q_ASSERT(curve_type >= PipeCurveType::STEP_LINE);
    Q_ASSERT(curve_type <= PipeCurveType::QUADRATIC_CURVE);
    Q_ASSERT(point_numbers >= 2);
    Q_ASSERT(point_numbers <= POINTS_MAX);

    /* read pump and motor performance data from csv model */
    PipingTestData piping_data;
    piping_data.curve_type = curve_type;
    piping_data.point_numbers = point_numbers;

    for(int i=0; i<point_numbers; i++)
    {
        piping_data.flow[i] = csvModel.text(i+1, 3).toDouble();
        piping_data.head[i] = csvModel.text(i+1, 4).toDouble();
    }

    /* generate pump model from pump performance data */
    Q_ASSERT(g_current_model > 0);
    Q_ASSERT(g_current_model <= 4);
    QString message(" ");
    if (!ImportPipingModel(piping_data))
    {
        QMessageBox::information(NULL, "Warning", message);
        return;
    }
}


void MainWindow::on_pushButton_plot_piping_curve_clicked()
{
    /* check if piping model imported */
    if(!IsPipingImported())
    {
        QMessageBox::information(NULL,"Warning","Piping curve isn't imported!");
        return;
    }

    //curve data
    double max_x = 0;
    double max_y = 0;
    QVector<double> x(101), y(101);

    //clear graph
    ui->customPlot->clearGraphs();
    ui->customPlot->clearTracerTexts();

    //add graph
    QCPGraph *graph1;
    //bool EOC;
    double flow_step = GetPipingModel().flow_max / 100;
    double flow = 0;
    for (int i=0; i<101; ++i)
    {
        x[i] = flow;
        y[i] = GetPipingHeadlossFromFlow(flow);
        if(y[i]>max_y)
            max_y=y[i];
        flow += flow_step;
    }

    graph1 = ui->customPlot->addGraph(ui->customPlot->xAxis,ui->customPlot->yAxis);
    graph1->setData(x, y);
    graph1->setPen(QColor(Qt::red));
    graph1->setName("piping head loss");

    //set legend
    ui->customPlot->legend->setFont(QFont("微软雅黑",9));
    ui->customPlot->legend->setVisible(true);

    //set axises
    ui->customPlot->xAxis->setLabel("Flow(m3/h)");
    ui->customPlot->yAxis->setLabel("Headloss(m)");

    max_x = x[100];
    ui->customPlot->xAxis->setRange(0, max_x*1.2);
    ui->customPlot->yAxis->setRange(0, max_y*1.2);
    ui->customPlot->xAxis->setLabelFont(QFont("微软雅黑",9));
    ui->customPlot->yAxis->setLabelFont(QFont("微软雅黑",9));
    ui->customPlot->xAxis->setVisible(true);
    ui->customPlot->yAxis->setVisible(true);

    //set plot
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                    QCP::iSelectLegend | QCP::iSelectPlottables);
    //add tracer text
    ui->customPlot->addTracerText(QColor(0,0,0),QFont("微软雅黑",9));
    ui->customPlot->addTracerText(graph1->pen().color(),QFont("微软雅黑",9));

    //add tracer
    ui->customPlot->addTracer();

    //test data point
    double test_data_x[POINTS_MAX], test_data_y[POINTS_MAX];
    QPen pen(Qt::red, 1);
    QBrush brush(Qt::red);
    double x_radius = (ui->customPlot->xAxis->pixelToCoord(1)-ui->customPlot->xAxis->pixelToCoord(0))*3;
    double y_radius = (ui->customPlot->yAxis->pixelToCoord(0)-ui->customPlot->yAxis->pixelToCoord(1))*3;
    double *test_data_y_source;

    test_data_y_source = GetPipingTestData().head;

    for (int i=0; i<POINTS_MAX; i++)
    {
        test_data_x[i] = GetPipingTestData().flow[i];
        test_data_y[i] = test_data_y_source[i];
        m_test_point_ellipse[i]->setPen(pen);
        m_test_point_ellipse[i]->setBrush(brush);
        m_test_point_ellipse[i]->setVisible(false);
        m_test_point_ellipse[i]->topLeft->setType(QCPItemPosition::ptPlotCoords);
        m_test_point_ellipse[i]->bottomRight->setType(QCPItemPosition::ptPlotCoords);
        m_test_point_ellipse[i]->topLeft->setCoords(test_data_x[i] - x_radius, test_data_y[i] + y_radius);
        m_test_point_ellipse[i]->bottomRight->setCoords(test_data_x[i] + x_radius, test_data_y[i] - y_radius);
        m_test_point_ellipse[i]->setVisible(true);
    }

    //replot
    ui->customPlot->replot();

}


void MainWindow::on_pushButton_show_import_result_clicked()
{
    //ui->plainTextEdit_import_result->clear();
    QString result;
    result += "模型数：";
    result += QString::number(g_model_qty);
    result += "\n";

    result += "总泵数：";
    int pump_qty = 0;
    for(int i=1; i<=g_model_qty; i++)
        pump_qty += g_pump[i].GetPumpQty();
    result += QString::number(pump_qty);
    result += "\n";

    for (int i=1; i<=g_model_qty; i++)
    {
        result += "模型";
        result += QString::number(i);
        result += "泵数：";
        result += QString::number(g_pump[i].GetPumpQty());
        if (g_pump[i].IsPumpMotorImported())
            result += ", 已导入";
        else
            result += ", 未导入";
        result += "\n";
    }

    result += "管路系统模型：";
    if (IsPipingImported())
        result += "已导入";
    else
        result += "未导入";
    result += "\n";

    ui->plainTextEdit_import_result->setPlainText(result);

}

void MainWindow::on_pushButton_energy_saving_calculation_clicked()
{
    /* pre condition */
    double flow = ui->lineEdit_target_flow->text().toDouble();
    double head = ui->lineEdit_target_head->text().toDouble();
    if (flow <= 0 || head <= 0)
    {
        QMessageBox::information(NULL, "Warning", "流量或扬程不能为0！");
    }

    if(!IsPipingImported())
    {
        QMessageBox::information(NULL,"Warning","Piping curve isn't imported!");
        return;
    }

    EnergySavingResult final_result[4];
    if (!BestEnergyCombination(flow, head, final_result)) {
        QMessageBox::information(NULL, "Warning", "流量扬程超范围！");
        return;
    }

    QString result_string;
    result_string += QString("总流量：") + QString::number(flow, 'f', 0) + QString(" m3/h\n");
    result_string += QString("扬程：") + QString::number(head, 'f', 0) + QString(" m\n");
    for (int i=0; i<4; i++) {
        if (g_pump[i+1].IsPumpMotorImported() && g_pump[i+1].GetPumpQty() > 0) {
            result_string += QString("水泵型号") + QString::number(i+1) + QString("：\n");
            result_string += QString("  运行数量：") + QString::number(final_result[i].pump_qty) + QString(" 台\n");
            result_string += QString("  单台流量：") + QString::number(final_result[i].flow, 'f', 1) + QString(" m3/h\n");
            result_string += QString("  运行频率：") + QString::number(final_result[i].Hz, 'f', 1) + QString(" Hz\n");
            result_string += QString("  单台功率：") + QString::number(final_result[i].power, 'f', 1) + QString(" kW\n");
        }
    }
    ui->plainTextEdit_energy_saving_result->setPlainText(result_string);
}


void MainWindow::on_lineEdit_target_flow_editingFinished()
{
    double flow = ui->lineEdit_target_flow->text().toDouble();

    if (IsPipingImported())
    {
        double headloss = GetPipingHeadlossFromFlow(flow);
        ui->lineEdit_target_head->setText(QString::number(headloss));
    }
}

void MainWindow::plot_efficiency_curve(double *effi, double *head, long start_flow, long stop_flow, long flow_step, double flow_axis_max, double head_axis_max) {

    //clear graph
    ui->customPlot_demo->clearGraphs();
    ui->customPlot_demo->clearTracerTexts();

    //add graph at maximum Hz
    QCPGraph *graph1, *graph2;

    QVector<double> x(256), y(256),y_2(256);

    double flow = start_flow;
    int points = (stop_flow - start_flow) / flow_step;
    if (points > 256)
        return;

    for (int i=0; i<points; i++)
    {
        x[i] = flow;
        y[i] = *(effi + i);
        y_2[i] = *(head + i);
        flow += flow_step;
    }

    graph1 = ui->customPlot_demo->addGraph(ui->customPlot_demo->xAxis,ui->customPlot_demo->yAxis);
    graph1->setData(x, y);
    graph1->setPen(QPen(QBrush(QColor(Qt::blue)), 2, Qt::SolidLine));
    graph1->setName(QString("System efficiency"));

    graph2 = ui->customPlot_demo->addGraph(ui->customPlot_demo->xAxis,ui->customPlot_demo->yAxis2);
    graph2->setData(x, y_2);
    graph2->setPen(QPen(QBrush(QColor(Qt::red)), 2, Qt::SolidLine));
    graph2->setName(QString("Head"));

    //set legend
    ui->customPlot_demo->legend->setFont(QFont("微软雅黑",9));
    ui->customPlot_demo->legend->setVisible(true);

    //set axises
    ui->customPlot_demo->xAxis->setLabel("Flow(m3/h)");
    ui->customPlot_demo->yAxis->setLabel("Efficiency(%)");
    ui->customPlot_demo->yAxis2->setLabel("Head(m)");
    ui->customPlot_demo->xAxis->setLabelColor(QColor(Qt::black));
    ui->customPlot_demo->yAxis->setLabelColor(QColor(Qt::blue));
    ui->customPlot_demo->yAxis2->setLabelColor(QColor(Qt::red));
    ui->customPlot_demo->xAxis->setRange(start_flow, flow_axis_max);
    ui->customPlot_demo->yAxis->setRange(0, 100);
    ui->customPlot_demo->yAxis2->setRange(0, head_axis_max);
    ui->customPlot_demo->xAxis->setLabelFont(QFont("微软雅黑",9));
    ui->customPlot_demo->yAxis->setLabelFont(QFont("微软雅黑",9));
    ui->customPlot_demo->yAxis2->setLabelFont(QFont("微软雅黑",9));
    ui->customPlot_demo->xAxis->setVisible(true);
    ui->customPlot_demo->yAxis->setVisible(true);
    ui->customPlot_demo->yAxis2->setVisible(true);

    //set plot
    ui->customPlot_demo->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                    QCP::iSelectLegend | QCP::iSelectPlottables);
    //add tracer text
    ui->customPlot_demo->addTracerText(QColor(Qt::black),QFont("微软雅黑",9));
    ui->customPlot_demo->addTracerText(QColor(Qt::blue),QFont("微软雅黑",9));
    ui->customPlot_demo->addTracerText(QColor(Qt::red),QFont("微软雅黑",9));

    //add tracer
    ui->customPlot_demo->addTracer();
    ui->customPlot_demo->replot();

}

void MainWindow::on_pushButton_demo_clear_clicked()
{
    m_timer_replot->stop();

    /* update drive type display */
    if (g_pump_total_qty >= 1) {
        if (g_pump[1].GetPumpMotorModel().vfd_installed)
            ui->Edit_drive_type_p1->setText("VFD");
        else
            ui->Edit_drive_type_p1->setText("DOL");
    }
    if (g_pump_total_qty >= 2) {
        if (g_pump[2].GetPumpMotorModel().vfd_installed)
            ui->Edit_drive_type_p2->setText("VFD");
        else
            ui->Edit_drive_type_p2->setText("DOL");
    }
    if (g_pump_total_qty >= 3) {
        if (g_pump[3].GetPumpMotorModel().vfd_installed)
            ui->Edit_drive_type_p3->setText("VFD");
        else
            ui->Edit_drive_type_p3->setText("DOL");
    }
    if (g_pump_total_qty >= 4) {
        if (g_pump[4].GetPumpMotorModel().vfd_installed)
            ui->Edit_drive_type_p4->setText("VFD");
        else
            ui->Edit_drive_type_p4->setText("DOL");
    }
    if (g_pump_total_qty >= 5) {
        if (g_pump[5].GetPumpMotorModel().vfd_installed)
            ui->Edit_drive_type_p5->setText("VFD");
        else
            ui->Edit_drive_type_p5->setText("DOL");
    }
    if (g_pump_total_qty >= 6) {
        if (g_pump[6].GetPumpMotorModel().vfd_installed)
            ui->Edit_drive_type_p6->setText("VFD");
        else
            ui->Edit_drive_type_p6->setText("DOL");
    }
    /* clear all value display */
    ui->Edit_Hz_p1->setText("0.0");
    ui->Edit_Hz_p2->setText("0.0");
    ui->Edit_Hz_p3->setText("0.0");
    ui->Edit_Hz_p4->setText("0.0");
    ui->Edit_Hz_p5->setText("0.0");
    ui->Edit_Hz_p6->setText("0.0");

    ui->Edit_flow_p1->setText("0");
    ui->Edit_flow_p2->setText("0");
    ui->Edit_flow_p3->setText("0");
    ui->Edit_flow_p4->setText("0");
    ui->Edit_flow_p5->setText("0");
    ui->Edit_flow_p6->setText("0");

    ui->Edit_power_p1->setText("0");
    ui->Edit_power_p2->setText("0");
    ui->Edit_power_p3->setText("0");
    ui->Edit_power_p4->setText("0");
    ui->Edit_power_p5->setText("0");
    ui->Edit_power_p6->setText("0");

    ui->Edit_efficiency->setText("0.0");
    ui->Edit_flow_total->setText("0");
    ui->Edit_head->setText("0.0");
    ui->Edit_power_total->setText("0");

    /* clear curves */
    ui->customPlot_demo->clearGraphs();
    ui->customPlot_demo->clearTracerTexts();

}

double effi[256], head[256];
long start_flow;
long stop_flow;
long step_flow;
int plot_progress;

void MainWindow::on_pushButton_demo_start_clicked()
{
    start_flow = ui->lineEdit_flow_start->text().toLong();
    stop_flow = ui->lineEdit_flow_stop->text().toLong();
    if (start_flow < 1 || stop_flow < 1 || start_flow >= stop_flow)
        return;
    step_flow = qMax((stop_flow - start_flow) / 200, 1);
    plot_progress = 0;
    //m_timer_replot->start(1000);
}

void MainWindow::on_timer_replot() {
    return;
    double total_flow;
    double total_power;
    EnergySavingResult calcu_result[4];

    if (plot_progress < 200){
        total_flow = start_flow + step_flow * plot_progress;
        head[plot_progress] = GetPipingHeadlossFromFlow(total_flow);
        BestEnergyCombination(total_flow, head[plot_progress], calcu_result);
        total_power = 0;
        for (int j=0; j<4; j++) {
            total_power += calcu_result[j].power * calcu_result[j].pump_qty;
        }
        effi[plot_progress] = Pump::CalcuEffi(total_flow, head[plot_progress], total_power);
        //qDebug()<<"effi"<<effi[i]<<"flow"<<total_flow<<"head"<<head[i]<<"total_power"<<total_power;
        if (plot_progress < 200) {
            plot_efficiency_curve(effi,
                              head,
                              start_flow,
                              start_flow + step_flow * plot_progress,
                              step_flow,
                              stop_flow * 1.1,
                              60);
        }
        plot_progress++;
    }
}
EnergySavingResult calcu_result[4];
void MainWindow::on_pushButton_test_clicked()
{
    double total_flow;
    double total_power;
    //EnergySavingResult calcu_result[4];

    if (plot_progress < 200){
        total_flow = start_flow + step_flow * plot_progress;
        head[plot_progress] = GetPipingHeadlossFromFlow(total_flow);
        qDebug()<<total_flow<<head[plot_progress];

        BestEnergyCombination(total_flow, head[plot_progress], calcu_result);
        return;
        total_power = 0;

        for (int j=0; j<4; j++) {
            total_power += calcu_result[j].power * calcu_result[j].pump_qty;
        }
        return;
        effi[plot_progress] = Pump::CalcuEffi(total_flow, head[plot_progress], total_power);
        //qDebug()<<"effi"<<effi[i]<<"flow"<<total_flow<<"head"<<head[i]<<"total_power"<<total_power;
        /*if (plot_progress < 200) {
            plot_efficiency_curve(effi,
                              head,
                              start_flow,
                              start_flow + step_flow * plot_progress,
                              step_flow,
                              stop_flow * 1.1,
                              60);
        }*/
        plot_progress++;
    }
}

