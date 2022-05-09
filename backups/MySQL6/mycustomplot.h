#ifndef MYCUSTOMPLOT_H
#define MYCUSTOMPLOT_H

#include "qcustomplot.h"

class MyCustomPlot : public QCustomPlot
{
public:
    MyCustomPlot(QWidget *parent);
    ~MyCustomPlot();

    QCPItemTracer *addTracer();
    void clearTracer();

    QCPItemText *addTracerText(QColor color,QFont font);
    void clearTracerTexts();
    void setLoadPoints(QVector<double> &points_x, QVector<double> &points_y, QVector<QString> &points_text,QColor color);
    bool hitTest(double x);
    void SetPlotVfCurve(bool plot_vf_curve);

protected:
    virtual void            mouseMoveEvent (QMouseEvent *event);
    double                  m_x;
    double                  m_y;
    QCPItemTracer           *m_tracer=nullptr;
    QCPItemText             *m_point_text=nullptr;
    QVector<QCPItemText*>    m_tracer_text;
    QVector<double>         *m_load_point_x=nullptr;
    QVector<double>         *m_load_point_y=nullptr;
    QVector<QString>        *m_load_point_text=nullptr;
    int                      m_hit_index;
    QCPItemCurve            *m_phaseTracerArrow=nullptr;
    QCPItemEllipse          *m_ellipse=nullptr;
    bool                     m_plot_vf_curve;

};

#endif // MYCUSTOMPLOT_H
