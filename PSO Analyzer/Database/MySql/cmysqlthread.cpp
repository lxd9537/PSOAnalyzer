#include "cmysqlthread.h"
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

bool CMySqlThread::readPumpModelList(QList<QString> &model_list) {
    model_list.clear();
    if (getConnectStatus()) {
        QSqlQuery query;
        QString sql;
        sql = "select distinct model from pump_para";
        if (query.exec(sql)) {
            while (query.next()) {
                QVariant variant = query.value(0);
                if (variant.isValid()) {
                    model_list.append(variant.toString());
                }
            }
            return true;
        }
    }
    return false;
}

bool CMySqlThread::readPumpData(const QString model, CPump::PerformTestData &perform_data) {
    perform_data.model = model;
    if (getConnectStatus()) {
        /* 在水泵para表中查找, 找不到则失败退出 */
        QSqlQuery query;
        QString sql;
        sql = "select distinct model,motor_power,min_flow,max_flow from pump_para where model = \'";
        sql += perform_data.model + "\'";
        if (query.exec(sql)) {
            if (query.size() < 1)
                return false;
            if (query.next()) {
                if (query.value(0).isValid()) {
                    perform_data.model = query.value(0).toString();
                }
                if (query.value(1).isValid()) {
                    perform_data.motor_power = query.value(1).toFloat();
                }
                if (query.value(2).isValid()) {
                    perform_data.flow_min = query.value(2).toFloat();
                }
                if (query.value(3).isValid()) {
                    perform_data.flow_max = query.value(3).toFloat();
                }
            }
        } else
            return false;

        sql = "select model,flow, head, power, npshr from pump_perform where model = \'";
        sql += perform_data.model + "\'";
        if (query.exec(sql)) {
            if (query.size() < 1) {
                return false;
            } else {
                int count = 0;{
                    while (query.next()) {
                        if (query.value(0).isValid()) {
                            perform_data.model = query.value(0).toString();
                        }
                        if (query.value(1).isValid()) {
                            perform_data.flow[count] = query.value(1).toFloat();
                        }
                        if (query.value(2).isValid()) {
                            perform_data.head[count] = query.value(2).toFloat();
                        }
                        if (query.value(3).isValid()) {
                            perform_data.power[count] = query.value(3).toFloat();
                        }
                        if (query.value(4).isValid()) {
                            perform_data.npshr[count] = query.value(4).toFloat();
                        }
                        count ++;
                        if (count > 9)
                            break;
                    }
                    perform_data.points = count;
                    return true;
                }
            }
        }
    }
    return false;
}

bool CMySqlThread::writePumpData(const CPump::PerformTestData &perform_data) {
    QSqlQuery query;
    QString sql;

    if (getConnectStatus()) {
        sql = "delete from pump_para where model = \'";
        sql += perform_data.model + "\'";
        if (query.exec(sql)) {
            sql = "select * from pump_para where model = \'";
            sql += perform_data.model + "\'";
            if (query.exec(sql)) {
                if (query.size() > 0) {
                    QMessageBox::warning(nullptr,"Warning","数据库已有该型号！");
                    return false;
                }
            }
        }
        sql = "insert into pump_para (id, model, motor_power, min_flow, max_flow) values(0,\'";
        sql += perform_data.model + "\',";
        sql += QString::number(perform_data.motor_power,'f',2) + ",";
        sql += QString::number(perform_data.flow_min, 'f', 2) + ",";
        sql += QString::number(perform_data.flow_max, 'f', 2) + ")";
        if (query.exec(sql)) {
            sql = "delete from pump_perform where model = \'";
            sql += perform_data.model + "\'";
            if (query.exec(sql)) {
                sql = "select * from pump_perform where model = \'";
                sql += perform_data.model + "\'";
                if (query.exec(sql)) {
                    if (query.size() > 0) {
                        QMessageBox::warning(nullptr,"Warning","数据库已有该型号！");
                        return false;
                    }
                }
            }
            sql = "insert into pump_perform (id, model, flow, head, power, npshr) values ";
            for (int i=0; i<perform_data.points; i++) {
                sql += "(0,\'";
                sql += perform_data.model + "\',";
                sql += QString::number(perform_data.flow[i],'f',2) + ",";
                sql += QString::number(perform_data.head[i], 'f', 2) + ",";
                sql += QString::number(perform_data.power[i], 'f', 2) + ",";
                sql += QString::number(perform_data.npshr[i], 'f', 2);
                sql += ")";
                if (i < perform_data.points - 1)
                    sql += ",";
            }
            if (!query.exec(sql))
                return false;
            else
                return true;
        }
    }
    return false;
}
bool CMySqlThread::deletePumpData(QString pump_model) {
    QSqlQuery query;
    QString sql;

    if (getConnectStatus()) {
        sql = "delete from pump_para where model = \'";
        sql += pump_model + "\'";
        if (query.exec(sql)) {
            sql = "delete from pump_perform where model = \'";
            sql += pump_model + "\'";
            if (query.exec(sql))
                return true;
        }
    }
    return false;
}

bool CMySqlThread::addPumpData(const CPump::PerformTestData &perform_data) {
    QSqlQuery query;
    QString sql;

    if (getConnectStatus()) {
        sql = "select * from pump_para where model = \'";
        sql += perform_data.model + "\' limit 1";
        if (query.exec(sql)) {
            if (query.size() > 0) {
                QMessageBox::about(nullptr,"Warning","数据库已有该型号！");
                return false;
            } else {
                return writePumpData(perform_data);
            }
        }
    }
    return false;
}

