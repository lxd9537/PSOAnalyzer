#ifndef CHistCurveWidget_H
#define CHistCurveWidget_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QLegend>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtCharts/QValueAxis>
#include <QtCharts/QDateTimeAxis>
#include <QDateTime>
#include <QDate>
#include <QTime>
#include <QFileDialog>
#include <Curves/ColorQueue/ccolorqueue.h>
#include <Database/InfluxDb/cinfluxdbquerythread.h>
#include <Curves/ChartView/ccustomchartview.h>
#include "ui_chistcurvewidget.h"

using namespace QtCharts;

namespace Ui {
class Ui_CHistCurveWidget;
}

class CHistCurveWidget : public QWidget
{
    Q_OBJECT
public:
    typedef struct {
        int para_code;
        QString para_name;
        QString unit;
        double range_low;
        double range_high;
        QString measurement;
        QString tag_key;
        QString tag_value;
        QString field;
    } CurveConfig;
    typedef struct {
        int para_code;
        QList<QPointF> points;
    } CurveData;

public:
    explicit CHistCurveWidget(QSemaphore &semaphore,
                              CurveConfig &current_curve,
                              CInfluxDbQueryThread::QueryUnit &query_unit,
                              QWidget *parent = nullptr);
    ~CHistCurveWidget();
    void ChartInit(const QList<CurveConfig> &para_pair_list);
    QDateTime getBeginTime() {return m_begin_time;};
    QDateTime getEndTime() {return m_end_time;};

signals:
    void eventReport(int id, int event_code, QString event_text);
    void requestForData(int para_code);

private slots:
    void on_addButton_clicked();
    void on_removeButton_clicked();
    void on_refreshButton_clicked();
    void on_restoreAxisRange_clicked();
    void on_autoRange_clicked();
    void on_exportCsv_clicked();
    void on_begin_time_dateTimeChanged(const QDateTime &dateTime);
    void on_end_time_dateTimeChanged(const QDateTime &dateTime);

public slots:
    void on_curve_update();

private:
    Ui::CHistCurveWidget *ui;
    QChart *m_chart;
    QFont m_font;
    QDateTime m_begin_time;
    QDateTime m_end_time;
    QList<CurveConfig> m_para_list;
    CColorQueue color_list;
    QSemaphore &ex_semaphore;
    CurveConfig &ex_current_curve;
    CInfluxDbQueryThread::QueryUnit &ex_query_unit;
    CCustomChartView *m_chart_view;

    void addSerie(CurveConfig para);
    void removeSerie(CurveConfig para);
    CHistCurveWidget::CurveConfig parseParaText(QString text);
};

#endif // CHistCurveWidget_H
