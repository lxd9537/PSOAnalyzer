#include "cpumpdatamanagedialog.h"
#include "ui_cpumpdatamanagedialog.h"

CPumpDataManageDialog::CPumpDataManageDialog(CMySqlThread *mysql_thread, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CPumpDataManageDialog)
{
    ui->setupUi(this);

    m_mysql_thread = mysql_thread;
    QFont font("Microsoft YaHei", 11);
    this->setModal(true);
    this->setWindowTitle("水泵数据库");
    this->resize(200,240);
    this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    this->setFont(font);
    this->setFixedSize(this->width(), this->height());

}

CPumpDataManageDialog::~CPumpDataManageDialog()
{
    delete ui;
}

void CPumpDataManageDialog::on_modelListRefresh_clicked()
{
    QList<QString> model_list;
    m_mysql_thread->readPumpModelList(model_list);
    ui->modelList->clear();
    ui->modelList->addItems(model_list);
}


void CPumpDataManageDialog::on_dataView_clicked()
{
    QString model = ui->modelList->currentText();
    if (model.isEmpty())
        return;
    CPump::PerformTestData raw_perform_data;
    if (!m_mysql_thread->readPumpData(ui->modelList->currentText(), raw_perform_data)) {
        QMessageBox::warning(this,"Warning","读数据库失败！");
        return;
    }
    else {
        CPumpDataInputDialog dia(this);
        dia.init(raw_perform_data);
        dia.setViewOnly();
        dia.exec();
    }
}


void CPumpDataManageDialog::on_dataModify_clicked()
{
    QString model = ui->modelList->currentText();
    if (model.isEmpty())
        return;
    CPump::PerformTestData raw_perform_data;
    if (!m_mysql_thread->readPumpData(model, raw_perform_data)) {
        QMessageBox::about(this, "修改水泵数据","无法在数据库中读取水泵数据！");
        return;
    } else {
        CPumpDataInputDialog dia;
        dia.init(raw_perform_data);
        if (dia.exec()==QDialog::Accepted) {
            raw_perform_data = dia.getData();
            if (m_mysql_thread->writePumpData(raw_perform_data)) {
                QMessageBox::about(this, "修改水泵数据","修改成功！");
            }
        }
    }
}

void CPumpDataManageDialog::on_dataNew_clicked()
{
    QString model;
    int points;
    bool ok;
    QInputDialog dlg;
    QFont font("Microsoft YaHei", 9);
    dlg.setFont(font);
    /* 得到水泵型号 */
    model = dlg.getText(this,
                        "新增水泵型号",
                        "水泵型号",
                        QLineEdit::Normal,
                        QString(),
                        &ok,
                        Qt::Dialog | Qt::WindowCloseButtonHint);
    if (!model.isEmpty() && ok) {
        /* 得到水泵数据行数 */
        points = dlg.getInt(this,
                            "新增水泵型号",
                            "数据行数",
                            0,
                            5,
                            10,
                            1,
                            &ok);
        /* 插入水泵数据 */
        if (points >= 5 && points <= 10 && ok) {
            CPump::PerformTestData raw_perform_data;
            raw_perform_data.model = model;
            raw_perform_data.points = points;
            raw_perform_data.flow_min = 0;
            raw_perform_data.flow_max = 0;
            raw_perform_data.motor_power = 0;
            for (int i=0; i<raw_perform_data.points; i++){
                raw_perform_data.flow[i] = 0;
                raw_perform_data.head[i] = 0;
                raw_perform_data.power[i] = 0;
                raw_perform_data.npshr[i] = 0;
            }
            if (m_mysql_thread->addPumpData(raw_perform_data)) {
                QList<QString> model_list;
                m_mysql_thread->readPumpModelList(model_list);
                ui->modelList->clear();
                ui->modelList->addItems(model_list);
                int i = ui->modelList->findText(raw_perform_data.model);
                if (i >= 0) {
                    ui->modelList->setCurrentIndex(i);
                    return;
                }
            }
        }
    }
}

void CPumpDataManageDialog::on_dataDelete_clicked()
{
    QString model = ui->modelList->currentText();
    if (model.isEmpty())
        return;

    if (QMessageBox::question(this, "Question", "确定要删除型号" + model, QMessageBox::Yes|QMessageBox::Cancel) == QMessageBox::Yes) {

        if (!m_mysql_thread->deletePumpData(model)) {
            return;
        } else {
            QList<QString> model_list;
            m_mysql_thread->readPumpModelList(model_list);
            ui->modelList->clear();
            ui->modelList->addItems(model_list);
        }
    }
}
