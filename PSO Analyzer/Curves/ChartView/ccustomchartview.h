#ifndef CCUSTOMCHARTVIEW_H
#define CCUSTOMCHARTVIEW_H

#include <QChartView>
#include <QObject>
#include <QValueAxis>
#include <QDateTimeAxis>
#include <QDateTime>
#include <QPointF>
#include <QDebug>
#include <QGuiApplication>
#include <QScatterSeries>
#include <QMouseEvent>
#include <QPen>

QT_CHARTS_USE_NAMESPACE

class CCustomChartView : public QChartView
{
    Q_OBJECT
public:
    CCustomChartView(QWidget *parent);
    void saveAxisRange();
    void restoreAxisRange();
public slots:
    void on_point_pressed(const QPointF &point);

private:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

private:
    QPoint m_press_pos;
    bool m_alreadySaveRange = false;
    QDateTime m_xMinDT, m_xMaxDT;
    double    m_xMin, m_xMax;
    QList<double> m_yMin, m_yMax;
    QGraphicsSimpleTextItem* m_point_value_display = nullptr;
    QGraphicsItem *m_coord_cross = nullptr;
};

#endif // CCustomChartView_H
