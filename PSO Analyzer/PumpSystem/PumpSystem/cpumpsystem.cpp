#include "cpumpsystem.h"
#define FLOAT_MAX (3.40282E+038)
#define FLOAT_MIN (1.17549E-038)
#define SQUARE(x) (x * x)
#define VELOSITY(flow, dia) (flow / 3600 / (SQUARE(dia/1000) * 3.14 / 4))


CPumpSystem::CPumpSystem()
{

}
CPumpSystem::~CPumpSystem()
{

}

/* 初始化 */
bool CPumpSystem::init(const CSystemProfileInputDialog::SystemProfile &system_profile,
                       const QList<CPump::PerformTestData> &pump_data_list, QString *error_message) {
    m_common_profile = system_profile.common_profile;
    if (m_common_profile.pump_numbers > pump_data_list.size()
            || m_common_profile.pump_numbers > system_profile.pump_profile_list.size())
        return false;

    for (int i=0; i<system_profile.common_profile.pump_numbers; i++) {
        CPump *pump = new CPump();
        pump->setName("泵" + QString::number(i+1));
        if (!pump->init(pump_data_list.at(i), system_profile.pump_profile_list.at(i), error_message)) {
            error_message->insert(0, "pump" + QString::number(i+1));
            return false;
        }
        m_pump_list.append(pump);
    }

    groupPumps();

    m_initiated = true;
    return m_initiated;
}

/* 执行计算槽函数 */
void CPumpSystem::doCalculation(float flow, float head) {
    if (m_group_list.isEmpty()) {
        QMessageBox::about(nullptr, "Warning", "无泵可用！");
        return;
    }
    QList<QList<CPump*>> group_list = m_group_list;
    m_combination_result.group_workpoint_list.clear();
    m_combination_result.flow = 0;
    m_combination_result.head = 0;
    m_combination_result.power = 0;
    if (BestCombinationWithDiffModel(group_list, flow, head, m_combination_result)){
        emit calculationFinished();
    }
}

/*
 * 通过流量、压力曲线求设定值曲线
 * 返回值是一个QPointF list，每一个Point的x是流量, y是压力，单位是MPa
*/
QVector<QPointF> CPumpSystem::getPressureSet(float begin_flow, float end_flow, float step) const {
    Q_ASSERT(begin_flow >= 0);
    Q_ASSERT(end_flow < 99999);
    Q_ASSERT(step >= 1);
    Q_ASSERT(end_flow - begin_flow > step);

    QVector<QPointF> pressure_set_list;
    if (m_common_profile.out_press_set.size() < 1)
        return pressure_set_list;

    QVector<QPointF> out_press_set;
    foreach(const QPointF &p, m_common_profile.out_press_set) {
        out_press_set.append(p);
    }
    std::sort(out_press_set.begin(), out_press_set.end(), [](const QPointF &p1, const QPointF &p2) { return p1.x() < p2.x();});
    QPointF point;
    point.setX(0);
    point.setY(out_press_set.first().y());
    out_press_set.insert(0, point);
    point.setX(100000);
    point.setY(out_press_set.last().y());
    out_press_set.append(point);

    float flow = begin_flow;
    int index = 0;
    QPointF point_1 = out_press_set.at(index ++);
    QPointF point_2 = out_press_set.at(index ++);
    while(flow <= end_flow) {
        while (flow > point_2.x()) {
            point_1 = point_2;
            Q_ASSERT(out_press_set.size() > index);
            point_2 = out_press_set.at(index ++);
        }
        QPointF setpoint;
        setpoint.setX(flow);
        setpoint.setY(linearConvert(point_1.x(), point_2.x(), point_1.y(), point_2.y(), flow));
        pressure_set_list.append(setpoint);

        flow += step;
    }
    return pressure_set_list;
}

/* 已知P2（总出口压力）和h0（水位）求扬程 */
float CPumpSystem::convertPressureSetToHead(float total_flow, float p2, float h0) const {
    /* P2: MPa
    * h0: m
    */
    /* 计算速度水头 */
    float common_out_velosity;
    if (m_common_profile.out_pipe_dia > 1) {
        common_out_velosity = VELOSITY(total_flow, m_common_profile.out_pipe_dia);
    } else
        common_out_velosity = 0;
    float head_v = SQUARE(common_out_velosity) / 2 / 9.8;

    /* 计算位置水头 */
    float head_h = p2 * 1000 / 9.8 - h0;

    return head_v + head_h;
}

