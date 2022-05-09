#include "dialog_showloadprofile.h"
#include "ui_dialog_showloadprofile.h"

Dialog_ShowLoadProfile::Dialog_ShowLoadProfile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_ShowLoadProfile)
{
    ui->setupUi(this);
    m_bar = new QCPBars(ui->widget->xAxis,ui->widget->yAxis);
    for(int i=0;i<20;i++)
    {
       m_value_text[i] = new QCPItemText(ui->widget);
       m_value_text[i]->setPositionAlignment(Qt::AlignHCenter | Qt::AlignBottom);
       m_value_text[i]->setPadding(QMargins(2,2,2,2));
    }
}

Dialog_ShowLoadProfile::~Dialog_ShowLoadProfile()
{
    delete ui;
    delete m_bar;
}
void Dialog_ShowLoadProfile::setData(QStandardItemModel *loadprofileData, QString &app)
{
    QVector<double> x(20);
    QVector<double> y(20);
    double max_y=0;

    for(int i=0;i<20;i++)
    {
        x[i]=0;
        y[i]=0;
    }

    for(int i=0;i<loadprofileData->rowCount();i++)
    {
        int load = loadprofileData->item(i,1)->text().toInt();

        if(load >= 0 && load <= 100)
        {
            x[load/5-1] = load;
            y[load/5-1] = loadprofileData->item(i,2)->text().toDouble();
        }
        if(y[load/5-1]>max_y)
            max_y =y[load/5-1];
    }
    m_bar->setAntialiased(false);
    m_bar->setPen(QPen(QColor(0, 168, 140).lighter(130)));
    m_bar->setBrush(QColor(0, 168, 140));
    m_bar->setWidth(5);
    m_bar->setData(x, y);
    ui->widget->xAxis->setRange(0,105);
    ui->widget->yAxis->setRange(0,max_y*1.5);

    ui->widget->xAxis->setLabel("load(%)");
    ui->widget->yAxis->setLabel("hours");

    for(int i=0;i<20;i++)
    {
       m_value_text[i]->setText(QString::number(y[i]));
       m_value_text[i]->position->setPixelPosition(
                   QPointF(ui->widget->xAxis->coordToPixel(x[i]),
                           ui->widget->yAxis->coordToPixel(y[i])));
       m_value_text[i]->setVisible(y[i]>0.01);
    }

    ui->widget->replot();

    this->setWindowTitle(app+" load profile");

}
