#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pump.h"
#include "piping.h"
#include <QComboBox>
#include "qcustomplot.h"
#include "global.h"


extern Pump g_pump[10];
extern int  g_model_qty;
extern int  g_current_model;
extern CurveChoose g_pump_curve_choose;
extern SensorlessCurveChoose g_sensorless_curve_choose;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->comboBox_pumpmodel_qty, SIGNAL(currentTextChanged(const QString&)), this, SLOT(on_comboBox_pumpmodel_qty_currentIndexChanged(const QString&)));
    connect(ui->comboBox_model_number, SIGNAL(currentTextChanged(const QString&)), this, SLOT(on_comboBox_model_number_currentIndexChanged(const QString&)));
    connect(ui->comboBox_pump_qty, SIGNAL(currentTextChanged(const QString&)), this, SLOT(on_comboBox_pump_qty_currentIndexChanged(const QString&)));
    connect(ui->comboBox_choose_pump_curve, SIGNAL(currentTextChanged(const QString&)), this, SLOT(on_comboBox_choose_pump_curve_currentTextChange(const QString&)));
    connect(ui->lineEdit_Hz, SIGNAL(editingFinished()), this, SLOT(on_lineEdit_Hz_editingFinished()));
    connect(ui->comboBox_model_number_2, SIGNAL(currentTextChanged(const QString&)), this, SLOT(on_comboBox_pump_number_2_currentTextChanged(const QString&)));
    connect(ui->comboBox_choose_sensorless_curve, SIGNAL(currentTextChanged(const QString&)), this, SLOT(on_comboBox_choose_pump_curve_currentTextChange(const QString&)));
    connect(ui->comboBox_model_number_3, SIGNAL(currentTextChanged(const QString&)), this, SLOT(on_comboBox_pump_number_3_currentTextChanged(const QString&)));

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

    if(csvModel.rowCount() < 11 || csvModel.columnCount() < 3)
    {
        QMessageBox::information(NULL, "Warning", "Please check CSV file integrity!");
        return;
    }

    /* read pump and motor performance data from csv model */
    PipingTestData piping_data;
    for(int i=0; i<POINTS_MAX; i++)
    {
        piping_data.flow[i] = csvModel.text(i+1, 1).toDouble();
        piping_data.head[i] = csvModel.text(i+1, 2).toDouble();
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


void MainWindow::on_pushButton_SameModelCompare_clicked()
{
    /* check if piping model imported */
    if(!IsPipingImported())
    {
        QMessageBox::information(NULL,"Warning","Piping curve isn't imported!");
        return;
    }

    /* check flow range */
    double flow_begin = ui->lineEdit_flow_begin->text().toDouble();
    double flow_end = ui->lineEdit_flow_end->text().toDouble();
    if (flow_begin <0 || flow_end <=0 || flow_end <= flow_begin)
        QMessageBox::information(NULL, "Warning", "检查流量起终点设置！");


    /* calculation */
    double flow[20];
    double head[20];
    double power[3][20];
    double Hz[3][20];

    EnergyWithSameModel(flow_begin, flow_end, flow, head, &power[0][0], &Hz[0][0]);

    /* store to a csv file */
    QVector<QStringList> *csvData;
    csvData = new QVector<QStringList>;
    QStringList row;

    row.clear();
    row.append("flow");
    row.append("head");
    row.append("power1");
    row.append("power2");
    row.append("power3");
    row.append("Hz1");
    row.append("Hz2");
    row.append("Hz3");
    csvData->append(row);

    for (int i=0; i<20; i++) {
        row.clear();
        row.append(QString::number(flow[i]));
        row.append(QString::number(head[i]));
        for (int n=0; n<3; n++)
            row.append(QString::number(power[n][i]));
        for (int n=0; n<3; n++)
            row.append(QString::number(Hz[n][i]));
        csvData->append(row);
    }

    QString filename;
    QFileDialog fileDialog(this);
    filename = fileDialog.getSaveFileName(this,"导出结果到Csv文件",QDir::currentPath(),"*.csv");
    if(filename=="")
        return;
    QxtCsvModel csvModel(this);
    csvModel.toCSV(filename);
    csvModel.setSource(filename);
    csvModel.insertRows(0,csvData->size());
    csvModel.insertColumns(0,8);
    for(int i=0; i<csvData->size(); i++)
        for(int j=0; j<8; j++)
            csvModel.setText(i,j,csvData->at(i).at(j));
    csvModel.toCSV(filename);
    delete csvData;

    QMessageBox::information(nullptr,"导出","导出完成！");
}


void MainWindow::on_pushButton_DiffModelCompare_clicked()
{
    /* check if piping model imported */
    if(!IsPipingImported())
    {
        QMessageBox::information(NULL,"Warning","Piping curve isn't imported!");
        return;
    }

    /* check flow range */
    double flow_begin = ui->lineEdit_flow_begin->text().toDouble();
    double flow_end = ui->lineEdit_flow_end->text().toDouble();
    if (flow_begin <0 || flow_end <=0 || flow_end <= flow_begin)
        QMessageBox::information(NULL, "Warning", "检查流量起终点设置！");

    /* calculation */
    double flow[20];
    double head[20];
    double power[2][20];
    double Hz[2][20];

    EnergyWithDiffModel(flow_begin, flow_end, flow, head, &power[0][0], &Hz[0][0]);

    /* store to a csv file */
    QVector<QStringList> *csvData;
    csvData = new QVector<QStringList>;
    QStringList row;

    row.clear();
    row.append("flow");
    row.append("head");
    row.append("power1");
    row.append("power2");
    row.append("Hz1");
    row.append("Hz2");

    csvData->append(row);

    for (int i=0; i<20; i++) {
        row.clear();
        row.append(QString::number(flow[i]));
        row.append(QString::number(head[i]));
        for (int n=0; n<2; n++)
            row.append(QString::number(power[n][i]));
        for (int n=0; n<2; n++)
            row.append(QString::number(Hz[n][i]));
        csvData->append(row);
    }

    QString filename;
    QFileDialog fileDialog(this);
    filename = fileDialog.getSaveFileName(this,"导出结果到Csv文件",QDir::currentPath(),"*.csv");
    if(filename=="")
        return;
    QxtCsvModel csvModel(this);
    csvModel.toCSV(filename);
    csvModel.setSource(filename);
    csvModel.insertRows(0,csvData->size());
    csvModel.insertColumns(0,8);
    for(int i=0; i<csvData->size(); i++)
        for(int j=0; j<6; j++)
            csvModel.setText(i,j,csvData->at(i).at(j));
    csvModel.toCSV(filename);
    delete csvData;

    QMessageBox::information(nullptr,"导出","导出完成！");

}


void MainWindow::on_pushButton_DiffFreqCompare_clicked()
{
    /* check if piping model imported */
    if(!IsPipingImported())
    {
        QMessageBox::information(NULL,"Warning","Piping curve isn't imported!");
        return;
    }

    /* check flow range */
    double flow_begin = ui->lineEdit_flow_begin->text().toDouble();
    double flow_end = ui->lineEdit_flow_end->text().toDouble();
    if (flow_begin <0 || flow_end <=0 || flow_end <= flow_begin)
        QMessageBox::information(NULL, "Warning", "检查流量起终点设置！");

    /* calculation */
    double flow[20];
    double head[20];
    double power[2][20];
    double Hz[2][20];

    double model_1_Hz = ui->lineEdit_model_1_freq->text().toDouble();
    if (model_1_Hz <25 || model_1_Hz >50)
        QMessageBox::information(NULL, "Warning", "型号1频率超范围！");

    EnergyWithDiffFreq(flow_begin, flow_end, flow, head, &power[0][0], &Hz[0][0], model_1_Hz);

    /* store to a csv file */
    QVector<QStringList> *csvData;
    csvData = new QVector<QStringList>;
    QStringList row;

    row.clear();
    row.append("flow");
    row.append("head");
    row.append("power1");
    row.append("power2");
    row.append("Hz1");
    row.append("Hz2");

    csvData->append(row);

    for (int i=0; i<20; i++) {
        row.clear();
        row.append(QString::number(flow[i]));
        row.append(QString::number(head[i]));
        for (int n=0; n<2; n++)
            row.append(QString::number(power[n][i]));
        for (int n=0; n<2; n++)
            row.append(QString::number(Hz[n][i]));
        csvData->append(row);
    }

    QString filename;
    QFileDialog fileDialog(this);
    filename = fileDialog.getSaveFileName(this,"导出结果到Csv文件",QDir::currentPath(),"*.csv");
    if(filename=="")
        return;
    QxtCsvModel csvModel(this);
    csvModel.toCSV(filename);
    csvModel.setSource(filename);
    csvModel.insertRows(0,csvData->size());
    csvModel.insertColumns(0,8);
    for(int i=0; i<csvData->size(); i++)
        for(int j=0; j<6; j++)
            csvModel.setText(i,j,csvData->at(i).at(j));
    csvModel.toCSV(filename);
    delete csvData;

    QMessageBox::information(nullptr,"导出","导出完成！");
}


void MainWindow::on_pushButton_DiffModelCompare_2_clicked()
{
    /* check if piping model imported */
    if(!IsPipingImported())
    {
        QMessageBox::information(NULL,"Warning","Piping curve isn't imported!");
        return;
    }

    /* check flow range */
    double flow_begin = ui->lineEdit_flow_begin->text().toDouble();
    double flow_end = ui->lineEdit_flow_end->text().toDouble();
    if (flow_begin <0 || flow_end <=0 || flow_end <= flow_begin)
        QMessageBox::information(NULL, "Warning", "检查流量起终点设置！");

    /* calculation */
    double flow[20];
    double head[20];
    double power[2][20];
    double Hz[2][20];

    EnergyWithDiffModel2(flow_begin, flow_end, flow, head, &power[0][0], &Hz[0][0]);

    /* store to a csv file */
    QVector<QStringList> *csvData;
    csvData = new QVector<QStringList>;
    QStringList row;

    row.clear();
    row.append("flow");
    row.append("head");
    row.append("power1");
    row.append("power2");
    row.append("Hz1");
    row.append("Hz2");

    csvData->append(row);

    for (int i=0; i<20; i++) {
        row.clear();
        row.append(QString::number(flow[i]));
        row.append(QString::number(head[i]));
        for (int n=0; n<2; n++)
            row.append(QString::number(power[n][i]));
        for (int n=0; n<2; n++)
            row.append(QString::number(Hz[n][i]));
        csvData->append(row);
    }

    QString filename;
    QFileDialog fileDialog(this);
    filename = fileDialog.getSaveFileName(this,"导出结果到Csv文件",QDir::currentPath(),"*.csv");
    if(filename=="")
        return;
    QxtCsvModel csvModel(this);
    csvModel.toCSV(filename);
    csvModel.setSource(filename);
    csvModel.insertRows(0,csvData->size());
    csvModel.insertColumns(0,6);
    for(int i=0; i<csvData->size(); i++)
        for(int j=0; j<6; j++)
            csvModel.setText(i,j,csvData->at(i).at(j));
    csvModel.toCSV(filename);
    delete csvData;

    QMessageBox::information(nullptr,"导出","导出完成！");
}

