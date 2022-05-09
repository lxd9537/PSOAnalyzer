#include "cperformcurvewidget.h"
#include "ui_cperformcurvewidget.h"
#include <QMessageBox>
#include <QtGlobal>
#include <QDebug>
#include <QSemaphore>
#include <QGraphicsDropShadowEffect>
#include <QColor>
#include <QPixmap>

#define SERIES_MAX_NUMBERS (10)
#define SCATTER_MARKER_SIZE (8)
#define X_AXIS_TICK_COUNT (11)
#define Y_AXIS_TICK_COUNT (11)
#define Y_AXIS_RANGE_MARGIN    (0.2)

CPerfomCurveWidget::CPerfomCurveWidget(CMySqlThread *mysql_thread, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CPerformCurveWidget)
{
    ui->setupUi(this);
    m_font.setFamily("Microsoft YaHei");
    m_font.setPointSize(11);

    this->setFont(m_font);

    m_chart = new QChart();
    m_chart->setTheme(QChart::ChartThemeBlueIcy);
    m_chart->setTitleFont(m_font);
    m_chart->setFont(m_font);
    m_chart->legend()->setFont(m_font);

    ui->chartView->setParent(this);
    ui->chartView->setRenderHint(QPainter::Antialiasing);
    ui->chartView->setChart(m_chart);

    /* model list */
    m_mysql_thread = mysql_thread;
    if (m_mysql_thread->getConnectStatus()) {
        QStringList model_list;
        m_mysql_thread->readPumpModelList(model_list);
        ui->modelList->addItems(model_list);
    }
}

CPerfomCurveWidget::~CPerfomCurveWidget()
{
    delete ui;
    m_chart->removeAllSeries();
    foreach(QAbstractAxis *axis, m_chart->axes()) {
        m_chart->removeAxis(axis);
        delete axis;
    }
    delete m_chart;
}

void  CPerfomCurveWidget::addSerie(const CurveType &curve_type, const ValueCurveData &curve_data) {
    if (m_chart->series().count() >= SERIES_MAX_NUMBERS) {
        QMessageBox::about(this, "信息", "最多显示10条曲线");
        return;
    }

    bool axisY_exists = false;
    QValueAxis *axisY;
    foreach(QAbstractAxis *axis, m_chart->axes()) {
        if (axis->titleText() == curve_data.axisY_title) {
            axisY_exists = true;
            axisY = (QValueAxis*)axis;
        }
    }
    if (!axisY_exists) {
        axisY = new QValueAxis(this);
        axisY->setTickCount(Y_AXIS_TICK_COUNT);
        axisY->setLabelFormat("%.0f " + curve_data.axisY_unit);
        axisY->setLabelsFont(m_font);
        axisY->setTitleText(curve_data.axisY_title);
        axisY->setRange(curve_data.range_low, curve_data.range_high);
        m_chart->addAxis(axisY, Qt::AlignLeft);
    }

    if (curve_type == SCATTER_CURVE) {
        QScatterSeries *serie = new QScatterSeries(m_chart);
        m_chart->addSeries(serie);
        serie->setName(curve_data.legend);
        serie->setMarkerSize(SCATTER_MARKER_SIZE);
        //serie->setColor(color_list.nextColor());
        serie->setBorderColor(serie->color());
        serie->attachAxis(m_chart->axes(Qt::Horizontal).at(0));
        serie->attachAxis(axisY);
        serie->replace(curve_data.points);
        serie->setPointLabelsVisible(true);
        connect(serie, &QSplineSeries::pressed, ui->chartView, &CCustomChartView::on_point_pressed);
    } else if (curve_type == SPLINE_CURVE) {
        QSplineSeries *serie = new QSplineSeries(m_chart);
        m_chart->addSeries(serie);
        serie->setName(curve_data.legend);
        serie->setColor(color_list.nextColor());
        serie->attachAxis(m_chart->axes(Qt::Horizontal).at(0));
        serie->attachAxis(axisY);
        serie->replace(curve_data.points);
        connect(serie, &QSplineSeries::pressed, ui->chartView, &CCustomChartView::on_point_pressed);
    }
}

/* 将图表截屏 */
void CPerfomCurveWidget::on_exportImg_clicked(){
    QPixmap p = ui->chartView->grab();
    QImage image = p.toImage();

    QString filename = QFileDialog::getSaveFileName(this, tr("PNG file"),
                                                       QApplication::applicationDirPath(),
                                                    tr("Files (*.png)"));
    image.save(filename);
}

