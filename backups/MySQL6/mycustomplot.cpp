#include "mycustomplot.h"

MyCustomPlot::MyCustomPlot(QWidget *parent):
    QCustomPlot (parent)
{
    m_tracer = new QCPItemTracer(this);
    m_load_point_x = new QVector<double>();
    m_load_point_y = new QVector<double>();
    m_load_point_text = new QVector<QString>();

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

    //display load point information
    if(m_plot_vf_curve&&m_load_point_x&&m_load_point_y&&m_phaseTracerArrow&&m_ellipse&&m_point_text)
    {
        m_point_text->setVisible(false);
        m_phaseTracerArrow->setVisible(false);
        m_ellipse->setVisible(false);

        if(hitTest(event->pos().x()))
        {
            QPointF point = event->pos();
            point.setY(20);
            m_phaseTracerArrow->end->setParentAnchor(m_ellipse->center);
            m_phaseTracerArrow->end->setCoords(4, 4);
            m_phaseTracerArrow->setTail(QCPLineEnding(QCPLineEnding::esBar, (m_point_text->bottom->pixelPosition().y() - m_point_text->top->pixelPosition().y())*0.85));

            m_ellipse->topLeft->setType(QCPItemPosition::ptAbsolute);
            m_ellipse->bottomRight->setType(QCPItemPosition::ptAbsolute);
            m_ellipse->topLeft->setPixelPosition(QPointF(xAxis->coordToPixel(m_load_point_x->at(m_hit_index))-6,
                                                         yAxis->coordToPixel(m_load_point_y->at(m_hit_index))+6));
            m_ellipse->bottomRight->setPixelPosition(QPointF(xAxis->coordToPixel(m_load_point_x->at(m_hit_index))+6,
                                                             yAxis->coordToPixel(m_load_point_y->at(m_hit_index))-6));

            m_point_text->setVisible(true);
            m_phaseTracerArrow->setVisible(true);
            m_ellipse->setVisible(true);
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

void MyCustomPlot::setLoadPoints(
        QVector<double> &points_x,
        QVector<double> &points_y,
        QVector<QString> &points_text,
        QColor color)
{
    m_load_point_x->clear();
    m_load_point_y->clear();
    m_load_point_text->clear();

    for(int i=0;i<points_x.size();i++)
    {
        m_load_point_x->append(points_x.at(i));
        m_load_point_y->append(points_y.at(i));
        m_load_point_text->append(points_text.at(i));
    }

    if(!m_point_text)
    {
        m_point_text = new QCPItemText(this);
        m_point_text->setTextAlignment(Qt::AlignLeft);
        m_point_text->setPositionAlignment(Qt::AlignBottom);
        m_point_text->setFont(QFont("微软雅黑",9));
        m_point_text->setPen(Qt::NoPen);
        m_point_text->position->setType(QCPItemPosition::ptAxisRectRatio);
        m_point_text->position->setCoords(0.5,1);
        m_point_text->setPadding(QMargins(5,2,2,5));
        m_point_text->setVisible(false);
    }
    if(!m_phaseTracerArrow)
    {
        m_phaseTracerArrow = new QCPItemCurve(this);
        m_phaseTracerArrow->start->setParentAnchor(m_point_text->left);
        m_phaseTracerArrow->startDir->setParentAnchor(m_phaseTracerArrow->start);
        m_phaseTracerArrow->startDir->setCoords(-40, 0);
        m_phaseTracerArrow->endDir->setParentAnchor(m_phaseTracerArrow->end);
        m_phaseTracerArrow->endDir->setCoords(30, 30);
        m_phaseTracerArrow->setHead(QCPLineEnding::esSpikeArrow);
        m_phaseTracerArrow->setVisible(false);
    }

    if(!m_ellipse)
    {
        m_ellipse = new QCPItemEllipse(this);
        m_ellipse->setPen(QPen(color));
        m_ellipse->setBrush(color);
        m_ellipse->setVisible(false);
    }
}

bool MyCustomPlot::hitTest(double x)
{
    for(int i=0; i<m_load_point_x->size(); i++)
    {
        if(abs(x-xAxis->coordToPixel(m_load_point_x->at(i)))<3)
        {
            m_hit_index = i;
            m_point_text->setText(m_load_point_text->at(i));
            return true;
        }
    }
    return false;
}

void MyCustomPlot::SetPlotVfCurve(bool plot_vf_curve)
{
    m_plot_vf_curve = plot_vf_curve;

}
