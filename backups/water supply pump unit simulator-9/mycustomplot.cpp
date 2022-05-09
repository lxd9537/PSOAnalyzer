#include "mycustomplot.h"

MyCustomPlot::MyCustomPlot(QWidget *parent):
    QCustomPlot (parent)
{

    m_tracer = new QCPItemTracer(this);
    m_tracer->setVisible(true);


}
MyCustomPlot::~MyCustomPlot()
{
    delete m_tracer;
    for(int i=0; i<m_tracer_text.size();i++)
    {
        delete m_tracer_text.at(i);
        m_tracer_text.remove(m_tracer_text.size()-1);
    }

}
void MyCustomPlot::mouseMoveEvent (QMouseEvent *event)
{
    //positioning of tracer
    m_x=xAxis->pixelToCoord(event->pos().x());
    m_y=yAxis->pixelToCoord(event->pos().y());
    m_tracer->position->setCoords(m_x,0);
    m_tracer->updatePosition();

    //positioning of tag text
    double y_position = this->yAxis->range().upper;
    double x_posision = m_x;
    for(int i=0;i<m_tracer_text.size()-1;i++)
    {
        m_tracer_text.at(i)->position->setType(QCPItemPosition::ptPlotCoords);
        double value = graph(i)->data()->findBegin(m_x)->mainValue();
        m_tracer_text.at(i)->setText(QString::number(value));
        double height = m_tracer_text.at(i)->font().pointSize();

         if(y_position <= height)
        {
            y_position = height;
            x_posision += 40;
        }
        m_tracer_text.at(i)->position->setCoords(x_posision,y_position);

        y_position -= height;
    }
    y_position = m_tracer_text.last()->font().pointSize();
    m_tracer_text.last()->position->setType(QCPItemPosition::ptPlotCoords);
    m_tracer_text.last()->setText(QString::number(m_x));
    m_tracer_text.last()->position->setCoords(x_posision,y_position);

    this->replot(QCustomPlot::rpQueuedReplot);
    QCustomPlot::mouseMoveEvent(event);

}
QCPItemText *MyCustomPlot::AddTracerText()
{
    QCPItemText *text = new QCPItemText(this);
    text->setPadding(QMargins(2,2,2,2));
    m_tracer_text.append(text);
    return text;
}
QCPItemText *MyCustomPlot::TracerText(int i)
{
    if(i<0||i>m_tracer_text.size())
        return nullptr;
    return m_tracer_text.at(i);
}
void MyCustomPlot::clearTracers()
{
    if(m_tracer_text.size()<1)
        return;
    QCPItemText *text;
    foreach(text,m_tracer_text)
        delete text;
    m_tracer_text.clear();
}
