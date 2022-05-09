#include "cinfluxdbquerythread.h"
#include <QDebug>
#include <QNetworkReply>

CInfluxDbQueryThread::CInfluxDbQueryThread(Config config,
                                           int id,
                                           QSemaphore &semaphore,
                                           QueryUnit &query_unit)
    :ex_semaphore(semaphore), ex_query_unit(query_unit) {
    m_id = id;
    m_influx_config = config;
    m_network_manager = new QNetworkAccessManager();
    Q_ASSERT(m_network_manager);

}
CInfluxDbQueryThread::~CInfluxDbQueryThread() {
    delete m_network_manager;
}

void CInfluxDbQueryThread::onQueryRequired() {

    qDebug()<<"Influxdb has received select request.";

    /* make sql */
    QString sql;
    sql = QString("select ");
    sql += ex_query_unit.field;
    sql += " ";
    sql += QString("from %1 where time > %2s and time < %3s").arg(ex_query_unit.measurement,
                                                                  QString::number(ex_query_unit.begin.toSecsSinceEpoch()),
                                                                  QString::number(ex_query_unit.end.toSecsSinceEpoch()));
    sql += QString(" and %1=\'%2\'").arg(ex_query_unit.tag_key, ex_query_unit.tag_value);
    qDebug()<<sql;

    /* make url */
    QString url;
    url = QString("http://%1:%2/query?db=%3&epoch=s&q=%4").
            arg(m_influx_config.hostName, QString::number(m_influx_config.portNumber), m_influx_config.dbName, sql);
    qDebug()<<url;

    /* query */
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    auto *reply = m_network_manager->get(req);
    qDebug()<<"Influxdb query req posted.";


    if (reply) {
        if (!reply->isFinished()) {
            connect(reply, &QNetworkReply::finished, this, &CInfluxDbQueryThread::onQueryReply);
            m_loop.exec();
            return;
        }
        else
            reply->deleteLater();
    }
    emit queryFinished();
}

void  CInfluxDbQueryThread::onQueryReply() {
    QString measurement;
    QList<QString> columns;
    QList<QList<double>> points;

    ex_semaphore.acquire();
    ex_query_unit.points.clear();

    auto *reply = qobject_cast<QNetworkReply *>(sender());
    if (reply) {
        QByteArray responseData = reply->readAll();
        QJsonParseError jsonError;
        QJsonDocument document = QJsonDocument::fromJson(responseData, &jsonError);
        if (reply->error()==QNetworkReply::NoError) {
            QJsonObject root_obj = document.object();
            if (root_obj.contains("error")) {
                m_current_error_text = root_obj["error"].toString();
                emit eventReport(m_id);
            } else if (root_obj["results"].isArray()) {
                QJsonObject results_obj = root_obj["results"].toArray().first().toObject();
                if (results_obj.contains("error")) {
                    m_current_error_text = root_obj["error"].toString();
                    emit eventReport(m_id);
                } else if (results_obj.contains("series")) {
                    QJsonObject series_obj = results_obj["series"].toArray().first().toObject();
                    if (series_obj.contains("name")) {
                        measurement = series_obj["name"].toString();
                        if (series_obj.contains("columns")) {
                            QJsonArray col_ary = series_obj["column"].toArray();
                            foreach (const QJsonValue &col_val, col_ary) {
                                columns.append(col_val.toString());
                            }
                            if (series_obj.contains("values")) {
                                QJsonArray value_ary = series_obj["values"].toArray();
                                foreach (const QJsonValue &col_val, value_ary) {
                                    QJsonArray rec_ary = col_val.toArray();
                                    QList<double> point;
                                    for (int i=0; i < rec_ary.count(); i++) {
                                        point.append(rec_ary.at(i).toDouble());
                                    }
                                    points.append(point);
                                    if (rec_ary.count() >= 2) {
                                        QPointF p;
                                        p.setX(rec_ary.first().toDouble() * 1000);
                                        p.setY(rec_ary.at(1).toDouble());
                                        ex_query_unit.points.append(p);
                                    }
                                }
                            }
                        }
                    }

                }
            }
        }
        else {
            m_current_error_text = reply->errorString();
            emit eventReport(m_id);
        }
        reply->deleteLater();
    }
    emit queryFinished();
    m_loop.exit();
}