/* 检查两个水泵是否同一型号、同一安装方式 */
bool CPumpSystem::isPumpSame(const CPump &pump_1, const CPump &pump_2) {
    CPumpProfileInput::PumpProfile profile_1 = pump_1.getProfile();
    CPumpProfileInput::PumpProfile profile_2 = pump_2.getProfile();

    bool is_same = (profile_1.model == profile_2.model)
            && (profile_1.vfd_installed == profile_2.vfd_installed)
            && floatSame(profile_1.vfd_max_speed, profile_2.vfd_max_speed, 0.5)
            && floatSame(profile_1.vfd_min_speed, profile_2.vfd_min_speed, 0.5)
            && floatSame(profile_1.in_pipe_coeff, profile_2.in_pipe_coeff, profile_1.in_pipe_coeff * 0.01)
            && floatSame(profile_1.out_pipe_coeff, profile_2.out_pipe_coeff, profile_1.out_pipe_coeff)
            && floatSame(profile_1.in_pipe_dia, profile_2.out_pipe_dia, 1)
            && floatSame(profile_1.out_pipe_dia, profile_2.out_pipe_dia, 1)
            && floatSame(profile_1.in_pt_height, profile_2.in_pt_height, 0.01)
            && floatSame(profile_1.out_pt_height, profile_2.out_pt_height, 0.01);
    return is_same;
}

/* 相同水泵分组 */
void CPumpSystem::groupPumps() {
    m_group_list.clear();
    QList<CPump*> new_group;
    m_group_list.append(new_group);

    QListIterator<CPump*> pump(m_pump_list);
    while (pump.hasNext()) {
        CPump *p = pump.next();
        if (!p->getProfile().enabled)
            continue;
        QMutableListIterator<QList<CPump*>> group(m_group_list);
        while (group.hasNext()) {
            QList<CPump*> &g = group.next();
            if (g.size() < 1) {
                g.append(p);
                break;
            } else {
                if (isPumpSame(*g.first(), *p)) {
                    g.append(p);
                    break;
                } else {
                    if (!group.hasNext()) {
                        QList<CPump*> new_group;
                        new_group.append(p);
                        m_group_list.append(new_group);
                        break;
                    }
                }
            }
        }
    }

    m_grouped = true;
}

bool CPumpSystem::BestQtyWithSameModel(QList<CPump*> group, float flow, float head,
                                       CPump::CalculateResult &result, int &qty,
                                       CPump::WorkingPoint &work_point)
{
    if (group.size() < 1) {
        return false;
    }

    QVector<CPump::WorkingPoint> workpoint_list;

    for (int i=0; i < group.size(); i++)
    {
        CPump::CalculateResult sub_result;
        CPump::WorkingPoint work_point;
        work_point = group.at(0)->calculateWorkingPoint(CPump::FROM_FLOW_AND_HEAD, flow / (i+1), head, sub_result);
        if ((sub_result == CPump::NORMAL) && (work_point.speed >= group.at(0)->getProfile().vfd_min_speed)) {
            workpoint_list.append(work_point);
        } else if ((sub_result == CPump::MIN_FLOW_EXCEEDED || work_point.speed < group.at(0)->getProfile().vfd_min_speed) && i == 0 ) {
            result = CPump::MIN_FLOW_EXCEEDED;
            return false;
        } else if (sub_result == CPump::MAX_FLOW_EXCEEDED && i == group.size() - 1) {
            result = CPump::MAX_FLOW_EXCEEDED;
            return false;
        }
    }
    if (workpoint_list.size() < 1)
        return false;
    else {
        int pump_qty = 1;
        float power = workpoint_list.at(0).power;
        for (int i=1; i<workpoint_list.size(); i++) {
            if (workpoint_list.at(i).power < power) {
                power = workpoint_list.at(i).power;
                pump_qty = i+1;
            }
        }
        result = CPump::NORMAL;
        qty = pump_qty;
        work_point = workpoint_list.at(pump_qty - 1);
        return true;
    }
}

