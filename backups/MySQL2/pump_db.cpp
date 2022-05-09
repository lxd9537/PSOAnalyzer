#include "pump_db.h"

Pump_db::Pump_db()
{
    QSettings *configIniRead = new QSettings("config.ini", QSettings::IniFormat);

    m_database_parameter.dbType=configIniRead->value("/database/dbType").toString();
    m_database_parameter.hostName=configIniRead->value("/database/hostname").toString();
    m_database_parameter.portNumber=configIniRead->value("/database/portNumber").toInt();
    m_database_parameter.dbName=configIniRead->value("/database/dbName").toString();
    m_database_parameter.userName=configIniRead->value("/database/userName").toString();
    m_database_parameter.password=configIniRead->value("/database/password").toString();
    m_DbTableName=configIniRead->value("/database table/tableName").toString();

    delete configIniRead;
}

void Pump_db::setDbPara(DbPara database_parameter)
{

    m_database_parameter = database_parameter;

    QSettings *configIniWrite = new QSettings("config.ini", QSettings::IniFormat);

    configIniWrite->setValue("/database/dbType", database_parameter.dbType);
    configIniWrite->setValue("database/hostname", database_parameter.hostName);
    configIniWrite->setValue("database/portNumber", QString::number(database_parameter.portNumber));
    configIniWrite->setValue("database/dbName", database_parameter.dbName);
    configIniWrite->setValue("database/userName", database_parameter.userName);
    configIniWrite->setValue("database/password", database_parameter.password);

    delete configIniWrite;
}

Pump_db::DbPara Pump_db::getDbPara()
{
    return m_database_parameter;
}

bool Pump_db::connnect()
{
    m_db = QSqlDatabase::addDatabase(m_database_parameter.dbType);
    m_db.setHostName(m_database_parameter.hostName);
    m_db.setPort(m_database_parameter.portNumber);
    m_db.setDatabaseName(m_database_parameter.dbName);
    m_db.setUserName(m_database_parameter.userName);
    m_db.setPassword(m_database_parameter.password);

    bool ok = m_db.open();
    if(!ok)
    {
        QMessageBox::critical(nullptr,QObject::tr("connect database failed!"),m_db.lastError().text());
        return false;
    }
    else
    {
        QMessageBox::information(nullptr,QObject::tr("Tips"),QObject::tr("connect database succeed!"));
        return true;
    }
}

bool Pump_db::getConnectStatus()
{
    return m_db.isOpen();
}

void Pump_db::getPumpFamilyList(QStringList* familyList)
{
    QSqlQuery query;
    QString sql;

    sql = "select distinct family from " + m_DbTableName;
    query.exec(sql);

    familyList->clear();
    while(query.next())
        *familyList<<query.value("family").toString();
}
void Pump_db::getPumpModelList(QString family, QStringList* modelList)
{
    QSqlQuery query;
    QString sql;

    sql = "select distinct model from " + m_DbTableName + " where family = '" + family + "'";
    query.exec(sql);

    modelList->clear();
    while(query.next())
        *modelList<<query.value("model").toString();
}

void Pump_db::getPumpSpeedList(QString family, QString model, QList<int>* speedList)
{
    QSqlQuery query;
    QString sql;

    sql = "select distinct speed from " + m_DbTableName +
            " where family = '" + family + "'"
            + "and model = '" + model +"'";
    query.exec(sql);

    speedList->clear();
    while(query.next())
        *speedList<<query.value("speed").toInt();
}

void Pump_db::getPumpImpRange(QString family, QString model, int speed, double* bigImp, double* smallImp)
{
    QSqlQuery query;
    QString sql;

    sql = "select distinct model from " + m_DbTableName +
            " where family = '" + family + "'"
            + "and model = '" + model +"'"
            + "and speed = " + QString::number(speed);
    query.exec(sql);

    *bigImp = -1;
    *smallImp = -1;
    while (query.next())
    {
        double imp_dia = query.value("imp_dia").toDouble();
        if(*bigImp < 0 || *smallImp <0)
            *bigImp = *smallImp = imp_dia;
        else {
            if(imp_dia > *bigImp)
                *bigImp = imp_dia;
            if(imp_dia < *smallImp)
                *smallImp = imp_dia;
        }
    }
}
