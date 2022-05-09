#include "cpumpsystemcalculator.h"
#include "ui_cpumpsystemcalculator.h"

CPumpSystemCalculator::CPumpSystemCalculator(CPumpSystem &pump_system, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CPumpSystemCalculator),
    ex_pump_system(pump_system)
{
    ui->setupUi(this);

    this->setModal(true);
    QFont font("Microsoft YaHei", 11);
    this->setFont(font);
    this->setWindowIcon(QIcon(":/ico/set"));
    this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    this->setFixedSize(this->width(), this->height());

    ex_pump_system.moveToThread(&m_thread);
    connect(this, &CPumpSystemCalculator::calculateBestCombination, &ex_pump_system, &CPumpSystem::doCalculation);
    connect(&ex_pump_system, &CPumpSystem::calculationFinished, this, &CPumpSystemCalculator::onCalculationFinished);

    m_thread.start();
    m_calculate_start = false;

    /* 显示格式 */
    QFont font_title;
    font_title.setFamily("Microsoft YaHei");
    font_title.setPointSize(11);
    font_title.setItalic(true);
    font_title.setUnderline(true);
    m_format_title.setFont(font_title);

    QFont font_content;
    font_content.setFamily("Microsoft YaHei");
    font_content.setPointSize(9);
    font_content.setBold(false);
    m_format_content.setFont(font_content);

    /* 显示水泵概况 */
    m_doc = ui->system_description_display->document();
    m_doc->setDefaultFont(font_content);

    QTextCursor cur = ui->system_description_display->textCursor();
    cur.setCharFormat(m_format_title);
    cur.insertText("系统概况:");

    QString str;
    const CCommonProfileInput::CommonProfile common_profile = ex_pump_system.getCommonProfile();
    str += "\n  水泵总数: " + QString::number(common_profile.pump_numbers) + " 台";
    str += "\n  出水总管管径: " + QString::number(common_profile.out_pipe_dia, 'f', 0) + " mm";
    str += "\n  出水总管压力变送器高度: " + QString::number(common_profile.out_pt_height, 'f', 2) + " m";
    for (int i=0; i<common_profile.in_tank_level.size(); i++) {
        QString sub_str = QString("\n  取水水池液位%1: %2 m").arg(i+1).arg(common_profile.in_tank_level.at(i), 0, 'f', 2);
        str += sub_str;
    }
    for (int i=0; i<common_profile.out_press_set.size(); i++) {
        QString sub_str = QString("\n  当流量为: %1 m3/h时, 压力设定值为: %2 MPa")
                .arg(common_profile.out_press_set.at(i).x(), 0, 'f', 1)
                .arg(common_profile.out_press_set.at(i).y(), 0, 'f', 3);
        str += sub_str;
    }
    cur.setCharFormat(m_format_content);
    cur.insertText(str);

    if (ex_pump_system.isPumpsGrouped()) {
        cur.setCharFormat(m_format_title);
        cur.insertText("\n泵组概况:");
        str.clear();
        int i=1;
        foreach (QList<CPump*> group, ex_pump_system.getPumpGroup()) {
            QString sub_str = QString("\n  泵组%1: 包含%2台水泵, ").arg(i++).arg(group.size());
            str += sub_str;
            sub_str.clear();
            if (group.size() > 0) {
                CPumpProfileInput::PumpProfile profile = group.at(0)->getProfile();
                sub_str = QString("\n  水泵型号为: %1，采用%2控制, ")
                        .arg(profile.model, profile.vfd_installed ? "变频" : "工频");
                if (profile.vfd_installed) {
                    sub_str += QString("最高转速%1 %，最低转速%2 %, ")
                            .arg(profile.vfd_max_speed,0,'f',1)
                            .arg(profile.vfd_min_speed,0,'f',1);
                }
                sub_str = QString("入口管径%1 mm，入口管压力损失系数%2 Pa/(m3/h)2, 出口管压力损失系数%3 Pa/(m3/h)2.")
                        .arg(profile.in_pipe_dia,0,'f',0)
                        .arg(profile.in_pipe_coeff,0,'g')
                        .arg(profile.out_pipe_coeff,0,'g');
            }
            str += sub_str;
        }
        cur.setCharFormat(m_format_content);
        cur.insertText(str);
    }
}

CPumpSystemCalculator::~CPumpSystemCalculator()
{
    m_thread.quit();
    m_thread.wait();
    m_thread.deleteLater();
    delete ui;
}


