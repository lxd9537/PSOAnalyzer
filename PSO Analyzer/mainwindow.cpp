#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qjsonobject.h"
#include "qjsonarray.h"
#include "qjsonvalue.h"
#include "qbytearray.h"
#include <QPalette>
#include <QColor>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /* get configuration from ini */
    CInfluxDbQueryThread::Config influxdb_query_config;
    CMySqlThread::Config mysql_config;
    if (!loadConfigFromIni(influxdb_query_config, mysql_config)) {
        statusBar()->showMessage("Load configuration from config.ini failed.", 5000);
    }

    /* create a influx database to query */
    m_influxdb_query_thread = new CInfluxDbQueryThread(influxdb_query_config,
                                                       INFLUXDB_QUERY_ID,
                                                       m_semaphore,
                                                       m_influx_query_unit);
    Q_ASSERT(m_influxdb_query_thread);

    connect(this, &MainWindow::influxdbQureyRequired,
            m_influxdb_query_thread, &CInfluxDbQueryThread::onQueryRequired);
    connect(m_influxdb_query_thread, &CInfluxDbQueryThread::queryFinished,
            this, &MainWindow::onInfluxDbQueryFinished);

    /* create a mysql database */
    m_mysql_thread = new CMySqlThread(mysql_config, MYSQL_CLIENT_ID);
    if (m_mysql_thread) {
        m_mysql_thread->start();
        connect(m_mysql_thread, &CMySqlThread::eventReport, this, &MainWindow::onEventReported);
        m_mysql_thread->openDatabase();
    }


    m_semaphore.release();


}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::loadConfigFromIni(CInfluxDbQueryThread::Config &influxdb_query_config,
                                   CMySqlThread::Config &mysql_config) {
    QSettings *configIniRead = new QSettings(QApplication::applicationDirPath()
                                             + CONFIG_FILE_NAME,
                                             QSettings::IniFormat);

    if (!configIniRead) {
       statusBar()->showMessage("Load configuration file failed.", 5000);
        return false;
    } else
        statusBar()->showMessage("Configuration file is loaded.", 5000);

    /* influx db query config */
    bool ok = true;
    if (configIniRead->contains("InfluxDB/HostName")) {
        influxdb_query_config.hostName = configIniRead->value("InfluxDB/HostName").toString();
        influxdb_query_config.portNumber = configIniRead->value("InfluxDB/PortNumber").toUInt(&ok);
        influxdb_query_config.dbName = configIniRead->value("InfluxDB/DbName").toString();
        influxdb_query_config.userName = configIniRead->value("InfluxDB/UserName").toString();
        influxdb_query_config.password = configIniRead->value("InfluxDB/Password").toString();
        if (!ok) {
            statusBar()->showMessage("InfluxDB query configuration error.", 5000);
            return false;
        }
    } else {
        statusBar()->showMessage("Missing InfluxDB query configuration in ini file.", 5000);
        return false;
    }

    /* my sql config */
    if (configIniRead->contains("MySQL/DbType")) {
        mysql_config.dbType = configIniRead->value("MySQL/DbType").toString();
        mysql_config.hostName = configIniRead->value("MySQL/HostName").toString();
        mysql_config.portNumber = configIniRead->value("MySQL/PortNumber").toUInt(&ok);
        mysql_config.dbName = configIniRead->value("MySQL/DbName").toString();
        mysql_config.userName = configIniRead->value("MySQL/UserName").toString();
        mysql_config.password = configIniRead->value("MySQL/Password").toString();
        if (!ok) {
            qDebug()<<"MySql configuration error.";
            return false;
        }
    } else {
        qDebug()<<"Missing MySql server configuration in ini file.";
        return false;
    }
    delete configIniRead;
    return true;
}

void MainWindow::TimeChartInit() {
    QFile hist_para_list(":/text/paras");
    if (!hist_para_list.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }
    QTextStream stream(&hist_para_list);
    stream.setCodec("UTF-8");
    while (!stream.atEnd()) {
        QString line = stream.readLine();
        QStringList values = line.split(QLatin1Char(' '), Qt::SkipEmptyParts);
        CHistCurveWidget::CurveConfig *curve = new CHistCurveWidget::CurveConfig();
        Q_ASSERT(curve);
        curve->para_name = values[0];
        curve->unit = values[1];
        curve->para_code = values[2].toInt();
        curve->range_low = values[3].toDouble();
        curve->range_high = values[4].toDouble();
        curve->measurement = values[5];
        curve->tag_key = values[6];
        curve->tag_value = values[7];
        curve->field = values[8];
        m_curve_list.append(*curve);
    }
    hist_para_list.close();
    m_time_widget->ChartInit(m_curve_list);
}

