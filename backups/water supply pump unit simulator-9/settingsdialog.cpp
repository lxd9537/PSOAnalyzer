#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QSerialPortInfo>
#include <QtNetwork>
#include <qnetworkinterface.h>
#include <QList>
#include <QStringListModel>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    m_input_dialog = new IntInputDialog(this);

    //connections
    connect(ui->conselCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SettingsDialog::on_conselCombo_currentIndexChanged);
    connect(ui->idEdit,&IntInputEdit::clicked,this,&SettingsDialog::on_IntInputEdit_clicked);
    connect(ui->portEdit,&IntInputEdit::clicked,this,&SettingsDialog::on_IntInputEdit_clicked);
    connect(m_input_dialog,&IntInputDialog::ValueChanged,this,&SettingsDialog::on_IntInputEdit_changed);
    connect(ui->applyButton, &QPushButton::clicked, this, &SettingsDialog::on_applyButton_clicked);
    connect(ui->cancelButton, &QPushButton::clicked, [this]() {hide();});
}

SettingsDialog::~SettingsDialog()
{
    delete m_input_dialog;
    delete ui;
}

void SettingsDialog::Initiation(int total_pumps)
{
    //set total pumps
    if(total_pumps > 6)
        m_total_pumps = 6;
    else if(total_pumps < 1)
        m_total_pumps = 1;
    else m_total_pumps = total_pumps;

    //emuration of system serial ports
    QComboBox *comsel[6];
    comsel[0]= ui->comselCombo_1;
    comsel[1]= ui->comselCombo_2;
    comsel[2]= ui->comselCombo_3;
    comsel[3]= ui->comselCombo_4;
    comsel[4]= ui->comselCombo_5;
    comsel[5]= ui->comselCombo_6;

    for(int i=0; i<6; i++)
        comsel[i]->clear();

    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
    {
        QString s = info.portName();
        for(int i=0; i<6; i++)
            comsel[i]->addItem(s);
    }

    for(int i=0; i<6; i++)
        comsel[i]->setCurrentText(m_settings.SerialPortName[i]);

    //other settings
    ui->conselCombo->setCurrentIndex(m_settings.connectType);
    ui->baudCombo->setCurrentText(QString::number(m_settings.baud));
    ui->dataBitsCombo->setCurrentText(QString::number(m_settings.dataBits));
    ui->parityCombo->setCurrentIndex(0);
    ui->stopBitsCombo->setCurrentText(QString::number(m_settings.stopBits));
    ui->idEdit->Initiation(m_settings.ModbusID,1,9999);
    ui->portEdit->Initiation(m_settings.port,1,30000);

    //hide unused serial ports
    QLabel *comsellabel[6];
    comsellabel[0]=ui->label_6;
    comsellabel[1]=ui->label_11;
    comsellabel[2]=ui->label_12;
    comsellabel[3]=ui->label_13;
    comsellabel[4]=ui->label_14;
    comsellabel[5]=ui->label_15;

    for(int i=0; i<m_total_pumps; i++)
    {
        comsel[i]->setVisible(true);
        comsellabel[i]->setVisible(true);
    }
    for(int i=m_total_pumps; i<6; i++)
    {
        comsel[i]->setVisible(false);
        comsellabel[i]->setVisible(false);
    }

    //emuration of ip address
    QString local_host_name = QHostInfo::localHostName();
    QHostInfo host_infor = QHostInfo::fromName(local_host_name);

    ui->ipCombo->clear();
    foreach(QHostAddress address, host_infor.addresses())
    {
        if(address.protocol()==QAbstractSocket::IPv4Protocol)
            ui->ipCombo->addItem(address.toString());
    }

    //in case that connection type changed
    on_conselCombo_currentIndexChanged(m_settings.connectType);
}

SettingsDialog::CommSettings SettingsDialog::ReadCommSettings() const
{
    return m_settings;
}

void SettingsDialog::on_conselCombo_currentIndexChanged(int index)
{
    ConnectionType type = static_cast<ConnectionType> (index);
    if (type == Serial) {
        ui->comselCombo_1->setEnabled(true);
        ui->comselCombo_2->setEnabled(true);
        ui->comselCombo_3->setEnabled(true);
        ui->comselCombo_4->setEnabled(true);
        ui->comselCombo_5->setEnabled(true);
        ui->comselCombo_6->setEnabled(true);
        ui->baudCombo->setEnabled(true);
        ui->dataBitsCombo->setEnabled(true);
        ui->parityCombo->setEnabled(true);
        ui->stopBitsCombo->setEnabled(true);
        ui->ipCombo->setDisabled(true);
        ui->portEdit->setDisabled(true);
    }
    else {
        ui->comselCombo_1->setDisabled(true);
        ui->comselCombo_2->setDisabled(true);
        ui->comselCombo_3->setDisabled(true);
        ui->comselCombo_4->setDisabled(true);
        ui->comselCombo_5->setDisabled(true);
        ui->comselCombo_6->setDisabled(true);
        ui->baudCombo->setDisabled(true);
        ui->dataBitsCombo->setDisabled(true);
        ui->parityCombo->setDisabled(true);
        ui->stopBitsCombo->setDisabled(true);
        ui->ipCombo->setEnabled(true);
        ui->portEdit->setEnabled(true);
    }

}
void SettingsDialog::on_IntInputEdit_clicked()
{
    m_current_int_edit = static_cast<IntInputEdit*>(sender());
    m_input_dialog->Initiation(
                m_current_int_edit->GetCurrentValue(),
                m_current_int_edit->GetMinValue(),
                m_current_int_edit->GetMaxValue());

    m_input_dialog->show();
}
void SettingsDialog::on_IntInputEdit_changed(int value)
{
    m_current_int_edit->SetCurrentValue(value);
}

void SettingsDialog::on_applyButton_clicked()
{
    m_settings.connectType = static_cast<ConnectionType>(ui->conselCombo->currentIndex());
    m_settings.ModbusID=ui->idEdit->GetCurrentValue();
    m_settings.SerialPortName[0]=ui->comselCombo_1->currentText();
    m_settings.SerialPortName[1]=ui->comselCombo_2->currentText();
    m_settings.SerialPortName[2]=ui->comselCombo_3->currentText();
    m_settings.SerialPortName[3]=ui->comselCombo_4->currentText();
    m_settings.SerialPortName[4]=ui->comselCombo_5->currentText();
    m_settings.SerialPortName[5]=ui->comselCombo_6->currentText();
    m_settings.baud = ui->baudCombo->currentText().toInt();
    m_settings.dataBits = ui->dataBitsCombo->currentText().toInt();
    m_settings.parity = ui->parityCombo->currentIndex();
    if (m_settings.parity > 0)
        m_settings.parity++;
    m_settings.stopBits = ui->stopBitsCombo->currentText().toInt();
    m_settings.IpAddr = ui->ipCombo->currentText();
    m_settings.port = ui->portEdit->GetCurrentValue();

    hide();
}
