#include "chistcurvewidget.h"
#include "ui_chistcurvewidget.h"
#include <QMessageBox>
#include <QtGlobal>
#include <QDebug>
#include <QSemaphore>
#include <QGraphicsDropShadowEffect>
#include <QColor>

#define SERIES_MAX_NUMBERS (5)
#define SCATTER_MARKER_SIZE (1)
#define X_AXIS_TICK_COUNT (5)
#define Y_AXIS_TICK_COUNT (11)
#define Y_AXIS_RANGE_MARGIN    (0.2)

CHistCurveWidget::CHistCurveWidget(QSemaphore &semaphore,
                                   CurveConfig &current_curve,
                                   CInfluxDbQueryThread::QueryUnit &query_unit,
                                   QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CHistCurveWidget),
    ex_semaphore(semaphore),
    ex_current_curve(current_curve),
    ex_query_unit(query_unit)
{

    ui->setupUi(this);
    m_font.setFamily("Microsoft YaHei");
    m_font.setPointSize(11);

    //QPalette pal = window()->palette();
    //pal.setColor(QPalette::Window, QRgb(0x40434a));
    //pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
    //window()->setPalette(pal);

    m_chart = new QChart();
    m_chart->setTheme(QChart::ChartThemeBlueIcy);
    m_chart->setTitleFont(m_font);
    m_chart->setFont(m_font);
    m_chart->legend()->setFont(m_font);

    m_chart_view = new CCustomChartView(this);
    m_chart_view->setRenderHint(QPainter::Antialiasing);
    m_chart_view->setChart(m_chart);
    ui->gridLayout_2->addWidget(m_chart_view);
}

CHistCurveWidget::~CHistCurveWidget()
{
    delete ui;
    m_chart->removeAllSeries();
    foreach(QAbstractAxis *axis, m_chart->axes()) {
        m_chart->removeAxis(axis);
        delete axis;
    }
    delete m_chart;
}

void CHistCurveWidget::ChartInit(const QList<CurveConfig> &para_list) {
    foreach(const CurveConfig &para, para_list) {
        ui->para_to_add->addItem(para.para_name);
        m_para_list.append(para);
    }
    ui->para_to_remove->clear();
    ui->end_time->setDateTime((m_end_time = QDateTime::currentDateTime()));
    ui->begin_time->setDateTime((m_begin_time = ui->end_time->dateTime().addSecs(-7200)));
}

/* add a new curve to list */
void CHistCurveWidget::on_addButton_clicked()
{
    if (m_chart->series().count() >= SERIES_MAX_NUMBERS) {
        QMessageBox::warning(this, "Warning", "最多6条曲线");
        return;
    }
    if (ui->para_to_add->count()<1)
        return;

    /* remove item from list */
    QString text_to_remove = ui->para_to_add->currentText();
    CurveConfig para_pair = parseParaText(text_to_remove);
    int code_to_remove = para_pair.para_code;
    Q_ASSERT(code_to_remove > 0);
    ui->para_to_add->removeItem(ui->para_to_add->currentIndex());

    /* add item to list */
    int index_to_insert = 0;
    for (int i=0; i<ui->para_to_remove->count(); i++) {
        int code = parseParaText(ui->para_to_remove->itemText(i)).para_code;
        Q_ASSERT(code_to_remove > 0);
        if (code_to_remove >= code)
            index_to_insert ++;
        else
            break;
    }
    ui->para_to_remove->insertItem(index_to_insert, text_to_remove);
    ui->para_to_remove->setCurrentIndex(index_to_insert);

    /* */
    addSerie(para_pair);
}

