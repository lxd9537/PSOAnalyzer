#include "caqtmessage.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonArray>
#include <QPointF>
#include <QSemaphore>
#include <QEventLoop>
#include <QDateTime>

CAqtMessage::CAqtMessage()
{

}

void CAqtMessage::Init(const QString &device_name,
                       const QString &recall_flag) {
    m_device_name = device_name;
    m_recall_flag = recall_flag;
}
bool CAqtMessage::importVars(QTextStream &stream) {
    stream.setCodec("UTF-8");
    bool all_ok = true;

    while (!stream.atEnd()) {
        QString line = stream.readLine();
        QStringList values = line.split(QLatin1Char(','));
        AqtVar var;
        var.name = values[0];
        if (values[1] == "FLOAT")
            var.type = FLOAT;
        else if (values[1] == "WORD" || values[1] == "SHORT" || values[1] == "INT")
            var.type = SHORT;
        else if (values[1] == "DWORD" || values[1] == "LONG" || values[1] == "DINT")
            var.type = LONG;
        else if (values[1] == "BOOL")
            var.type = BOOL;
        if (values[2] == "read/write")
            var.rw = READONLY;
        else
            var.rw = READWRITE;
        if (values[3] == "onchange")
            var.mode = ONCHANGE;
        else
            var.mode = ONTIME;

        bool ok = true;
        switch(var.type) {
        case FLOAT:
            var.db.setValue(values[4].toFloat(&ok));
            var.value.setValue(0.0);
            var.old_value.setValue(0.0);
            break;
        case SHORT:
            var.db.setValue(values[4].toShort(&ok));
            var.value.setValue(0);
            var.old_value.setValue(0);
            break;
        case LONG:
            var.db.setValue(values[4].toLong(&ok));
            var.value.setValue(0);
            var.old_value.setValue(0);
            break;
        case BOOL:
            var.db.setValue(false);
            var.value.setValue(false);
            var.old_value.setValue(false);
            break;
        }

        if (values[5]=="alarm")
            var.alarm = true;
        else
            var.alarm = false;

        all_ok = (all_ok && ok);
        if (all_ok) {
            if (var.alarm)
                m_alarm_var_list.append(var);
            else
                m_value_var_list.append(var);
        }
    }
    return all_ok;
}

void CAqtMessage::makeJsonMessage(const QList<AqtVar> &var_list, const QString &group, QString &message) {
    QJsonObject data_points;
    for (const AqtVar &var : var_list) {
        QJsonValue val;
        switch(var.type) {
        case FLOAT: val = var.value.toFloat();break;
        case SHORT: val = var.value.toInt();break;
        case LONG: val = var.value.toLongLong();break;
        case BOOL: val = var.value.toBool();break;
        }
        data_points.insert(var.name,val);
    }

    QJsonObject root;
    root.insert("device", m_device_name);
    root.insert("timestamp", QDateTime::currentDateTime().toMSecsSinceEpoch());
    root.insert("groupname", group);
    root.insert("recallflag", m_recall_flag);
    root.insert("datapoints", data_points);

    QJsonDocument doc(root);
    message = doc.toJson(QJsonDocument::Compact);

    return;
}
