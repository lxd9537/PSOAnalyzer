#ifndef PUMP_DB_H
#define PUMP_DB_H
#include <QStringList>

#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTextCodec>
#include <QDebug>
#include <QMessageBox>
#include <QSettings>

class Pump_db
{
public:
    struct DbPara{
        QString dbType;
        QString hostName;
        int portNumber;
        QString dbName;
        QString userName;
        QString password;
    };

public:
    Pump_db();
    void    setDbPara(DbPara database_parameter);
    DbPara  getDbPara();
    bool    connnect();
    bool    getConnectStatus();
    void    getPumpFamilyList(QStringList* familyList);
    void    getPumpModelList(QString family, QStringList* modelList);
    void    getPumpSpeedList(QString family, QString model, QList<int>* speedList);
    void    getPumpImpRange(QString family, QString model, int speed, double* bigImp, double* smallImp);
private:
    QSqlDatabase m_db;
    DbPara m_database_parameter;
    QString m_DbTableName;
};

#endif // PUMP_DB_H
