#ifndef INFLUXDBINSERTTHREAD_H
#define INFLUXDBINSERTTHREAD_H

#include "QString"
#include "QThread"
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonArray>
#include <QPointF>
#include <QEventLoop>

/* influx DB */
#define PUMPS_NUMBERS              (6)
#define SYSTEMS_NUMBERS            (2)

#define ERROR_TEXT_INFLUXDB_INSERT_TIMEOUT       ("InfluxDB insert timeout.")

class CInfluxDbInsertThread : public QThread
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
        QString key;
        QString value;
    }Point;
    typedef struct {
        bool enable;
        QString meansurement;
        QString tag_key;
        QString tag_value;
        QList<Point> fields;
    }InsertUnit;

public:
    CInfluxDbInsertThread(Config config, int id, const QVector<InsertUnit> &insert_units);
    ~CInfluxDbInsertThread();
    const QString *getCurrentErrorText() {return &m_current_error_text;};

signals:
    void eventReport(int id);
    void insertFinished();

public slots:
    void onInsertRequired();
    void onInsertReply();

private:
    int m_id;
    Config m_influx_config;
    QNetworkAccessManager *m_network_manager;
    QString m_current_error_text;
    const QVector<InsertUnit> &m_insert_units;
    QEventLoop m_loop;
};

#endif // MYSQLTHREAD_H