/* delete a exist curve from list */
void CHistCurveWidget::on_removeButton_clicked()
{
    if (ui->para_to_remove->count() < 1)
        return;

    QString text_to_remove = ui->para_to_remove->currentText();
    CurveConfig para_pair = parseParaText(text_to_remove);
    int code_to_remove = para_pair.para_code;
    Q_ASSERT(code_to_remove > 0);
    ui->para_to_remove->removeItem(ui->para_to_remove->currentIndex());

    int index_to_insert = 0;
    for (int i=0; i<ui->para_to_add->count(); i++) {
        int code = parseParaText(ui->para_to_add->itemText(i)).para_code;
        Q_ASSERT(code_to_remove > 0);
        if (code_to_remove >= code)
            index_to_insert ++;
        else
            break;
    }
    ui->para_to_add->insertItem(index_to_insert, text_to_remove);
    ui->para_to_add->setCurrentIndex(index_to_insert);

    removeSerie(para_pair);
}

void CHistCurveWidget::on_refreshButton_clicked()
{
    if (m_begin_time >= m_end_time) {
        QMessageBox::warning(this, "Warning", "结束时间不能比开始时间早！");
        return;
    }

    //QDateTimeAxis *axisX;
    if (!m_chart->axes(Qt::Horizontal).empty()) {
        ((QDateTimeAxis*)m_chart->axes(Qt::Horizontal).at(0))->setRange(m_begin_time, m_end_time);
        //axisX->setRange(m_begin_time, m_end_time);
    }
    for (int i=0; i<ui->para_to_remove->count(); i++) {
        int code = parseParaText(ui->para_to_remove->itemText(i)).para_code;
        Q_ASSERT(code > 0);

        emit requestForData(code);
    }
    m_chart_view->saveAxisRange();
}

void  CHistCurveWidget::addSerie(CurveConfig para) {
    if (m_chart->series().count() >= SERIES_MAX_NUMBERS)
        return;
    if (para.para_code < 1)
        return;

    /* add x axis */
    QDateTimeAxis *axisX;
    if (m_chart->axes(Qt::Horizontal).empty()) {
        axisX = new QDateTimeAxis(this);
        Q_ASSERT(axisX);
        axisX->setTickCount(X_AXIS_TICK_COUNT);
        axisX->setFormat("yy-MM-dd hh:mm:ss");
        axisX->setLabelsFont(m_font);
        axisX->setRange(m_begin_time, m_end_time);
        m_chart->addAxis(axisX,Qt::AlignBottom);
    } else {
        axisX = (QDateTimeAxis*)m_chart->axes(Qt::Horizontal).first();
        axisX->setRange(m_begin_time, m_end_time);
    }

    /* add y axis */
    QValueAxis *axisY;
    axisY = new QValueAxis(this);
    Q_ASSERT(axisY);
    axisY->setTickCount(Y_AXIS_TICK_COUNT);
    axisY->setLabelFormat("%.0f " + para.unit);
    axisY->setLabelsFont(m_font);
    axisY->setRange(para.range_low, para.range_high);
    m_chart->addAxis(axisY, Qt::AlignLeft);
    QScatterSeries *serie = new QScatterSeries(m_chart);
    Q_ASSERT(serie);
    m_chart->addSeries(serie);
    serie->setName(para.para_name);
    serie->setMarkerSize(SCATTER_MARKER_SIZE);
    QColor color = color_list.nextColor();
    serie->setColor(color);
    serie->setBorderColor(color);
    serie->attachAxis(axisX);
    serie->attachAxis(axisY);

    m_chart_view->saveAxisRange();
m_chart->setTheme(QChart::ChartThemeBlueIcy);
    emit requestForData(para.para_code);
}

void  CHistCurveWidget::removeSerie(CurveConfig para) {
    if (para.para_code < 1)
        return;
    if (m_chart->series().empty())
        return;
    foreach(QAbstractSeries *serie, m_chart->series()) {
        if (serie->name() == para.para_name) {
            foreach(QAbstractAxis *axis, serie->attachedAxes()) {
                if (axis->alignment() == Qt::AlignLeft) {
                    m_chart->removeAxis(axis);
                    delete axis;
                    break;
                }
            }
            m_chart->removeSeries(serie);
            delete serie;
            break;
        }
    }
}

