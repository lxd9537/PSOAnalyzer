#include "mycustomplot.h"

MyCustomPlot::MyCustomPlot(QWidget *parent):
    QCustomPlot (parent)
{
    m_tracer = new QCPItemTracer(this);

    for(int i=0;i<2;i++){
        m_vfd_cross_point_x[i] = new QVector<double>;
        m_vfd_cross_point_y[i] = new QVector<double>;
        m_vfd_cross_point_text[i] = new QCPItemText(this);
        m_vfd_cross_point_text_list[i] = new QVector<QString>;
        m_vfd_cross_arrow[i] = new QCPItemCurve(this);
        m_vfd_cross_ellipse[i] = new QCPItemEllipse(this);

        m_vfd_cross_point_text[i]->setVisible(false);
        m_vfd_cross_arrow[i]->setVisible(false);
        m_vfd_cross_ellipse[i]->setVisible(false);
    }

    xAxis->grid()->setSubGridVisible(true);
    yAxis->grid()->setSubGridVisible(true);

}
MyCustomPlot::~MyCustomPlot()
{
}
void MyCustomPlot::mouseMoveEvent (QMouseEvent *event)
{

    //update position of tracer
    if(m_tracer!=nullptr)
    {
        m_x=xAxis->pixelToCoord(event->pos().x());
        m_y=yAxis->pixelToCoord(event->pos().y());
        m_tracer->position->setCoords(m_x,0);
    }

    //update value and position of the tracer text
    int pt_x,pt_y;
    pt_x = legend->outerRect().right();
    pt_y = legend->outerRect().bottom()+8;

    if(m_tracer_text.size()>0)
    {
        m_tracer_text.at(0)->setText(QString::number(m_x));
        QPointF point = event->pos();
        point.setY(8);
        m_tracer_text.at(0)->position->setPixelPosition(point);
        m_tracer_text.at(0)->setTextAlignment(Qt::AlignLeft);
        m_tracer_text.at(0)->setPositionAlignment(Qt::AlignLeft| Qt::AlignTop);
    }

    for(int i=1;i<m_tracer_text.size();i++)
    {
        double value = graph(i-1)->data()->findBegin(m_x)->mainValue();
        m_tracer_text.at(i)->setText(QString::number(value));
        m_tracer_text.at(i)->position->setCoords(pt_x,pt_y);
        pt_y = m_tracer_text.at(i)->bottom->pixelPosition().toPoint().y()+2;
    }

    //display Vfd cross point
    for(int j=0;j<2;j++){
        if(m_vfd_cross_curve_enabled[j])
        {
            m_vfd_cross_point_text[j]->setVisible(false);
            m_vfd_cross_arrow[j]->setVisible(false);
            m_vfd_cross_ellipse[j]->setVisible(false);

            if(VfdCrossPointHitTest(event->pos().x(),j))
            {
                QPointF point = event->pos();
                point.setY(20);

                m_vfd_cross_ellipse[j]->topLeft->setType(QCPItemPosition::ptAbsolute);
                m_vfd_cross_ellipse[j]->bottomRight->setType(QCPItemPosition::ptAbsolute);
                m_vfd_cross_ellipse[j]->topLeft->setPixelPosition(
                            QPointF(xAxis->coordToPixel(m_vfd_cross_point_x[j]->at(m_vfd_cross_hit_index[j]))-6,
                            yAxis->coordToPixel(m_vfd_cross_point_y[j]->at(m_vfd_cross_hit_index[j]))+6));

                m_vfd_cross_arrow[j]->end->setParentAnchor(m_vfd_cross_ellipse[j]->center);
                m_vfd_cross_arrow[j]->end->setCoords(4, 4);
                m_vfd_cross_arrow[j]->setTail(QCPLineEnding(QCPLineEnding::esBar,
                    (m_vfd_cross_point_text[j]->bottom->pixelPosition().y() - m_vfd_cross_point_text[j]->top->pixelPosition().y())*0.85));

                m_vfd_cross_ellipse[j]->bottomRight->setPixelPosition(
                            QPointF(xAxis->coordToPixel(m_vfd_cross_point_x[j]->at(m_vfd_cross_hit_index[j]))+6,
                            yAxis->coordToPixel(m_vfd_cross_point_y[j]->at(m_vfd_cross_hit_index[j]))-6));

                m_vfd_cross_point_text[j]->setVisible(true);
                m_vfd_cross_arrow[j]->setVisible(true);
                m_vfd_cross_ellipse[j]->setVisible(true);
            }
        }
        else {
            m_vfd_cross_point_text[j]->setVisible(false);
            m_vfd_cross_arrow[j]->setVisible(false);
            m_vfd_cross_ellipse[j]->setVisible(false);
        }
    }

    this->replot(QCustomPlot::rpQueuedReplot);
    QCustomPlot::mouseMoveEvent(event);

}
QCPItemText *MyCustomPlot::addTracerText(QColor color,QFont font)
{
    QCPItemText *text = new QCPItemText(this);
    m_tracer_text.append(text);

    text->setPadding(QMargins(2,2,2,2));
    text->setPen(Qt::NoPen);
    text->setFont(font);
    text->setColor(color);
    text->setTextAlignment(Qt::AlignRight);
    text->setPositionAlignment(Qt::AlignRight| Qt::AlignTop);
    text->position->setType(QCPItemPosition::ptAbsolute);
    text->setVisible(true);

    return text;
}
void MyCustomPlot::clearTracerTexts()
{
    for(int i=0;i< m_tracer_text.size();i++)
        removeItem(m_tracer_text.at(i));
    m_tracer_text.clear();
}

