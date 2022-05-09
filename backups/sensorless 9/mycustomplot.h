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

    void clearCrossPointText();

    void setVfdCrossPoints(
            QVector<double> *points_x[2],
            QVector<double> *points_y[2],
            QVector<QString> *points_text[2],
            QColor *color[2],
            bool enable[2]);

    bool VfdCrossPointHitTest(double x, int curveIndex);

protected:
    virtual void            mouseMoveEvent (QMouseEvent *event);
    double                   m_x;
    double                   m_y;
    QCPItemTracer           *m_tracer = nullptr;
    QVector<QCPItemText*>    m_tracer_text;

    int                      m_vfd_cross_hit_index[2] = {0,0};
    bool                     m_vfd_cross_curve_enabled[2] = {false,false};
    QVector<double>         *m_vfd_cross_point_x[2] = {nullptr,nullptr};
    QVector<double>         *m_vfd_cross_point_y[2] = {nullptr,nullptr};
    QCPItemText             *m_vfd_cross_point_text[2] = {nullptr,nullptr};
    QVector<QString>        *m_vfd_cross_point_text_list[2] = {nullptr,nullptr};
    QCPItemCurve            *m_vfd_cross_arrow[2] = {nullptr,nullptr};
    QCPItemEllipse          *m_vfd_cross_ellipse[2] = {nullptr,nullptr};

};

#endif // MYCUSTOMPLOT_H
