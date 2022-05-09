#include "mysqlthread.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QtDebug>
#include <QDateTime>

CMySqlThread::CMySqlThread(Config mysql_config, int id)
{
    m_id = id;
    m_mysql_config = mysql_config;

}
CMySqlThread::~CMySqlThread() {

}
bool CMySqlThread::openDatabase() {
    m_db = QSqlDatabase::addDatabase(m_mysql_config.dbType);
    if (m_db.isValid()) {
        m_db.setHostName(m_mysql_config.hostName);
        m_db.setPort(m_mysql_config.portNumber);
        m_db.setDatabaseName(m_mysql_config.dbName);
        m_db.setUserName(m_mysql_config.userName);
        m_db.setPassword(m_mysql_config.password);
        if (m_db.open()) {
            return true;
        }
    }
    m_current_error_text = m_db.lastError().databaseText();
    emit eventReport(m_id);
    return false;
}
void CMySqlThread::closeDatabase() {
    m_db.close();
}

bool CMySqlThread::getConnectStatus() {
    if (m_db.isValid()) {
        if (m_db.isOpen()) {
            return true;
        } else {
            m_current_error_text = m_db.lastError().databaseText();
            emit eventReport(m_id);
        }
    } else {
        m_current_error_text = ERROR_TEXT_MYSQL_CREATE_ERROR;
        emit eventReport(m_id);
    }
    return false;
}

bool CMySqlThread::recordEvent(QString owner, QString event) {
    if (getConnectStatus()) {
        QSqlQuery query;
        QString sql;
        sql = "insert into event_log values(\'";
        sql += QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss") + "\',\'";
        sql += owner + "\',\'";
        sql += event + "\')";
        return query.exec(sql);
    }
    return false;
}

bool CMySqlThread::writeSetPoint(QString name, float value) {
    if (getConnectStatus()) {
        QSqlQuery query;
        QString sql;
        sql = "insert into setpoint values(\'";
        sql += name + "\',\'";
        sql += QString::number(value) + "\',\'";
        sql += QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss") + "\')";
        return query.exec(sql);
    }
    return false;
}

bool CMySqlThread::readSetPoint(QString name, float *value) {
    if (getConnectStatus()) {
        QSqlQuery query;
        QString sql;
        sql = "select * from setpoint where datetime = (select max(datetime) from setpoint where name = \'";
        sql += name + "\')";
        if (query.exec(sql)) {
            if (query.next()) {
                QVariant variant = query.value("value");
                if (variant.isValid()) {
                    *value = query.value("value").toFloat();
                    return true;
                }
            }
        }
    }
    return false;
}