/*
* 递归算法求不同型号泵组的最小功率, 返回值为真，计算代表成功；返回值为假，代表当前泵组无法满足给定的流量和扬程。
* group_list: 可用的泵组列表。每个泵组由相同型号的水泵组成。
* flow, head: 需要满足的流量和扬程。
* best_result_list: 满足流量和扬程的计算结果，包括1个总功率，以及一个清单。
*   清单每一个元素(best_result)是其中一个泵组为满足流量和扬程的最佳水泵数和对应工作点。
*/
bool CPumpSystem::BestCombinationWithDiffModel(QList<QList<CPump*>> group_list,
                                               const float flow, const float head,
                                               CPumpSystem::PumpCombination &pump_combination) {
    //如果泵组为空，则返回false(代表无泵可用去满足流量和扬程）
    if (group_list.size() < 1)
        return false;

    //泵组清单里的第一组水泵拿出来计算。该组水泵的数量不应该为0
    QList<CPump*> group = group_list.at(0);
    group_list.removeAt(0);
    Q_ASSERT(group.size() > 0);

    //新建一个清单
    QVector<PumpCombination> pump_combination_list;

    //无变频器时的计算
    if (!group.at(0)->getProfile().vfd_installed) {
        CPump::CalculateResult result;
        CPump::WorkingPoint workpoint;
        //如果当前的工频泵组是最后一组泵，那么看一下水泵的总流量（数量分别是1,2,3..)是不是正好与需求的流量相等
        if (group_list.isEmpty()) {
            workpoint = group.at(0)->calculateWorkingPoint(CPump::FROM_HEAD_AND_SPEED, head, 100, result);
            if (result == CPump::NORMAL)
            for (int i=1; i<=group.size(); i++) {
                if ((workpoint.flow * i - flow) < 0.3) {
                    GroupWorkpoint gwp;
                    gwp.group = group;
                    gwp.workpoint = workpoint;
                    gwp.running_qty = i;
                    PumpCombination combination;
                    combination.flow = flow;
                    combination.head = head;
                    combination.power = i * gwp.workpoint.power;
                    combination.group_workpoint_list.append(gwp);
                    pump_combination_list.append(combination);
                }
            }
        }
        //如果当前的工频泵组不是最后一组泵，那么分别在运行(1,2,3..)台时，递归计算
        else {
            //计算工频泵的工作点，如果水泵计算出错，超流量或超功率或超扬程，或者流量超过总的流量，
            //代表水泵不能用，则step_flow设为无穷大，这样只执行一次流量为0的循环
            //否则的话，将step_flow定义为水泵的流量
            float step_flow;
            workpoint = group.at(0)->calculateWorkingPoint(CPump::FROM_HEAD_AND_SPEED, head, 100, result);
            if (result != CPump::NORMAL || workpoint.flow > flow)
                step_flow = FLOAT_MAX;
            else
                step_flow = workpoint.flow;
            //起始流量为0，代表当前泵组有可能不运行
            float sub_flow = 0;
            //水泵数量初始化为0台
            int running_qty = 0;
            //开始循环
            while (sub_flow < (flow + 0.1)) {
                //当前泵组的工作点放入清单
                GroupWorkpoint gwp;
                gwp.group = group;
                gwp.workpoint = workpoint;
                gwp.running_qty = running_qty;
                PumpCombination combination;
                combination.flow = sub_flow;
                combination.head = head;
                combination.power = running_qty * gwp.workpoint.power;
                combination.group_workpoint_list.append(gwp);
                //如果flow-sub_flow > 0，则基于flow-sub_flow递归计算，否则，什么也不用做
                if (flow - sub_flow > 0.1) {
                    if (BestCombinationWithDiffModel(group_list, flow - sub_flow, head, combination)) {
                        pump_combination_list.append(combination);
                    }
                } else {
                    //把本轮循环的结果放入结果清单
                    pump_combination_list.append(combination);
                }
                //流量递增
                sub_flow += step_flow;
                //水泵数量增加1台
                running_qty ++;
            }
        }
    }

    //有变频器时的计算
    else {
        CPump::CalculateResult result;
        int running_qty;
        CPump::WorkingPoint workpoint;
        GroupWorkpoint gwp;
        PumpCombination combination;
        //如果当前的变频泵组是最后一组泵，那么计算在给定流量下，效率最高的水泵组合
        if (group_list.isEmpty()) {
            if (BestQtyWithSameModel(group, flow, head, result, running_qty, workpoint)
                    && (result == CPump::NORMAL)) {
                gwp.group = group;
                gwp.running_qty = running_qty;
                gwp.workpoint = workpoint;
                combination.flow = flow;
                combination.head = head;
                combination.power = workpoint.power * running_qty;
                combination.group_workpoint_list.append(gwp);
                pump_combination_list.append(combination);
            }
        }
        //如果当前的变频泵组不是最后一组泵，那么在不同的流量下，再递归计算效率最高的组合
        else {
            const int step_numbers = 100;
            CPump::CalculateResult result_1;
            CPump::CalculateResult result_2;
            float max_flow;
            float min_flow;
            //测试水泵在100%转速时，能不能达到要求的扬程
            CPump::WorkingPoint workpoint_1 = group.at(0)->
                    calculateWorkingPoint(CPump::FROM_HEAD_AND_SPEED, head, 100, result_1);
            //测试水泵在最低转速时，流量不会超过要求的流量上限
            CPump::WorkingPoint workpoint_2 = group.at(0)->
                    calculateWorkingPoint(CPump::FROM_HEAD_AND_SPEED, head, group.at(0)->getProfile().vfd_min_speed,result_2);
            //水泵满足在流量上下限之间时，产生流量阶梯
            if (result_1 == CPump::NORMAL
                    && (result_2 == CPump::NORMAL
                        || result_2 == CPump::MAX_HEAD_EXCEEDED
                        || result_2 == CPump::MAX_FLOW_EXCEEDED)) {
                max_flow = workpoint_1.flow * group.size(); //最大转速时，全部水泵能达到的最大流量
                min_flow = workpoint_2.flow;                //最小转速时，1台水泵能达到的最小流量
                float step_flow = flow / step_numbers;
                float sub_flow = 0;
                while (sub_flow < qMin(max_flow, flow)) {
                    if (sub_flow > 0.1 && sub_flow < min_flow)
                        continue;
                    if (sub_flow < 0.1) {
                        gwp.group = group;
                        gwp.running_qty = 0;
                        combination.flow = 0;
                        combination.head = 0;
                        combination.power = 0;
                        combination.group_workpoint_list.append(gwp);
                    }
                    else {
                        if (BestQtyWithSameModel(group, sub_flow, head, result, running_qty, workpoint)
                                && (result == CPump::NORMAL)) {
                            gwp.group = group;
                            gwp.running_qty = running_qty;
                            gwp.workpoint = workpoint;
                            combination.flow = sub_flow;
                            combination.head = head;
                            combination.power = workpoint.power * running_qty;
                            combination.group_workpoint_list.append(gwp);
                        }
                    }
                    if (flow - sub_flow < 0.1) {
                        gwp.group = group;
                        gwp.running_qty = 0;
                        combination.flow = 0;
                        combination.head = 0;
                        combination.power = 0;
                        combination.group_workpoint_list.append(gwp);
                    }
                    else {
                        BestCombinationWithDiffModel(group_list, flow - sub_flow, head, combination);
                    }
                    //把本轮循环的结果放入结果清单
                    pump_combination_list.append(combination);
                    //流量递增
                    sub_flow += step_flow;
                }
            }
        }
    }

    //从得到的水泵组合中，选取功率最低的组合，附加到输入参数中的引用变量水泵组合中
    if (pump_combination_list.isEmpty())
        return false;
    PumpCombination combination;
    float mini_power = FLOAT_MAX;
    for(int i=0; i<pump_combination_list.size(); i++) {
        if (pump_combination_list.at(i).power < mini_power
                && abs(pump_combination_list.at(i).flow - flow) < 1) {
            mini_power = pump_combination_list.at(i).power;
            combination = pump_combination_list.at(i);
        }
    }
    pump_combination.flow += combination.flow;
    pump_combination.head = head;
    pump_combination.power += combination.power;
    pump_combination.group_workpoint_list += combination.group_workpoint_list;
    return true;

}
