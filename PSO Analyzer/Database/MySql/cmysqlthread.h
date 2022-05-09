#ifndef CMYSQLTHREAD_H
#define CMYSQLTHREAD_H

#include "QString"
#include "QThread"
#include "QSqlDatabase"
#include "PumpSystem/Pump/cpump.h"
#include "QMessageBox"

#define ERROR_CODE_MYSQL_GEN_ERROR               (401)
#define ERROR_CODE_MYSQL_CREATE_ERROR            (402)
#define ERROR_TEXT_MYSQL_CREATE_ERROR            ("Create MySQL client failed.")

class CMySqlThread : public QThread
{
    Q_OBJECT
public:
    typedef struct {
        QString dbType;
        QString hostName;
        uint portNumber;
        QString dbName;
        QString userName;
        QString password;
    }Config;

public:
    CMySqlThread(Config mysql_config, int id);
    ~CMySqlThread();
    bool openDatabase();
    void closeDatabase();
    bool getConnectStatus();
    bool recordEvent(QString owner, QString event);
    bool writeSetPoint(QString name, float value);
    bool readSetPoint(QString name, float *value);
    const QString *getCurrentErrorText() {return &m_current_error_text;};
    bool readPumpModelList(QList<QString> &model_list);
    bool readPumpData(const QString model, CPump::PerformTestData &perform_data);
    bool writePumpData(const CPump::PerformTestData &perform_data);
    bool deletePumpData(QString pump_model);
    bool addPumpData(const CPump::PerformTestData&perform_data);

signals:
    void eventReport(int id);

private:
    int m_id;
    Config m_mysql_config;
    QSqlDatabase m_db;
    QString m_current_error_text;

};

#endif // CMYSQLTHREAD_H
