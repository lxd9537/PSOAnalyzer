#ifndef MYCUSTOMPLOT_H
#define MYCUSTOMPLOT_H

#include "qcustomplot.h"

class MyCustomPlot : public QCustomPlot
{
public:
    MyCustomPlot(QWidget *parent);
    ~MyCustomPlot();
    void AddTracerText();
    QCPItemText *TracerText(int i);
    void clearTracers();
protected:
    virtual void            mouseMoveEvent (QMouseEvent *event);
    double                  m_x;
    double                  m_y;
    QCPItemTracer           *m_tracer;
    QVector<QCPItemText*>   m_tracer_text;
};

#endif // MYCUSTOMPLOT_H