void CHistCurveWidget::on_begin_time_dateTimeChanged(const QDateTime &dateTime)
{
    m_begin_time = dateTime;
}

void CHistCurveWidget::on_end_time_dateTimeChanged(const QDateTime &dateTime)
{
    m_end_time = dateTime;
}

CHistCurveWidget::CurveConfig CHistCurveWidget::parseParaText(QString text) {
    CurveConfig para_to_return;
    para_to_return.para_code = 0;
    foreach (const CurveConfig &para, m_para_list) {
        if (para.para_name == text) {
            para_to_return = para;
        }
    }
    return para_to_return;
}

void CHistCurveWidget::on_restoreAxisRange_clicked()
{
    m_chart_view->restoreAxisRange();
}


void CHistCurveWidget::on_autoRange_clicked()
{
    foreach(QAbstractSeries *serie, m_chart->series()) {
        double yMax = std::numeric_limits<double>::min();
        double yMin = std::numeric_limits<double>::max();
        QScatterSeries *sc_serie = static_cast<QScatterSeries*>(serie);
        foreach(const QPointF &p, sc_serie->points()) {
            if (p.y() > yMax)
                yMax = p.y();
            if (p.y() < yMin)
                yMin = p.y();
        }
        double margin = (yMax-yMin) * Y_AXIS_RANGE_MARGIN;
        yMax += margin;
        yMin -= margin;

        foreach(QAbstractAxis *axis, sc_serie->attachedAxes()) {
            if (axis->alignment() == Qt::AlignLeft) {
                axis->setRange(yMin, yMax);
                break;
            }
        }
    }
}


void CHistCurveWidget::on_exportCsv_clicked()
{
    QString head_line;
    int max_rows = 0;

    foreach(QAbstractSeries *serie, m_chart->series()) {
        QScatterSeries *sc_serie = static_cast<QScatterSeries*>(serie);
        head_line += (sc_serie->name() + "时间" + "," + sc_serie->name() + "数值" + ",");
        max_rows = qMax(max_rows, sc_serie->count());
    }
    if (head_line.endsWith(",")) {
        head_line[head_line.count() - 1] = '\n';
    }

    QVector<QString> *content = new QVector<QString>(max_rows);
    Q_ASSERT(content);
    if (content->count() >= max_rows) {
        foreach(QAbstractSeries *serie, m_chart->series()) {
            QScatterSeries *sc_serie = static_cast<QScatterSeries*>(serie);
            for (int i=0; i<sc_serie->points().count(); i++) {
                QPointF p = sc_serie->points().at(i);
                QString row_str;
                row_str = (QDateTime::fromMSecsSinceEpoch(p.x())).toString("yyyy/MM/dd hh:mm:ss") + ",";
                row_str += QString::number(p.y()) + ",";
                (*content)[i] += row_str;
            }
            for (int i=sc_serie->points().count(); i<max_rows; i++) {
                (*content)[i] += ", ,";
            }
        }
    } else
        return;
    for (int i=0; i<content->count(); i++) {
        if ((*content)[i].endsWith(",")) {
            int cnt = (*content)[i].count();
            (*content)[i][cnt-1] = '\n';
        }
    }

    QString fileName = QFileDialog::getSaveFileName(this, tr("CSV file"),
                                                    QApplication::applicationDirPath(),
                                                    tr("Files (*.csv)"));
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write(head_line.toUtf8().toStdString().c_str());
        for (int i = 0; i < content->count(); i++) {
            file.write(content->at(i).toStdString().c_str());
        }
        file.close();
    };
    delete content;
}

void CHistCurveWidget::on_curve_update() {
    foreach (QAbstractSeries *serie, m_chart->series()) {
        if (serie->name() == ex_current_curve.para_name) {
             ((QScatterSeries*)serie)->replace(ex_query_unit.points);
            break;
        }
    }
    ex_semaphore.release();
}
