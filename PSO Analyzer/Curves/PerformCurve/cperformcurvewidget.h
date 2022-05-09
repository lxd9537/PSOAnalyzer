#ifndef CPERFORMCURVEWIDGET_H
#define CPERFORMCURVEWIDGET_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QSplineSeries>
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
#include <QFileDialog>
#include <Curves/ColorQueue/ccolorqueue.h>
#include <Database/MySql/cmysqlthread.h>
#include <PumpSystem/Pump/cpump.h>
#include "ui_cperformcurvewidget.h"

using namespace QtCharts;

namespace Ui {
class Ui_CPerformCurveWidget;
}

class CPerfomCurveWidget : public QWidget
{
    Q_OBJECT
public:
    typedef struct {
        QString legend;
        QString axisY_title;
        QString axisY_unit;
        double range_low;
        double range_high;
        QList<QPointF> points;
    } ValueCurveData;

    typedef enum {
        SCATTER_CURVE,
        SPLINE_CURVE
    } CurveType;

public:
    explicit CPerfomCurveWidget(CMySqlThread *mysql_thread, QWidget *parent = nullptr);
    ~CPerfomCurveWidget();

private slots:
    void on_vfdCurveDisply_stateChanged(int arg1);
    void on_refreshButton_clicked();
    void on_exportImg_clicked();

private:
    Ui::CPerformCurveWidget *ui;
    QChart *m_chart;

    CColorQueue color_list;
    QFont m_font;

    CMySqlThread *m_mysql_thread;
    CPump m_pump;

    void addSerie(const CurveType &curve_type, const ValueCurveData &curve_data);

    void addPerformDataCurve();
    void addPolyfitCurve();
};

#endif // CPerfomCurveWidget_H
