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
    m_tableName.pumpTableName=configIniRead->value("/table_name/pumpTableName").toString();
    m_tableName.motorTableName=configIniRead->value("/table_name/motorTableName").toString();

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

void Pump_db::setTableName(DbTableName tableName)
{
    m_tableName = tableName;

    QSettings *configIniWrite = new QSettings("config.ini", QSettings::IniFormat);

    configIniWrite->setValue("/table_name/pumpTableName", m_tableName.pumpTableName);
    configIniWrite->setValue("table_name/motorTableName", m_tableName.motorTableName);

    delete configIniWrite;
}

Pump_db::DbTableName Pump_db::getTableName()
{
    return m_tableName;
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
    return true;
}

bool Pump_db::getConnectStatus()
{
    return m_db.isOpen();
}

void Pump_db::getPumpFamilyList(QStringList* familyList)
{
    if(!m_db.isOpen())
    {
        QMessageBox::warning(nullptr,"Data acquired failed", "Database is not connected!");
        return;
    }
    QSqlQuery query;
    QString sql;

    sql = "select distinct family from " + m_tableName.pumpTableName;
    query.exec(sql);

    familyList->clear();
    while(query.next())
        *familyList<<query.value("family").toString();
}
void Pump_db::getPumpModelList(QString family, QStringList* modelList)
{
    if(!m_db.isOpen())
    {
        QMessageBox::warning(nullptr,"Data acquired failed", "Database is not connected!");
        return;
    }

    QSqlQuery query;
    QString sql;

    sql = "select distinct model from " + m_tableName.pumpTableName + " where family = '" + family + "'";
    query.exec(sql);

    modelList->clear();
    while(query.next())
        *modelList<<query.value("model").toString();
}

void Pump_db::getPumpSpeedList(QString family, QString model, QList<int>* speedList)
{
    if(!m_db.isOpen())
    {
        QMessageBox::warning(nullptr,"Data acquired failed", "Database is not connected!");
        return;
    }

    QSqlQuery query;
    QString sql;

    sql = "select distinct speed from " + m_tableName.pumpTableName +
            " where family = '" + family + "'"
            + "and model = '" + model +"'";
    query.exec(sql);

    speedList->clear();
    while(query.next())
        *speedList<<query.value("speed").toInt();
}

