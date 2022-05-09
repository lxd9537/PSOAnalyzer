#include "dialog_getdbpara.h"
#include "ui_dialog_getdbpara.h"

Dialog_GetDbPara::Dialog_GetDbPara(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_GetDbPara)
{
    ui->setupUi(this);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &Dialog_GetDbPara::on_applyButton_clicked);
}

Dialog_GetDbPara::~Dialog_GetDbPara()
{
    delete ui;
}
void Dialog_GetDbPara::Initialization(Pump_db::DbPara database_parameter, Pump_db::DbTableName tableName)
{
    ui->lineEdit_1->setText(database_parameter.dbType);
    ui->lineEdit_2->setText(database_parameter.hostName);
    ui->lineEdit_3->setText(QString::number(database_parameter.portNumber));
    ui->lineEdit_4->setText(database_parameter.dbName);
    ui->lineEdit_5->setText(database_parameter.userName);
    ui->lineEdit_6->setText(database_parameter.password);
    ui->lineEdit_7->setText(tableName.pumpTableName);
    ui->lineEdit_8->setText(tableName.motorTableName);

}

void Dialog_GetDbPara::on_applyButton_clicked()
{
    m_database_parameter.dbType = ui->lineEdit_1->text();
    m_database_parameter.hostName = ui->lineEdit_2->text();
    m_database_parameter.portNumber = ui->lineEdit_3->text().toInt();
    m_database_parameter.dbName = ui->lineEdit_4->text();
    m_database_parameter.userName = ui->lineEdit_5->text();
    m_database_parameter.password = ui->lineEdit_6->text();
    m_tableName.pumpTableName = ui->lineEdit_7->text();
    m_tableName.motorTableName = ui->lineEdit_8->text();
}

Pump_db::DbPara Dialog_GetDbPara::GetDbPara()
{
    return m_database_parameter;
}

Pump_db::DbTableName Dialog_GetDbPara::GetDbTableName()
{
    return m_tableName;
}
