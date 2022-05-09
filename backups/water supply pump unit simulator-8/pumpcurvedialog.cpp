#include "pumpcurvedialog.h"
#include "ui_pumpcurvedialog.h"

PumpCurveDialog::PumpCurveDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PumpCurveDialog)
{
    ui->setupUi(this);
    m_system_profile = new PumpUnit::SysProfile;
    m_pump_perform = new Pump::PerformData[6];
}

PumpCurveDialog::~PumpCurveDialog()
{
    delete ui;
    delete m_system_profile;
    delete m_pump_perform;
}

void PumpCurveDialog::SetSystemProfile(PumpUnit::SysProfile system_profile)
{
    *m_system_profile = system_profile;
}
void PumpCurveDialog::SetPumpPerform(int pump_number, Pump::PerformData pump_perform)
{
    if(pump_number < 6 && pump_number >=0)
        m_pump_perform[pump_number] = pump_perform;
}

void PumpCurveDialog::Initiation(bool curve_content)
{
    if(curve_content)
        ShowIndividualCurve();
    else
        ShowCombineCurve();

}
void PumpCurveDialog::ShowIndividualCurve()
{

    ui->customPlot->setGeometry(0,0,640,480);
    ui->customPlot->xAxis->setLabel("Flow (m3 / h)");
    ui->customPlot->yAxis->setLabel("Head (m)");

    // pump curve
    QVector<double> x(101), y(101);
    double max_head = 0;
    double max_flow = 0;

    QColor color[6]={
        QColor(200,30,30),      //red
        QColor(30,200,30),      //green
        QColor(30,30,200),      //blue
        QColor(200,150,30),     //orange
        QColor(150,75,150),     //purple
        QColor(0,0,0)};         //black

    ui->customPlot->clearGraphs();
    //ui->customPlot->clearTracers();

    for(int j=0; j<m_system_profile->total_installed; j++)
    {
        for (int i=0; i<101; ++i)
        {
            x[i] = m_pump_perform[j].flow_max / 100 * i;
            y[i] = m_pump_perform[j].Kfactor_head_from_flow[3]*pow(x[i],3)
                    + m_pump_perform[j].Kfactor_head_from_flow[2]*pow(x[i],2)
                    + m_pump_perform[j].Kfactor_head_from_flow[1]*x[i]
                    + m_pump_perform[j].Kfactor_head_from_flow[0];
        }
        ui->customPlot->addGraph();
        ui->customPlot->graph(j)->setData(x, y);
        ui->customPlot->graph(j)->setPen(color[j]);
        ui->customPlot->graph(j)->setName("Pump "+QString::number(j+1));

        ui->customPlot->AddTracerText();
        ui->customPlot->TracerText(j)->setPositionAlignment(Qt::AlignLeft);
        ui->customPlot->TracerText(j)->setFont(QFont(font().family(), 9));
        ui->customPlot->TracerText(j)->setPen(QPen(color[j]));

        if(m_pump_perform[j].head_at_zero_flow > max_head)
            max_head = m_pump_perform[j].head_max;
        if(m_pump_perform[j].flow_max > max_flow)
            max_flow = m_pump_perform[j].flow_max;
    }

    ui->customPlot->xAxis->setRange(0, max_flow*1.2);
    ui->customPlot->yAxis->setRange(0, max_head*1.2);
    ui->customPlot->legend->setVisible(true);

}
void PumpCurveDialog::ShowCombineCurve()
{
    double max_y=0;
    double max_x=0;
    ui->customPlot->setGeometry(0,0,640,480);
    ui->customPlot->xAxis->setLabel("Flow (m3 / h)");
    ui->customPlot->yAxis->setLabel("Head (m)");

    // pump curve
    QVector<double> x(101), y(101);

    QColor color[7]={
        QColor(200,30,30),      //red
        QColor(30,200,30),      //green
        QColor(30,30,200),      //blue
        QColor(200,150,30),     //orange
        QColor(150,75,150),     //purple
        QColor(0,0,0),          //black
        QColor(200,30,175),     //pink
        };

    ui->customPlot->clearGraphs();
    //ui->customPlot->clearTracers();

    for(int j=0; j<m_system_profile->total_installed; j++)
    {
        for (int i=0; i<101; ++i)
        {
            double s_flow = m_pump_perform[0].flow_max / 100 * i;
            x[i] = s_flow * (j+1);
            y[i] = m_pump_perform[0].Kfactor_head_from_flow[3]*pow(s_flow,3)
                    + m_pump_perform[0].Kfactor_head_from_flow[2]*pow(s_flow,2)
                    + m_pump_perform[0].Kfactor_head_from_flow[1]*s_flow
                    + m_pump_perform[0].Kfactor_head_from_flow[0];
        }
        ui->customPlot->addGraph();
        ui->customPlot->graph(j)->setData(x, y);
        ui->customPlot->graph(j)->setPen(color[j]);
        ui->customPlot->graph(j)->setName(QString::number(j+1) + "-Pump");

        ui->customPlot->AddTracerText();
        ui->customPlot->TracerText(j)->setPositionAlignment(Qt::AlignLeft);
        ui->customPlot->TracerText(j)->setFont(QFont(font().family(), 9));
        ui->customPlot->TracerText(j)->setPen(QPen(color[j]));
        ui->customPlot->TracerText(j)->setPadding(QMargins(2,2,2,2));
    }

    max_y = m_pump_perform[0].head_max;
    max_x = m_pump_perform[0].flow_max * m_system_profile->total_installed;

    for (int i=0; i<101; ++i)
    {
        x[i] = m_system_profile->duty_flow_total / 100 * i;
        y[i] = pow(x[i],2) * m_system_profile->Kfactor_head_from_flow[1] + m_system_profile->Kfactor_head_from_flow[0];
        y[i] = y[i] * 10;
    }

    if(y[100]>max_y)
        max_y=y[100];
    if(x[100]>max_x)
        max_x=x[100];

    ui->customPlot->addGraph();
    ui->customPlot->graph(m_system_profile->total_installed)->setData(x, y);
    ui->customPlot->graph(m_system_profile->total_installed)->setPen(color[m_system_profile->total_installed]);
    ui->customPlot->graph(m_system_profile->total_installed)->setName("System");

    ui->customPlot->AddTracerText();
    ui->customPlot->TracerText(m_system_profile->total_installed)->setPositionAlignment(Qt::AlignLeft);
    ui->customPlot->TracerText(m_system_profile->total_installed)->setFont(QFont(font().family(), 9));
    ui->customPlot->TracerText(m_system_profile->total_installed)->setPadding(QMargins(2,2,2,2));

    ui->customPlot->TracerText(m_system_profile->total_installed)->setPen(QPen(color[m_system_profile->total_installed]));
    ui->customPlot->xAxis->setRange(0, max_x*1.2);
    ui->customPlot->yAxis->setRange(0, max_y*1.2);
    ui->customPlot->legend->setVisible(true);
}

