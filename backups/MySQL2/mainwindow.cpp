#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionSet_database,&QAction::triggered,this,&MainWindow::on_SetDbPara_clicked);
    connect(ui->pushButton_connect,&QPushButton::clicked,this,&MainWindow::on_connect_clicked);
    connect(ui->comboBox_pumpFamily,QOverload<const QString &>::of(&QComboBox::currentIndexChanged),this,&MainWindow::on_family_changed);
    connect(ui->comboBox_pumpModel,QOverload<const QString &>::of(&QComboBox::currentIndexChanged),this,&MainWindow::on_model_changed);
    connect(ui->comboBox_pumpSpeed,QOverload<const QString &>::of(&QComboBox::currentIndexChanged),this,&MainWindow::on_speed_changed);

    m_dialog_getdbpara = new Dialog_GetDbPara;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_dialog_getdbpara;
}

void MainWindow::on_SetDbPara_clicked()
{

    m_dialog_getdbpara->Initialization(m_pumpDb.getDbPara());

    if(m_dialog_getdbpara->exec()==QDialog::Accepted)
    {
        m_pumpDb.setDbPara(m_dialog_getdbpara->GetDbPara());
    }

}

void MainWindow::on_connect_clicked()
{
    if(m_pumpDb.connnect())
    {
        ui->pushButton_connect->setEnabled(false);

        QStringList family_list;
        m_pumpDb.getPumpFamilyList(&family_list);

        foreach(QString pump_family,family_list)
        {
            ui->comboBox_pumpFamily->addItem(pump_family);
        }
     }
}

void MainWindow::on_family_changed()
{
    if(m_pumpDb.getConnectStatus())
    {
        QString family = ui->comboBox_pumpFamily->currentText();
        QStringList model_list;
        m_pumpDb.getPumpModelList(family,&model_list);

        QString pump_model;
        ui->comboBox_pumpModel->clear();
        foreach(pump_model,model_list)
             ui->comboBox_pumpModel->addItem(pump_model);
     }
}

void MainWindow::on_model_changed()
{
    if(m_pumpDb.getConnectStatus())
    {

        QString family = ui->comboBox_pumpFamily->currentText();
        QString model = ui->comboBox_pumpModel->currentText();

        QList<int> speed_list;
        m_pumpDb.getPumpSpeedList(family,model, &speed_list);

        ui->comboBox_pumpModel->clear();
        foreach(int pump_speed,speed_list)
        {
            ui->comboBox_pumpSpeed->addItem(QString::number(pump_speed));
        }

    }
}

void MainWindow::on_speed_changed()
{
    if(m_pumpDb.getConnectStatus())
    {
        QString family = ui->comboBox_pumpFamily->currentText();
        QString model = ui->comboBox_pumpModel->currentText();
        int speed = ui->comboBox_pumpSpeed->currentText().toInt();

        double bigImp,smallImp;
        m_pumpDb.getPumpImpRange(family,model,speed,&bigImp,&smallImp);
        ui->lineEdit_impRange->setText(QString::number(smallImp,'f',1)
                                       + " - " + QString::number(bigImp,'f',1));
    }
}
