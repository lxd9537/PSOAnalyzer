#include "cpumpcalculator.h"
#include "ui_cpumpcalculator.h"

CPumpCalculator::CPumpCalculator(CMySqlThread *mysql_thread, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CPumpCalculator)
{
    ui->setupUi(this);
    this->setModal(true);
    QFont font("Microsoft YaHei", 11);
    this->setFont(font);
    this->setWindowIcon(QIcon(":/ico/set"));
    this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    this->setFixedSize(this->width(), this->height());

    m_mysql_thread = mysql_thread;

    if (m_mysql_thread->getConnectStatus()) {
        QStringList model_list;
        m_mysql_thread->readPumpModelList(model_list);
        ui->modelList->addItems(model_list);
    }
}

CPumpCalculator::~CPumpCalculator()
{
    delete ui;
}

void CPumpCalculator::on_calculationTypeList_currentTextChanged(const QString &arg1)
{
    if (arg1 == "已知流量和扬程") {
        m_calculate_type = CPump::FROM_FLOW_AND_HEAD;
        ui->parameterName_1->setText("流量(m3/h)");
        ui->parameterName_2->setText("扬程(m)");
    }
    else if (arg1 == "已知流量和转速") {
        m_calculate_type = CPump::FROM_FLOW_AND_SPEED;
        ui->parameterName_1->setText("流量(m3/h)");
        ui->parameterName_2->setText("转速(%)");
    }
    else if (arg1 == "已知扬程和转速") {
        m_calculate_type = CPump::FROM_HEAD_AND_SPEED;
        ui->parameterName_1->setText("扬程(m)");
        ui->parameterName_2->setText("转速(%)");
    }
}


void CPumpCalculator::on_calculateButton_clicked()
{
    double para_1 = ui->pararmeterInput_1->value();
    double para_2 = ui->pararmeterInput_2->value();
    on_calculationTypeList_currentTextChanged(ui->calculationTypeList->currentText());
    ui->resultText->clear();
    CPump pump;
    CPump::PerformTestData perform_data;
    QString error_message;
    if (!m_mysql_thread->readPumpData(ui->modelList->currentText(), perform_data)) {
        ui->resultText->appendPlainText("从数据库读水泵性能数据失败");
        return;
    }

    if (!pump.createModel(perform_data, &error_message)) {
        ui->resultText->appendPlainText(error_message);
        return;
    }

    CPump::WorkingPoint work_point;
    CPump::CalculateResult result;
    work_point = pump.calculateWorkingPoint(m_calculate_type, para_1, para_2, result);
    QString str;
    if (result != CPump::NORMAL) {
        switch(result) {
        case CPump::NORMAL:break;
        case CPump::MAX_FLOW_EXCEEDED: str = "超过最大流量";break;
        case CPump::MAX_HEAD_EXCEEDED: str = "超过最大扬程";break;
        case CPump::MIN_FLOW_EXCEEDED: str = "低于最小流量";break;
        case CPump::MAX_POWER_EXCEEDED: str = "超过最大功率";break;
        }
        ui->resultText->appendPlainText(str);
    } else {
        str += "转速(%): " + QString::number(work_point.speed) + "\n";
        str += "流量(m3/h): " + QString::number(work_point.flow) + "\n";
        str += "扬程(m): " + QString::number(work_point.head) + "\n";
        str += "功率(kW): " + QString::number(work_point.power) + "\n";
        str += "效率(%): " + QString::number(work_point.effi) + "\n";
        str += "NPSHr(m): " + QString::number(work_point.npshr) + "\n";
        ui->resultText->appendPlainText(str);
    }

}