void MainWindow::onEventReported(int id) {
    QString owner_name, event_text;
    switch(id) {
    case INFLUXDB_QUERY_ID:
        owner_name = "Modbu query client ";
        event_text = *(qobject_cast<CInfluxDbQueryThread *>(sender())->getCurrentErrorText());
        break;
    }
    statusBar()->showMessage(event_text, 5000);
}

void MainWindow::onTimeChartToQueryInfluxDb(int para_code) {
    if (para_code < 1)
        return;

    foreach(CHistCurveWidget::CurveConfig curve, m_curve_list) {
        if (curve.para_code == para_code) {
            m_current_curve = curve;
            break;
        }
    }

    if (m_current_curve.para_code < 1) {
        return;
    }
    m_influx_query_unit.begin = m_time_widget->getBeginTime();
    m_influx_query_unit.end = m_time_widget->getEndTime();
    m_influx_query_unit.field = m_current_curve.field;
    m_influx_query_unit.measurement = m_current_curve.measurement;
    m_influx_query_unit.tag_key = m_current_curve.tag_key;
    m_influx_query_unit.tag_value = m_current_curve.tag_value;
    m_influx_query_unit.points.clear();

    emit influxdbQureyRequired();
}

void MainWindow::onInfluxDbQueryFinished() {
    emit curveUpdate();
}

void MainWindow::on_pumpDataManage_triggered()
{
    CPumpDataManageDialog dlg(m_mysql_thread, this);
    dlg.exec();

}

void MainWindow::on_openProject_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this,
                                                    "打开项目",
                                                    qApp->applicationDirPath(),
                                                    "PSO project (*.pprj)",
                                                    nullptr);

    m_pro_file = new QFile(filename);
    if (!m_pro_file->open(QIODevice::ReadWrite)) {
        qDebug() << "can't open error!";
        delete m_pro_file;
        return;
    }
    QTextStream stream(m_pro_file);
    stream.setCodec("UTF-8");
    QString str = stream.readAll();

    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(str.toUtf8(), &jsonError);
    if (jsonError.error != QJsonParseError::NoError && !doc.isNull()) {
        qDebug() << "Json格式错误！" << jsonError.error;
        m_pro_file->close();
        delete m_pro_file;
        return;
    }

    CSystemProfileInputDialog::SystemProfile system_profile;
    if (doc.isObject()) {
        QJsonObject root_obj = doc.object();
        if (CSystemProfileInputDialog::JSonToSystemProfile(root_obj, system_profile)) {
            m_system_profile = system_profile;
            m_system_profile_loaded = true;
            QString title = this->windowTitle();
            if (title.contains("@")) {
                title = title.left(title.indexOf("@") - 1);
            }
            this->setWindowTitle(title + "@" + filename);
            QList<QString> model_list;
            m_mysql_thread->readPumpModelList(model_list);
            CSystemProfileInputDialog dia(model_list, system_profile);
            if (dia.exec() == QDialog::Accepted) {
                CSystemProfileInputDialog::SystemProfile new_profile;
                QByteArray byte_array ;
                new_profile = dia.getSysemProfile();
                CSystemProfileInputDialog::systemProfileToJSon(new_profile, byte_array);
                QString backup_name = filename;
                backup_name.replace(".pprj", " backup.pprj");
                if (m_pro_file->rename(backup_name)) {
                    QFile new_file(filename);
                    if (new_file.open(QIODevice::WriteOnly)) {
                        new_file.write(byte_array);
                        new_file.close();
                        m_pro_file->remove();
                    }
                }
            }
        }
    }

    m_pro_file->close();
    delete m_pro_file;
}