void CPerfomCurveWidget::on_refreshButton_clicked()
{
    /* 删除现有曲线 */
    m_chart->removeAllSeries();

    /* 得到水泵数据 */
    CPump::PerformTestData perform_data;
    QString error_message;

    if (!m_mysql_thread->readPumpData(ui->modelList->currentText(), perform_data)) {
        QMessageBox::warning(this, "Warning", "从数据库读水泵性能数据失败");
        return;
    }

    /* 水泵建模 */
    if (!m_pump.createModel(perform_data, &error_message)) {
        QMessageBox::warning(this, "Warning", error_message);
        return;
    }

    /* 没有曲线被选中 */
    if (!(ui->testDataHead->isChecked()
            ||ui->testDataEffi->isChecked()
            ||ui->testDataPower->isChecked()
            ||ui->testDataNpshr->isChecked()
            ||ui->polyHead->isChecked()
            ||ui->polyEffi->isChecked()
            ||ui->polyPower->isChecked()
            ||ui->polyNpshr->isChecked())) {
        return;
    }

    /* 添加 x 轴 */
    QValueAxis *axisX;
    if (m_chart->axes(Qt::Horizontal).empty()) {
        axisX = new QValueAxis(this);
        axisX->setTickCount(X_AXIS_TICK_COUNT);
        axisX->setLabelFormat("%.0fm3/h");
        axisX->setLabelsFont(m_font);
        axisX->setTitleText("流量");
        axisX->setRange(0, m_pump.getPerformData().flow_max);
        m_chart->addAxis(axisX, Qt::AlignBottom);
    } else {
        axisX = (QValueAxis*)m_chart->axes(Qt::Horizontal).at(0);
        axisX->setRange(0, m_pump.getPerformData().flow_max);
    }

    /* 添加测试数据散点图 */
    addPerformDataCurve();

    /* 添加拟合曲线 */
    addPolyfitCurve();
}

void CPerfomCurveWidget::addPerformDataCurve() {
    if (!m_pump.isModelCreated())
        return;

    if (ui->testDataHead->isChecked()) {
        ValueCurveData curve_data;
        curve_data.axisY_title = "扬程";
        curve_data.axisY_unit = "m";
        curve_data.legend = "扬程测试数据";
        curve_data.range_low = 0;
        curve_data.range_high = m_pump.getPerformData().head[0] * 1.3;
        for (int i=0; i<m_pump.getPerformData().points; i++) {
            QPointF point(QString::number(m_pump.getPerformData().flow[i],'f', 1).toFloat(),
                          QString::number(m_pump.getPerformData().head[i], 'f', 1).toFloat());
            curve_data.points.append(point);
        }
        CPerfomCurveWidget::addSerie(SCATTER_CURVE, curve_data);
    }

    if (ui->testDataEffi->isChecked()) {
        ValueCurveData curve_data;
        curve_data.axisY_title = "效率";
        curve_data.axisY_unit = "%";
        curve_data.legend = "效率测试数据";
        curve_data.range_low = 0;
        curve_data.range_high = 100;
        for (int i=0; i<m_pump.getPerformData().points; i++) {
            QPointF point(QString::number(m_pump.getPerformData().flow[i], 'f', 1).toFloat(),
                          QString::number(m_pump.getPerformData().effi[i], 'f', 1).toFloat());
            curve_data.points.append(point);
        }
        CPerfomCurveWidget::addSerie(SCATTER_CURVE, curve_data);
    }

    if (ui->testDataPower->isChecked()) {
        ValueCurveData curve_data;
        curve_data.axisY_title = "功率";
        curve_data.axisY_unit = "kW";
        curve_data.legend = "功率测试数据";
        curve_data.range_low = 0;
        curve_data.range_high = m_pump.getPerformData().power[m_pump.getPerformData().points - 1] * 1.3;
        for (int i=0; i<m_pump.getPerformData().points; i++) {
            QPointF point(QString::number(m_pump.getPerformData().flow[i], 'f', 1).toFloat(),
                          QString::number(m_pump.getPerformData().power[i], 'f', 1).toFloat());
            curve_data.points.append(point);
        }
        CPerfomCurveWidget::addSerie(SCATTER_CURVE, curve_data);
    }

    if (ui->testDataNpshr->isChecked()) {
        ValueCurveData curve_data;
        curve_data.axisY_title = "NPSHr";
        curve_data.axisY_unit = "m";
        curve_data.legend = "NPSHr测试数据";
        curve_data.range_low = 0;
        curve_data.range_high = m_pump.getPerformData().npshr[m_pump.getPerformData().points - 1] * 1.3;
        for (int i=0; i<m_pump.getPerformData().points; i++) {
            QPointF point(QString::number(m_pump.getPerformData().flow[i], 'f', 1).toFloat(),
                          QString::number(m_pump.getPerformData().npshr[i], 'f', 1).toFloat());
            curve_data.points.append(point);
        }
        CPerfomCurveWidget::addSerie(SCATTER_CURVE, curve_data);
    }
}