QCPItemTracer *MyCustomPlot::addTracer()
{
    if(m_tracer==nullptr)
    {
        m_tracer = new QCPItemTracer(this);
        m_tracer->setVisible(true);
    }
    return m_tracer;
}

void MyCustomPlot::clearTracer()
{
    if(m_tracer!=nullptr)
        removeItem(m_tracer);
    m_tracer = nullptr;
}

void MyCustomPlot::clearCrossPointText()
{
    for(int i=0;i<2;i++){
        m_vfd_cross_point_text[i]->setVisible(false);
        m_vfd_cross_arrow[i]->setVisible(false);
        m_vfd_cross_ellipse[i]->setVisible(false);
    }
}

void MyCustomPlot::setVfdCrossPoints(
        QVector<double> *points_x[2],
        QVector<double> *points_y[2],
        QVector<QString> *points_text[2],
        QColor *color[2],
        bool enable[2])
{
    for(int j=0;j<2;j++){
        m_vfd_cross_curve_enabled[j] = enable[j];
        if(!enable[j]) continue;

        m_vfd_cross_point_x[j]->clear();
        m_vfd_cross_point_y[j]->clear();
        m_vfd_cross_point_text_list[j]->clear();

        for(int i=0;i<points_x[j]->size();i++)
        {
            m_vfd_cross_point_x[j]->append(points_x[j]->at(i));
            m_vfd_cross_point_y[j]->append(points_y[j]->at(i));
            m_vfd_cross_point_text_list[j]->append(points_text[j]->at(i));
        }

        m_vfd_cross_point_text[j]->setTextAlignment(Qt::AlignLeft);
        m_vfd_cross_point_text[j]->setPositionAlignment(Qt::AlignBottom);
        m_vfd_cross_point_text[j]->setFont(QFont("微软雅黑",9));
        m_vfd_cross_point_text[j]->setPen(Qt::NoPen);
        m_vfd_cross_point_text[j]->position->setType(QCPItemPosition::ptAxisRectRatio);
        m_vfd_cross_point_text[j]->position->setCoords(0.6+j*0.2,1);
        m_vfd_cross_point_text[j]->setPadding(QMargins(5,2,2,5));

        m_vfd_cross_arrow[j]->start->setParentAnchor(m_vfd_cross_point_text[j]->left);
        m_vfd_cross_arrow[j]->startDir->setParentAnchor(m_vfd_cross_arrow[j]->start);
        m_vfd_cross_arrow[j]->startDir->setCoords(-40, 0);
        m_vfd_cross_arrow[j]->endDir->setParentAnchor(m_vfd_cross_arrow[j]->end);
        m_vfd_cross_arrow[j]->endDir->setCoords(30, 30);
        m_vfd_cross_arrow[j]->setHead(QCPLineEnding::esSpikeArrow);

        m_vfd_cross_ellipse[j]->setPen(QPen(*color[j]));
        m_vfd_cross_ellipse[j]->setBrush(*color[j]);
    }
}

bool MyCustomPlot::VfdCrossPointHitTest(double x, int curveIndex)
{
    if (curveIndex < 0 || curveIndex > 1) return false;

    if(m_vfd_cross_curve_enabled[curveIndex]){
        for(int i=0; i<20; i++)
        {
            if(abs(x-xAxis->coordToPixel(m_vfd_cross_point_x[curveIndex]->at(i)))<3)
            {
                m_vfd_cross_hit_index[curveIndex] = i;
                m_vfd_cross_point_text[curveIndex]->setText(m_vfd_cross_point_text_list[curveIndex]->at(i));
                return true;
            }
        }
    }
    return false;
}
