#ifndef CAQTMESSAGE_H
#define CAQTMESSAGE_H
#include <QString>
#include <QVariant>
#include <QTextStream>
#include <QJsonObject>

class CAqtMessage
{
    typedef enum {
        READONLY = 0,
        READWRITE = 1
    } ReadWriteType;
    typedef enum {
        ONCHANGE = 0,
        ONTIME = 1,
        FORCE = 2,
    } PublishMode;
    typedef enum {
        SHORT = 0,
        LONG = 1,
        FLOAT = 2,
        BOOL = 3
    } DataType;
    typedef struct {
        QString name;
        QVariant value;
        DataType type;
        ReadWriteType rw;
        PublishMode mode;
        QVariant db;
        bool alarm;
        QVariant old_value;
    } AqtVar;

public:
    CAqtMessage();

void Init(const QString &device_name,
          const QString &recall_flag);
bool importVars(QTextStream &stream);

void sendDataPoints(PublishMode mode) {
    QString message;
    if (mode == FORCE) {
        makeJsonMessage(m_alarm_var_list, "alarm", message);
        publishMessage(message);
        makeJsonMessage(m_value_var_list, "update",message);
        publishMessage(message);
        return;
    } else if (mode == ONTIME) {
        QList<AqtVar> var_list;
        const QList<AqtVar> &alarm_var_list = m_alarm_var_list;
        for (const AqtVar &var: alarm_var_list) {
            if (var.mode == ONTIME)
                var_list.append(var);
        }
        makeJsonMessage(var_list, "alarm", message);
        publishMessage(message);

        var_list.clear();
        const QList<AqtVar> &value_var_list = m_value_var_list;
        for (const AqtVar &var: value_var_list) {
            if (var.mode == ONTIME)
                var_list.append(var);
        }
        makeJsonMessage(var_list, "update", message);
        publishMessage(message);
    } if (mode == ONCHANGE) {
        QList<AqtVar> var_list;
        for (AqtVar &var: m_alarm_var_list) {
            if (var.mode == ONCHANGE)
                if (var.value.toBool() != var.old_value.toBool()) {
                    var_list.append(var);
                    var.old_value.setValue(var.value.toBool());
                }
        }
        makeJsonMessage(var_list, "alarm", message);
        publishMessage(message);

        var_list.clear();
        for (AqtVar &var: m_value_var_list) {
            if (var.mode == ONTIME) {
                switch (var.type) {
                case FLOAT:
                    if (abs(var.value.toFloat() - var.old_value.toFloat()) > var.db.toFloat()) {
                        var_list.append(var);
                        var.old_value.setValue(var.value.toFloat());
                    }
                    break;
                case BOOL:
                    if (var.value.toBool() != var.old_value.toBool()) {
                        var_list.append(var);
                        var.old_value.setValue(var.value.toFloat());
                    }
                    break;
                case SHORT:
                    if (abs(var.value.toInt() - var.old_value.toInt()) > var.db.toInt()) {
                        var_list.append(var);
                        var.old_value.setValue(var.value.toInt());
                    }
                    break;
                case LONG:
                    if (abs(var.value.toLongLong() - var.old_value.toLongLong()) > var.db.toLongLong()) {
                        var_list.append(var);
                        var.old_value.setValue(var.value.toLongLong());
                    }
                    break;
                }
                var_list.append(var);
            }
        }
        makeJsonMessage(var_list, "update", message);
        publishMessage(message);
    }

}
void publishMessage(const QString &message) {

}

void makeJsonMessage(const QList<AqtVar> &var_list, const QString &group, QString &message);

private:
    QString m_topic = "GL1234567890123";
    QString m_device_name = "PLC01";
    QString m_recall_flag = "0";

public:
    QList<AqtVar> m_value_var_list;
    QList<AqtVar> m_alarm_var_list;

};

#endif // CAQTMESSAGE_H

