#ifndef CINFLUXDBQUERYTHREAD_H
#define CINFLUXDBQUERYTHREAD_H

#include "QString"
#include "QThread"
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonArray>
#include <QPointF>
#include <QSemaphore>
#include <QEventLoop>

#define ERROR_CODE_INFLUXDB_SELECT_ERROR       (303)


class CInfluxDbQueryThread : public QThread
{
    Q_OBJECT
public:
    typedef struct {
        QString hostName;
        uint    portNumber;
        QString dbName;
        QString userName;
        QString password;
    }Config;
    typedef struct {
        QString measurement;
        QString tag_key;
        QString tag_value;
        QString field;
        QDateTime begin;
        QDateTime end;
        QList<QPointF> points;
    }QueryUnit;

public:
    CInfluxDbQueryThread(Config config, int id, QSemaphore &semaphore, QueryUnit &query_unit);
    ~CInfluxDbQueryThread();
    const QString *getCurrentErrorText() {return &m_current_error_text;};

signals:
    void eventReport(int id);
    void queryFinished();

public slots:
    void onQueryRequired();
    void onQueryReply();

private:
    int m_id;
    Config m_influx_config;
    QNetworkAccessManager *m_network_manager;
    QString m_current_error_text;
    QSemaphore &ex_semaphore;
    QueryUnit &ex_query_unit;
    QEventLoop m_loop;
};

#endif // MYSQLTHREAD_H
