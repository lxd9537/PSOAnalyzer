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
#include <polyfit.h>
#include <pump.h>
#include <QtMath>

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
    struct DbTableName{
        QString pumpTableName;
        QString motorTableName;
    };

    struct pumpSelection{
      QString   family;
      QString   model;
      int       speed;
      double    impTrimed;
      QString   motorPower;
      int       motorPoles;
    };
    struct motorSelection{
        double  power_rate;
        int     poles;
    };

public:
    Pump_db();
    void        setDbPara(DbPara database_parameter);
    DbPara      getDbPara();
    void        setTableName(DbTableName tableName);
    DbTableName getTableName();

    bool    connnect();
    bool    getConnectStatus();
    void    getPumpFamilyList(QStringList* familyList);
    void    getPumpModelList(QString family, QStringList* modelList);
    void    getPumpSpeedList(QString family, QString model, QList<int>* speedList);
    void    getPumpImpRange(QString family, QString model, int speed, double* bigImp, double* smallImp);
    void    getMotorList(QStringList* motor_list);
    void    getMotorPolesList(double power_rate, QList<int>* poles_list);
    bool    getPumpData(pumpSelection pumpSelection,Pump::PerformData *pumpPerformanceData);
    bool    getMotorData(motorSelection motorSelection,Pump::MotorData *motorData);

private:
    QSqlDatabase    m_db;
    DbPara          m_database_parameter;
    DbTableName     m_tableName;
};

#endif // PUMP_DB_H