void MainWindow::on_newProject_triggered()
{
    bool ok;
    int pump_numbers = QInputDialog::getInt(this, "输入系统水泵数量", "水泵数量", 2, 1, 8, 1, &ok, Qt::WindowFlags()|Qt::WindowCloseButtonHint);
    if (!ok || pump_numbers < 1 || pump_numbers > 8)
        return;
    QString filename = QFileDialog::getSaveFileName(this,
                                                    "保存项目",
                                                    qApp->applicationDirPath(),
                                                    "PSO project (*.pprj)",
                                                    nullptr);
    m_pro_file = new QFile(filename);
    if (!m_pro_file->open(QIODevice::ReadWrite)) {
        qDebug() << "can't open error!";
        delete m_pro_file;
        return;
    }

    CSystemProfileInputDialog::SystemProfile profile;
    CSystemProfileInputDialog::newSystemProfile(profile, pump_numbers);
    QByteArray byte_array;
    CSystemProfileInputDialog::systemProfileToJSon(profile, byte_array);
    m_pro_file->write(byte_array);
    m_system_profile_loaded = true;
    QString title = this->windowTitle();
    if (title.contains("@")) {
        title = title.left(title.indexOf("@") - 1);
    }
    this->setWindowTitle( title + "@" + filename);
    QList<QString> model_list;
    m_mysql_thread->readPumpModelList(model_list);
    CSystemProfileInputDialog dia(model_list, profile);
    if (dia.exec() == QDialog::Accepted) {
        CSystemProfileInputDialog::SystemProfile new_profile;
        QByteArray byte_array ;
        new_profile = dia.getSysemProfile();
        CSystemProfileInputDialog::systemProfileToJSon(new_profile, byte_array);
        QString backup_name = filename;
        backup_name.replace(".pprj", " backup.pprj");
        if (m_pro_file->rename(backup_name)) {
            QFile new_file(filename);
            if (new_file.open(QIODevice::WriteOnly)) {
                new_file.write(byte_array);
                new_file.close();
                m_pro_file->remove();
                m_system_profile = new_profile;
            }
        }
    }
}


void MainWindow::on_histCurve_triggered()
{
    if (!m_time_widget) {
        m_time_widget = new CHistCurveWidget(m_semaphore, m_current_curve, m_influx_query_unit, this);
        m_time_widget->setObjectName(QString::fromUtf8("m_time_widget"));
        m_time_widget->setVisible(true);
        ui->stackedWidget->addWidget(m_time_widget);

        TimeChartInit();
        connect(m_time_widget, &CHistCurveWidget::requestForData,
                this, &MainWindow::onTimeChartToQueryInfluxDb);
        connect(this, &MainWindow::curveUpdate,
                m_time_widget, &CHistCurveWidget::on_curve_update);
    }
    ui->stackedWidget->setCurrentWidget(m_time_widget);

}


void MainWindow::on_singlePumpCalculator_triggered()
{
    if (m_mysql_thread->getConnectStatus()) {
        CPumpCalculator dlg(m_mysql_thread, this);
        dlg.exec();
    }

}


void MainWindow::on_performCurve_triggered()
{
    if (!m_perform_curve) {
        m_perform_curve = new CPerfomCurveWidget(m_mysql_thread, this);
        m_perform_curve->setObjectName(QString::fromUtf8("m_value_widget"));
        m_perform_curve->setVisible(true);
        ui->stackedWidget->addWidget(m_perform_curve);
    }
    ui->stackedWidget->setCurrentWidget(m_perform_curve);

}

void MainWindow::on_pumpGroupCalculator_triggered()
{
    if (!m_system_profile_loaded) {
        QMessageBox::about(this, "Warning", "没有打开项目!");
        return;
    }

    QList<CPump::PerformTestData> test_data_list;
    Q_ASSERT(m_system_profile.common_profile.pump_numbers == m_system_profile.pump_profile_list.size());

    for (int i=0; i<m_system_profile.common_profile.pump_numbers; i++) {
        if(m_mysql_thread->getConnectStatus()) {
            CPump::PerformTestData test_data;
            if (!m_mysql_thread->readPumpData(m_system_profile.pump_profile_list.at(i).model, test_data)) {
                QMessageBox::about(this, "Warning", "从MySQL读水泵测试数据失败!");
                return;
            } else {
                test_data_list.append(test_data);
            }
        } else{
            QMessageBox::about(this, "Warning", "MySQL连接断开!");
            return;
        }

    }

    CPumpSystem pump_system;
    QString error_text;
    if (!pump_system.init(m_system_profile, test_data_list,&error_text)) {
        QMessageBox::about(this, "Warning", "没有载入项目!");
        return;
    }

    CPumpSystemCalculator dia(pump_system, this);
    dia.exec();
}

void MainWindow::on_viewProject_triggered()
{
    if (!m_system_profile_loaded) {
        QMessageBox::about(this, "Warning", "没有打开项目!");
        return;
    }
    QList<QString> model_list;
    m_mysql_thread->readPumpModelList(model_list);
    CSystemProfileInputDialog dlg(model_list, m_system_profile);
    dlg.setViewOnly();
    dlg.exec();
}

