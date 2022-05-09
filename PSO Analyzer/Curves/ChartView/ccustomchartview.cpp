#include "ccustomchartview.h"

CCustomChartView::CCustomChartView(QWidget *parent):QChartView(parent)
{
    QFont font("Microsoft YaHei", 11);
    this->setFont(font);
    this->setCursor(QCursor(Qt::CrossCursor));

}

void CCustomChartView::mousePressEvent(QMouseEvent *event)
{
    /* no any series to return */
    if (this->chart()->series().count()<1) {
        QChartView::mousePressEvent(event);
        return;
    }

    /* middle key for move */
    if (event->button() == Qt::MiddleButton || event->button() == Qt::LeftButton) {
       m_press_pos = event->pos();
    }
    QChartView::mousePressEvent(event);
}

void CCustomChartView::mouseMoveEvent(QMouseEvent *event)
{
    if (this->chart()->series().count()<1)
        return;

    if (m_point_value_display) {
        m_point_value_display->hide();
    }

    const QPoint curPos = event->pos();

    if (event->buttons() & Qt::MiddleButton) {
        QPoint offset = curPos - m_press_pos;
        m_press_pos = curPos;
        this->chart()->scroll(-offset.x(), offset.y());
    }
    QChartView::mouseMoveEvent(event);
}

void CCustomChartView::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_point_value_display) {
        m_point_value_display->hide();
    }
    QChartView::mouseReleaseEvent(event);
}

void CCustomChartView::wheelEvent(QWheelEvent *event)
{
    if ((this->chart()->axes(Qt::Vertical).empty())
            || (this->chart()->axes(Qt::Horizontal).empty()))
        return;

    const QPointF curPos = event->position();

    const double factor = 1.5;//缩放比例
    if ((QGuiApplication::keyboardModifiers() == Qt::ControlModifier)) {
        for (int i=0; i<this->chart()->axes(Qt::Vertical).count(); i++) {
            QPointF curVal = this->chart()->mapToValue(QPointF(curPos), this->chart()->series().at(i));
            QValueAxis *axisY = dynamic_cast<QValueAxis*>(this->chart()->axes(Qt::Vertical).at(i));
            const double yMin = axisY->min();
            const double yMax = axisY->max();
            const double yCentral = curVal.y();
            double bottomOffset;
            double topOffset;
            if (event->angleDelta().y() > 0) {//放大
                bottomOffset = 1.0 / factor * (yCentral - yMin);
                topOffset = 1.0 / factor * (yMax - yCentral);
            } else {//缩小
                bottomOffset = 1.0 * factor * (yCentral - yMin);
                topOffset = 1.0 * factor * (yMax - yCentral);
            }
            axisY->setRange(yCentral - bottomOffset, yCentral + topOffset);
        }
    } else {
        QPointF curVal = this->chart()->mapToValue(QPointF(curPos), this->chart()->series().first());
        if (this->chart()->axes(Qt::Horizontal).first()->inherits("QtCharts::QDateTimeAxis")) {
            QDateTimeAxis *axisX = dynamic_cast<QDateTimeAxis*>(this->chart()->axes(Qt::Horizontal).first());
            const double xMin = axisX->min().toSecsSinceEpoch();
            const double xMax = axisX->max().toSecsSinceEpoch();
            const double xCentral = curVal.x() / 1000; //from msec to sec

            double leftOffset;
            double rightOffset;
            if (event->angleDelta().y() > 0) {//放大
                leftOffset = (xCentral - xMin)/factor;
                rightOffset = (xMax - xCentral)/factor;
            } else {//缩小
                leftOffset = factor * (xCentral - xMin);
                rightOffset = factor * (xMax - xCentral);
            }
            axisX->setRange(QDateTime::fromSecsSinceEpoch(xCentral - leftOffset),
                            QDateTime::fromSecsSinceEpoch(xCentral + rightOffset));
        } else {
            QValueAxis *axisX = dynamic_cast<QValueAxis*>(this->chart()->axes(Qt::Horizontal).first());
            const double xMin = axisX->min();
            const double xMax = axisX->max();
            const double xCentral = curVal.x() / 1000; //from msec to sec

            double leftOffset;
            double rightOffset;
            if (event->angleDelta().y() > 0) {//放大
                leftOffset = (xCentral - xMin)/factor;
                rightOffset = (xMax - xCentral)/factor;
            } else {//缩小
                leftOffset = factor * (xCentral - xMin);
                rightOffset = factor * (xMax - xCentral);
            }
            axisX->setRange((xCentral - leftOffset), (xCentral + rightOffset));
        }
    }
    QChartView::wheelEvent(event);
}

