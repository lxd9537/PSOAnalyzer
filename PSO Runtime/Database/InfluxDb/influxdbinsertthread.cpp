#include "influxdbinsertthread.h"
#include <QDebug>
#include <QNetworkReply>

CInfluxDbInsertThread::CInfluxDbInsertThread(Config config,
                                             int id,
                                             const QVector<InsertUnit> &insert_units)
                                            :m_insert_units(insert_units) {
    m_id = id;
    m_influx_config = config;
    m_network_manager = new QNetworkAccessManager();
    Q_ASSERT(m_network_manager);

}
CInfluxDbInsertThread::~CInfluxDbInsertThread() {
    delete m_network_manager;
}

void CInfluxDbInsertThread::onInsertRequired()
{
    qDebug()<<"InfluxDb has received insert request.";

    /* make url */
    qDebug()<<"InfluxDb is making url.";
    QString url;
    url = QString("http://%1:%2/write?db=%3").
            arg(m_influx_config.hostName,
                QString::number(m_influx_config.portNumber),
                m_influx_config.dbName);

    /* make data */
    qDebug()<<"InfluxDb is making insertion data.";
    QString data;
    foreach (const InsertUnit &unit, m_insert_units) {
        if (unit.enable) {
            data.append(unit.meansurement + ",");
            data.append(unit.tag_key + "=" + unit.tag_value + " ");
            foreach (const Point &field, unit.fields) {
                if (field.key.count() < 1)
                    break;
                data.append(field.key + "=" + field.value + ",");
            }
            if (data.endsWith(','))
                data.replace(data.count() - 1, 1, '\n');
        }
    }
    if (data.isEmpty()) {
        qDebug()<<"InfluxDb unit to insert is empty.";
        return;
    }

    /* post data */
    qDebug()<<"InfluxDb is ready for post insertion.";
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    auto *reply = m_network_manager->post(req, data.toLocal8Bit());
    qDebug()<<"Influxdb data posted.";

    if (reply) {
        if (!reply->isFinished()) {
            connect(reply, &QNetworkReply::finished, this, &CInfluxDbInsertThread::onInsertReply);
            m_loop.exec();
            return;
        }
        else
            reply->deleteLater();
    }

    emit insertFinished();
}

void  CInfluxDbInsertThread::onInsertReply() {
    /* forward replied error infor */
    auto *reply = qobject_cast<QNetworkReply *>(sender());
    if (reply) {
        if (reply->error()==QNetworkReply::NoError) {
            QByteArray responseData;
            responseData = reply->readAll();
            QJsonObject root;
            if (!responseData.isEmpty()) {
                QJsonParseError jsonError;
                QJsonDocument document = QJsonDocument::fromJson(responseData, &jsonError);
                if (!document.isNull() && (jsonError.error == QJsonParseError::NoError)) {
                    if (document.isObject())
                        root = document.object();
                }
                if (!root.isEmpty()) {
                    m_current_error_text = root["error"].toString();
                    emit eventReport(m_id);
                }
            } else
                qDebug()<<"InfluxDb insertion with no error.";
        } else {
            m_current_error_text = reply->errorString();
            emit eventReport(m_id);
            qDebug()<<"InfluxDb insertion with error."<<reply->errorString();
        }
        reply->deleteLater();
    }
    emit insertFinished();
    m_loop.exit();
}