void CPumpSystemCalculator::on_startCalculate_clicked()
{
    if (!m_thread.isRunning())
        m_thread.start();
    if (!ex_pump_system.isInitiated())
        return;

    float min_flow = ui->minFlow->value();
    float max_flow = ui->maxFlow->value();
    float step = ui->flowStep->value();
    if (max_flow - min_flow < (step + 0.1))
        return;
    QVector<QPointF> setpoint_list;
    setpoint_list = ex_pump_system.getPressureSet(min_flow, max_flow, step);
    m_flow_head_list.clear();
    foreach (float h0, ex_pump_system.getCommonProfile().in_tank_level) {
        foreach (const QPointF &set_point, setpoint_list) {
            float flow = set_point.x();
            float head = ex_pump_system.convertPressureSetToHead(flow, set_point.y(), h0);
            m_setpoint_map[QString::number(flow,'f',0) + QString::number(head,'f',1)]
                    = QPointF(h0, set_point.y());
            m_flow_head_list.append(QPointF(flow, head));
        }
    }
    m_point_index = 0;
    m_calculate_start = true;
    if (!m_flow_head_list.isEmpty())
        emit calculateBestCombination(m_flow_head_list.at(m_point_index).x(),
                                      m_flow_head_list.at(m_point_index).y());
    m_point_index ++;

    if (!m_calculate_progress) {
        m_calculate_progress = new QProgressDialog("计算进度", "取消",
                                               0, 100 ,
                                               this, Qt::Dialog|Qt::CustomizeWindowHint | Qt::WindowTitleHint);

        m_calculate_progress->setWindowTitle("进度对话框");
    }
    m_calculate_progress->setRange(0, m_flow_head_list.size());
    m_calculate_progress->show();

    QTextCursor cur = ui->system_description_display->textCursor();
    cur.setCharFormat(m_format_title);
    cur.insertText("\n计算结果:\n");

}


void CPumpSystemCalculator::on_terminateCalculate_clicked()
{

    m_calculate_start = false;
    m_calculate_progress->cancel();
    m_calculate_progress->close();
    m_thread.quit();
}

void CPumpSystemCalculator::onCalculationFinished() {

    m_calculate_progress->setValue(m_point_index + 1);
    CPumpSystem::PumpCombination combination = ex_pump_system.getCombinationReslut();
    if (combination.flow > 0.1) {
        QString str = QString("  水位: %1m, 出口压力: %2MPa, 总流量: %3m3/h, 扬程: %4m, 总功率: %5kW\n")
                .arg(QString::number(m_setpoint_map.value(QString::number(combination.flow,'f',0) + QString::number(combination.head,'f',1)).x(), 'f' ,2),
                     QString::number(m_setpoint_map.value(QString::number(combination.flow,'f',0) + QString::number(combination.head,'f',1)).y(), 'f' ,3),
                     QString::number(combination.flow, 'f' ,0),
                     QString::number(combination.head, 'f' ,1),
                     QString::number(combination.power, 'f', 1));

        for (int i=0; i<combination.group_workpoint_list.size(); i++) {
            QString str_2;
            if (combination.group_workpoint_list.at(i).running_qty > 0) {
                str_2 = QString("    泵组( ");
                for (int j=0; j<combination.group_workpoint_list.at(i).group.size(); j++) {
                    str_2 += combination.group_workpoint_list.at(i).group.at(j)->getName();
                    str_2 += " ";
                }
                str_2 += "), ";
                str_2 += QString("运行数量: %1, 单泵流量: %2m3/h, 单泵功率: %3kW.\n").arg(QString::number(combination.group_workpoint_list.at(i).running_qty)
                                                          , QString::number(combination.group_workpoint_list.at(i).workpoint.flow, 'f', 0)
                                                          , QString::number(combination.group_workpoint_list.at(i).workpoint.power, 'f', 1));
                str += str_2;
            }
        }

        QTextCursor cur = ui->system_description_display->textCursor();
        cur.setCharFormat(m_format_content);
        cur.insertText(str);
    }

    if (m_calculate_start) {
        if (m_flow_head_list.size() > m_point_index) {
            emit calculateBestCombination(m_flow_head_list.at(m_point_index).x(),
                                          m_flow_head_list.at(m_point_index).y());
            m_point_index ++;
        }
        else {
            m_calculate_start = false;
            m_point_index = 0;
            m_calculate_progress->cancel();
        }
    }
}