void CCustomChartView::saveAxisRange()
{
    if (this->chart()->axes(Qt::Horizontal).empty())
        return;
    if (this->chart()->axes(Qt::Vertical).empty())
        return;

    /* save x axix pos */
    if (this->chart()->axes(Qt::Horizontal).first()->inherits("QtCharts::QDateTimeAxis")) {
        QDateTimeAxis *axisX = dynamic_cast<QDateTimeAxis*>(this->chart()->axes(Qt::Horizontal).first());
        m_xMinDT = axisX->min();
        m_xMaxDT = axisX->max();
    } else {
        QValueAxis *axisX = dynamic_cast<QValueAxis*>(this->chart()->axes(Qt::Horizontal).first());
        m_xMin = axisX->min();
        m_xMax = axisX->max();
    }

    /* save y axis pos */
    m_yMin.clear();
    m_yMax.clear();

    for (int i=0; i < this->chart()->axes(Qt::Vertical).count(); i++) {
        QValueAxis *axisY = dynamic_cast<QValueAxis*>(this->chart()->axes(Qt::Vertical).at(i));
        m_yMin.append(axisY->min());
        m_yMax.append(axisY->max());
    }
    m_alreadySaveRange = true;
}

void  CCustomChartView::restoreAxisRange() {
    if (this->chart()->axes(Qt::Horizontal).empty())
        return;
    if (m_yMin.count() < this->chart()->axes(Qt::Vertical).count())
        return;
    if (m_yMax.count() < this->chart()->axes(Qt::Vertical).count())
        return;

    if (m_alreadySaveRange) {
        if (this->chart()->axes(Qt::Horizontal).first()->inherits("QtCharts::QDateTimeAxis")) {
            this->chart()->axes(Qt::Horizontal).first()->setRange(m_xMinDT, m_xMaxDT);
        } else {
            this->chart()->axes(Qt::Horizontal).first()->setRange(m_xMin, m_xMax);
        }

        QValueAxis *yAxis;
        for (int i=0; i<this->chart()->axes(Qt::Vertical).count(); i++) {
            yAxis = dynamic_cast<QValueAxis*>(this->chart()->axes(Qt::Vertical).at(i));
            yAxis->setRange(m_yMin[i], m_yMax[i]);
        }
    }
}
void CCustomChartView::on_point_pressed(const QPointF &point) {
    QXYSeries *serie = qobject_cast<QXYSeries*>(sender());

    if (!m_point_value_display) {
        m_point_value_display = new QGraphicsSimpleTextItem(this->chart());
        m_point_value_display->setZValue(10);
        m_point_value_display->setFont(QFont("Microsoft YaHei", 11));
        m_point_value_display->setBrush(QBrush(QColor(240,30,30,192)));
    }

    QString value_str;
    if (this->chart()->axes(Qt::Horizontal).first()->inherits("QtCharts::QDateTimeAxis")) {
        value_str = "日期/时间: " + (QDateTime::fromMSecsSinceEpoch(point.x()))
                .toString("yy-MM-dd hh:mm:ss") + "\n";
    } else {
        QString str = this->chart()->axes(Qt::Horizontal).first()->titleText() + ": ";
        value_str =  str + QString::number(point.x(), 'f', 1 ) + "\n";
    }
    value_str += serie->name() + ": ";
    value_str += QString::number(point.y(), 'f', 1);
    value_str += "\n";
    m_point_value_display->setText(value_str);
    m_point_value_display->setPos(QPointF(m_press_pos.x() + 5, m_press_pos.y()));
    m_point_value_display->show();

}