void CPerfomCurveWidget::addPolyfitCurve() {
    if (!m_pump.isModelCreated())
        return;

    float speed;
    if (ui->vfdCurveDisply->isChecked())
        speed = ui->speed->value();
    else
        speed = 100;

    if (ui->polyHead->isChecked()) {
        ValueCurveData curve_data;
        curve_data.axisY_title = "扬程";
        curve_data.axisY_unit = "m";
        curve_data.legend = "扬程拟合曲线";
        curve_data.range_low = 0;
        curve_data.range_high = m_pump.getPerformData().head[0] * 1.3;
        float flow = m_pump.getPerformData().flow_min * speed / 100;
        float step = (m_pump.getPerformData().flow_max * speed / 100 - flow) / 100;
        for (int i = 0; i < 100; i++) {
            CPump::CalculateResult result;
            QPointF point(flow, m_pump.calcuHead(flow, speed, result));
            if (result == CPump::MAX_FLOW_EXCEEDED)
                break;
            curve_data.points.append(point);
            flow += step;
        }
        CPerfomCurveWidget::addSerie(SPLINE_CURVE, curve_data);
    }

    if (ui->polyEffi->isChecked()) {
        ValueCurveData curve_data;
        curve_data.axisY_title = "效率";
        curve_data.axisY_unit = "%";
        curve_data.legend = "效率拟合曲线";
        curve_data.range_low = 0;
        curve_data.range_high = 100;
        float flow = m_pump.getPerformData().flow_min * speed / 100;
        float step = (m_pump.getPerformData().flow_max * speed / 100 - flow) / 100;
        for (int i = 0; i < 100; i++) {
            CPump::CalculateResult result;
            QPointF point(flow, m_pump.calcuEffi(flow, speed, result));
            if (result == CPump::MAX_FLOW_EXCEEDED)
                break;
            curve_data.points.append(point);
            flow += step;
        }
        CPerfomCurveWidget::addSerie(SPLINE_CURVE, curve_data);
    }
    if (ui->polyPower->isChecked()) {
        ValueCurveData curve_data;
        curve_data.axisY_title = "功率";
        curve_data.axisY_unit = "kW";
        curve_data.legend = "功率拟合曲线";
        curve_data.range_low = 0;
        curve_data.range_high = m_pump.getPerformData().power[m_pump.getPerformData().points - 1] * 1.3;

        float flow = m_pump.getPerformData().flow_min * speed / 100;
        float step = (m_pump.getPerformData().flow_max * speed / 100 - flow) / 100;
        for (int i = 0; i < 100; i++) {
            CPump::CalculateResult result;
            QPointF point(flow, m_pump.calcuPower(flow, speed, result));
            if (result == CPump::MAX_FLOW_EXCEEDED)
                break;
            curve_data.points.append(point);
            flow += step;
        }
        CPerfomCurveWidget::addSerie(SPLINE_CURVE, curve_data);
    }
    if (ui->polyNpshr->isChecked()) {
        ValueCurveData curve_data;
        curve_data.axisY_title = "NPSHr";
        curve_data.axisY_unit = "m";
        curve_data.legend = "NPSHr拟合曲线";
        curve_data.range_low = 0;
        curve_data.range_high = m_pump.getPerformData().npshr[m_pump.getPerformData().points - 1] * 1.3;

        float flow = m_pump.getPerformData().flow_min * speed / 100;
        float step = (m_pump.getPerformData().flow_max * speed / 100 - flow) / 100;
        for (int i = 0; i < 100; i++) {
            CPump::CalculateResult result;
            QPointF point(flow, m_pump.calcuNpshr(flow, speed, result));
            if (result == CPump::MAX_FLOW_EXCEEDED)
                break;
            curve_data.points.append(point);
            flow += step;
        }
        CPerfomCurveWidget::addSerie(SPLINE_CURVE, curve_data);
    }
}


void CPerfomCurveWidget::on_vfdCurveDisply_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked) {
        ui->speed->setReadOnly(false);
        ui->speed->setDisabled(false);
    } else {
        ui->speed->setValue(100);
        ui->speed->setReadOnly(true);
        ui->speed->setDisabled(true);
    }
}