void Pump_db::getPumpImpRange(QString family, QString model, int speed, double* bigImp, double* smallImp)
{
    if(!m_db.isOpen())
    {
        QMessageBox::warning(nullptr,"Data acquired failed", "Database is not connected!");
        return;
    }

    QSqlQuery query;
    QString sql;

    sql = "select distinct imp_dia from " + m_tableName.pumpTableName +
            " where family = '" + family + "'"
            + " and model = '" + model +"'"
            + " and speed = " + QString::number(speed);
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

void Pump_db::getMotorList(QStringList* motor_list)
{
    if(!m_db.isOpen())
    {
        QMessageBox::warning(nullptr,"Data acquired failed", "Database is not connected!");
        return;
    }

    QSqlQuery query;
    QString sql;

    sql = "select distinct power_rate from " + m_tableName.motorTableName;

    query.exec(sql);

    motor_list->clear();
    while(query.next())
        *motor_list<<query.value("power_rate").toString();
}

void Pump_db::getMotorPolesList(double power_rate, QList<int>* poles_list)
{
    if(!m_db.isOpen())
    {
        QMessageBox::warning(nullptr,"Data acquired failed", "Database is not connected!");
        return;
    }

    QSqlQuery query;
    QString sql;

    sql = "select distinct poles from " + m_tableName.motorTableName
            + " where power_rate > " + QString::number(power_rate - 0.05)
            + " and power_rate < "  + QString::number(power_rate + 0.05);

    query.exec(sql);

    poles_list->clear();
    while(query.next())
        poles_list->append(query.value("poles").toInt());
}

bool Pump_db::getPumpData(pumpSelection pumpSelection,Pump::PerformData *pumpPerformanceData)
{
    if(!m_db.isOpen())
    {
        QMessageBox::warning(nullptr,"Data acquired failed", "Database is not connected!");
        return false;
    }

    //查询数据库
    QSqlQuery query;
    QString sql;

    sql = "select distinct imp_dia from " + m_tableName.pumpTableName +
            " where family = '" + pumpSelection.family + "'"
            + " and model = '" + pumpSelection.model +"'"
            + " and speed = " + QString::number(pumpSelection.speed);

    query.exec(sql);

    if(query.size()<1)
    {
        QMessageBox::warning(nullptr,"failed to acquire pump data","There's no data in database for the specified pump!");
        return false;
    }

    //获得叶轮最接近水泵数据
    double          bigImp   = -1;
    double          smallImp = -1;
    int             impCount = 0;
    double          fImpDia[100];
    QStringList     ImpDiaList;
    double          fSmallerImp=0, fBiggerImp=1e308;

    while (query.next())
    {
        double imp_dia = query.value("imp_dia").toDouble();
        if(bigImp < 0 || smallImp <0)
            bigImp = smallImp = imp_dia;
        else {
            if(imp_dia > bigImp)
                bigImp = imp_dia;
            if(imp_dia < smallImp)
                smallImp = imp_dia;
        }

        fImpDia[impCount] = imp_dia;
        ImpDiaList.append(query.value("imp_dia").toString());
        impCount++;
    }

    if(pumpSelection.impTrimed < smallImp || pumpSelection.impTrimed > bigImp)
    {
        QMessageBox::warning(nullptr,"failed to acquire pump data","Impeller trimed is out of range!");
        return false;
    }

    //获得最接近的两个叶轮直径
    for (int i = 0; i < impCount; i++)
    {
        if ((fImpDia[i] > fSmallerImp) && (fImpDia[i] <= pumpSelection.impTrimed))
            fSmallerImp = fImpDia[i];

        if ((fImpDia[i] < fBiggerImp) && (fImpDia[i] >= pumpSelection.impTrimed))
            fBiggerImp = fImpDia[i];
    }

    //获得拟合系数
    QString strConvtImp;
    double          fPumpFlow_1[30],     fPumpFlow_2[30];
    double          fPumpHead_1[30],     fPumpHead_2[30];
    double          fPumpP2_1[30],       fPumpP2_2[30];
    unsigned int    nPoints_1 = 0,       nPoints_2 = 0;

    double fKFlowHead_1[CURVE_FIT_ORDER+1],     fKFlowHead_2[CURVE_FIT_ORDER+1];
    double fKFlowP2_1[CURVE_FIT_ORDER+1],       fKFlowP2_2[CURVE_FIT_ORDER+1];
    double fFlowMax_1=0,        fFlowMax_2=0;
    double fFlowMin_1=1e308,    fFlowMin_2=1e308;
    double fPowerMax_1=0,       fPowerMax_2=0;
    double fPowerMin_1=1e308,   fPowerMin_2=1e308;
    double fHeadMax_1=0,        fHeadMax_2=0;
    double fHeadMin_1=1e308,    fHeadMin_2=1e308;

    double fKFlowHead[CURVE_FIT_ORDER+1];
    double fKFlowP2[CURVE_FIT_ORDER+1];
    double fFlowMax=0;
    double fFlowMin=1e308;
    double fPowerMax=0;
    double fPowerMin=1e308;
    double fHeadMax=0;
    double fHeadMin=1e308;

    //重新查询数据库
    sql = "select * from " + m_tableName.pumpTableName +
            " where family = '" + pumpSelection.family + "'"
            + " and model = '" + pumpSelection.model +"'"
            + " and speed = " + QString::number(pumpSelection.speed);

    query.exec(sql);

    //叶轮直径恰好在库中
    if (abs(pumpSelection.impTrimed - fSmallerImp)<0.1 || abs(pumpSelection.impTrimed - fBiggerImp)<0.1)
    {
        double imp_dia;

        if(abs(pumpSelection.impTrimed - fSmallerImp)<0.1)
            imp_dia = fSmallerImp;
        else imp_dia = fBiggerImp;

        while(query.next())
        {
            if(abs(query.value("imp_dia").toDouble() - imp_dia)<0.1)
            {
               fPumpFlow_1[nPoints_1] = query.value("flow").toDouble();
               fPumpHead_1[nPoints_1] = query.value("head").toDouble();
               fPumpP2_1[nPoints_1] = query.value("power").toDouble();
               nPoints_1++;
            }
        }

        polyfit(fPumpFlow_1, fPumpHead_1, nPoints_1, CURVE_FIT_ORDER, fKFlowHead);
        polyfit(fPumpFlow_1, fPumpP2_1, nPoints_1, CURVE_FIT_ORDER, fKFlowP2);

        for (unsigned int i = 0; i < nPoints_1; i++)
        {
            if (fFlowMax < fPumpFlow_1[i])
                fFlowMax = fPumpFlow_1[i];
            if (fFlowMin > fPumpFlow_1[i])
                fFlowMin = fPumpFlow_1[i];

            if (fPowerMax < fPumpP2_1[i])
                fPowerMax = fPumpP2_1[i];
            if (fPowerMin > fPumpP2_1[i])
                fPowerMin = fPumpP2_1[i];

            if (fHeadMax < fPumpHead_1[i])
                fHeadMax = fPumpHead_1[i];
            if (fHeadMin > fPumpHead_1[i])
                fHeadMin = fPumpHead_1[i];
        }
    }

    //叶轮直径不在库中
    else
    {
    //获取小叶轮/大叶轮系数
        while(query.next())
        {
            if(abs(query.value("imp_dia").toDouble() - fSmallerImp)<0.1)
            {
               fPumpFlow_1[nPoints_1] = query.value("flow").toDouble();
               fPumpHead_1[nPoints_1] = query.value("head").toDouble();
               fPumpP2_1[nPoints_1] = query.value("power").toDouble();
               nPoints_1++;
            }
            if(abs(query.value("imp_dia").toDouble() - fBiggerImp)<0.1)
            {
               fPumpFlow_2[nPoints_2] = query.value("flow").toDouble();
               fPumpHead_2[nPoints_2] = query.value("head").toDouble();
               fPumpP2_2[nPoints_2] = query.value("power").toDouble();
               nPoints_2++;
            }
        }

        double SizeFactor_1 = pumpSelection.impTrimed / fSmallerImp;
        double SizeFactor_2 = pumpSelection.impTrimed / fBiggerImp;

        for (unsigned int k = 0; k < nPoints_1; k++)
        {
            fPumpFlow_1[k] = fPumpFlow_1[k]*SizeFactor_1;
            fPumpHead_1[k] = fPumpHead_1[k] * pow(SizeFactor_1, 2);
            fPumpP2_1[k]= fPumpP2_1[k]* pow(SizeFactor_1, 3);
        }

         for (unsigned int k = 0; k < nPoints_2; k++)
        {
            fPumpFlow_2[k] = fPumpFlow_2[k] * SizeFactor_2;
            fPumpHead_2[k] = fPumpHead_2[k] * pow(SizeFactor_2, 2);
            fPumpP2_2[k] = fPumpP2_2[k] * pow(SizeFactor_2, 3);
        }

        polyfit(fPumpFlow_1, fPumpHead_1, nPoints_1, CURVE_FIT_ORDER, fKFlowHead_1);
        polyfit(fPumpFlow_1, fPumpP2_1, nPoints_1, CURVE_FIT_ORDER, fKFlowP2_1);

        polyfit(fPumpFlow_2, fPumpHead_2, nPoints_2, CURVE_FIT_ORDER, fKFlowHead_2);
        polyfit(fPumpFlow_2, fPumpP2_2, nPoints_2, CURVE_FIT_ORDER, fKFlowP2_2);

        for (unsigned int i = 0; i < nPoints_1; i++)
        {
            if (fFlowMax_1 < fPumpFlow_1[i])
                fFlowMax_1 = fPumpFlow_1[i];
            if (fFlowMin_1 > fPumpFlow_1[i])
                fFlowMin_1 = fPumpFlow_1[i];

            if (fPowerMax_1 < fPumpP2_1[i])
                fPowerMax_1 = fPumpP2_1[i];
            if (fPowerMin_1 > fPumpP2_1[i])
                fPowerMin_1 = fPumpP2_1[i];

            if (fHeadMax_1 < fPumpHead_1[i])
                fHeadMax_1 = fPumpHead_1[i];
            if (fHeadMin_1 > fPumpHead_1[i])
                fHeadMin_1 = fPumpHead_1[i];
        }


        for (unsigned int i = 0; i < nPoints_2; i++)
        {
            if (fFlowMax_2 < fPumpFlow_2[i])
                fFlowMax_2 = fPumpFlow_2[i];
            if (fFlowMin_2 > fPumpFlow_2[i])
                fFlowMin_2 = fPumpFlow_2[i];

            if (fPowerMax_2 < fPumpP2_2[i])
                fPowerMax_2 = fPumpP2_2[i];
            if (fPowerMin_2 > fPumpP2_2[i])
                fPowerMin_2 = fPumpP2_2[i];

            if (fHeadMax_2 < fPumpHead_2[i])
                fHeadMax_2 = fPumpHead_2[i];
            if (fHeadMin_2 > fPumpHead_2[i])
                fHeadMin_2 = fPumpHead_2[i];
        }


        //插值法计算所需直径数据
        double fConvertK;
        fConvertK = (pumpSelection.impTrimed - fSmallerImp) / (fBiggerImp - fSmallerImp);

        for (int i = 0; i < CURVE_FIT_ORDER+1; i++)
        {
            fKFlowHead[i] = (fKFlowHead_2[i] - fKFlowHead_1[i])*fConvertK + fKFlowHead_1[i];
            fKFlowP2[i] = (fKFlowP2_2[i] - fKFlowP2_1[i])*fConvertK + fKFlowP2_1[i];
        }

        fFlowMax = (fFlowMax_2 - fFlowMax_1)*fConvertK + fFlowMax_1;
        fFlowMin = (fFlowMin_2 - fFlowMin_1)*fConvertK + fFlowMin_1;
        fPowerMax = (fPowerMax_2 - fPowerMax_1)*fConvertK + fPowerMax_1;
        fPowerMin= (fPowerMin_2 - fPowerMin_1)*fConvertK + fPowerMin_1;
        fHeadMax = (fHeadMax_2 - fHeadMax_1)*fConvertK + fHeadMax_1;
        fHeadMin = (fHeadMin_2 - fHeadMin_1)*fConvertK + fHeadMin_1;
    }

    //得到0扬程流量
    double  low_flow = 0;
    double  high_flow = fFlowMax * 5;
    double  middle_flow = high_flow / 2;
    int     cycles = 0;

    do {
        double head_calcu = 0;
        for(int i=0;i<CURVE_FIT_ORDER+1;i++)
            head_calcu += fKFlowHead[i]*pow(middle_flow, i);

        if (abs(head_calcu) <= 0.1)
            break;
        else
            if (head_calcu > 0.1)
                low_flow = middle_flow;
            else
                high_flow = middle_flow;

        middle_flow = (high_flow + low_flow) / 2;

    } while (cycles++ < 30);

    //结果回传
    pumpPerformanceData->Hz_max = 50;
    pumpPerformanceData->Hz_min =5;
    pumpPerformanceData->flow_max = fFlowMax;
    pumpPerformanceData->flow_min = fFlowMin;
    pumpPerformanceData->power_max = fPowerMax;
    pumpPerformanceData->power_min = fPowerMin;
    pumpPerformanceData->head_max = fHeadMax;
    pumpPerformanceData->head_min = fHeadMin;

    for(int i=0;i<CURVE_FIT_ORDER+1;i++)
    {
        pumpPerformanceData->Kfactor_head_from_flow[i] = fKFlowHead[i];
        pumpPerformanceData->Kfactor_power_from_flow[i] = fKFlowP2[i];
    }
    pumpPerformanceData->head_at_zero_flow = fKFlowHead[0];
    pumpPerformanceData->flow_at_zero_head = middle_flow;

    return true;
}

bool Pump_db::getMotorData(motorSelection motorSelection,Pump::MotorData *motorData)
{
    if(!m_db.isOpen())
    {
        QMessageBox::warning(nullptr,"Data acquired failed", "Database is not connected!");
        return false;
    }

    //查询数据库
    QSqlQuery query;
    QString sql;
    double motorLowerLimit = motorSelection.power_rate - 0.01;
    double motorUpperLimit = motorSelection.power_rate + 0.01;

    sql = "select * from " + m_tableName.motorTableName +
            " where power_rate > " + QString::number(motorLowerLimit)
            + " and power_rate < " + QString::number(motorUpperLimit)
            + " and poles = " + QString::number(motorSelection.poles);

    query.exec(sql);

    if(query.size()<1)
    {
        QMessageBox::warning(nullptr,"failed to acquire motor data","There's no data in database for the specified motor!");
        return false;
    }

    //获得拟合系数
    unsigned int    nPoints = 0;
    double          fMotorPower[10];
    double          fMotorEffi[10];

    while(query.next())
    {
        fMotorPower[nPoints] = query.value("power").toDouble();
        fMotorEffi[nPoints] = query.value("effi").toDouble();
        nPoints++;
    }

    polyfit(fMotorPower, fMotorEffi, nPoints, CURVE_FIT_ORDER, motorData->Kfactor_effi_from_power);

    motorData->power_rate = motorSelection.power_rate;
    motorData->poles = motorSelection.poles;

    return true;
}
