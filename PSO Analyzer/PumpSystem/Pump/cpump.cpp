#include "cpump.h"
#define SQUARE(x) (x*x)

CPump::CPump()
{

}

/* 验证性能数据的合法性 */
bool CPump::testDataValidation(const PerformTestData &perform_data, QString *error_message) {
    //flow order check
    for(int i=0;i<perform_data.points-1;i++)
        for(int j=i+1;j<perform_data.points;j++)
            if(perform_data.flow[i] >= perform_data.flow[j]) {
                if (error_message)
                    *error_message = "流量未按升序排列!";
                return false;
            }

    //sign check
    if(perform_data.flow[0] < 0) {
        if (error_message)
            *error_message = "测试数据小于零!";
        return false;
    }

    for(int i=0;i<perform_data.points;i++)
        if(perform_data.head[i]<=0 || perform_data.power[i]<=0) {
            if (error_message)
                *error_message = "测试数据小于零!";
            return false;
        }

    //upperlimit check
    for(int i=0;i<perform_data.points;i++)
        if(perform_data.flow[i]>99999 || perform_data.head[i]>99999 || perform_data.power[i]>99999) {
            if (error_message)
                *error_message = "测试数据超过上限99999!";
            return false;
        }

    //check flow max/min
    if(perform_data.flow_min < 0
            || perform_data.flow_min >= perform_data.flow_max)
    {
        if (error_message)
            *error_message = "最大流量不大于最小流量!";
        return false;
    }

    //check points
    if (perform_data.points < POINTS_MIN || perform_data.points > POINTS_MAX) {
        if (error_message)
            *error_message = QString("数据行数小于%1或大于%2!").arg(QString::number(POINTS_MIN), QString::number(perform_data.points));
        return false;
    }

    return true;
}

/* 将水泵数据存入类对象，并转化为拟合系数 */
bool CPump::createModel(const PerformTestData &test_data, QString *error_message) {
    if (!testDataValidation(test_data, error_message))
        return false;

    m_test_data = test_data;

    polyfit(test_data.flow, test_data.head, test_data.points, POLYFIT_ORDER, m_pump_coeffs.coeffs_head_to_flow);
    polyfit(test_data.flow, test_data.effi, test_data.points, POLYFIT_ORDER, m_pump_coeffs.coeffs_effi_to_flow);
    polyfit(test_data.flow, test_data.power, test_data.points, POLYFIT_ORDER, m_pump_coeffs.coeffs_power_to_flow);
    polyfit(test_data.flow, test_data.npshr, test_data.points, POLYFIT_ORDER, m_pump_coeffs.coeffs_npshr_to_flow);

    m_pump_model_created = true;
    return true;
}

/* 导入水泵安装特性 */
void CPump::setProfile(const CPumpProfileInput::PumpProfile &profile) {
    m_profile = profile;
    m_pump_profile_set = true;
}

/* 通过流量扬程计算效率
*  此处的效率是不包含进出口管压力损失的效率
*/
double CPump::effi(double flow, double head, double power) {
    if (power < 0.001)
        return 0;

    return flow * head * 9.8 / power / 36;
}

/* 通过拟合计算扬程
*  flow: 水泵流量(m3/h)
*  speed: 转速百分比(0-100)
*  返回值：减去了进出口管的压力损失后的整体扬程
*/

double CPump::calcuHead(double flow, double speed, CalculateResult &result) {
    result = NORMAL;
    double correct_speed;
    if (speed < 0)
        correct_speed = 0;
    else if (speed > 100)
        correct_speed = 100;
    else
        correct_speed = speed;

    double relative_speed = correct_speed / 100;
    if(flow > m_test_data.flow_max * relative_speed) {
        result = MAX_FLOW_EXCEEDED;
        return 0;
    } else if (flow < m_test_data.flow_min * relative_speed) {
        result = MIN_FLOW_EXCEEDED;
        return 0;
    }

    /* calculate head */
    double coeffs[POLYFIT_ORDER + 1];
    for(int i=0; i<POLYFIT_ORDER + 1; i++) {
        coeffs[i] = m_pump_coeffs.coeffs_head_to_flow[i] * qPow(relative_speed, 2 - i);
    }
    double head = 0;
    for(int i=0; i<POLYFIT_ORDER + 1; i++){
        head += qPow(flow, i) * coeffs[i];
    }

    return head - pressureLoss(flow);
}

/* 通过计算扬程功率计算效率 */
double CPump::calcuEffi(double flow, double speed, CalculateResult &result) {
    result = NORMAL;
    double correct_speed;
    if (speed < 0)
        correct_speed = 0;
    else if (speed > 100)
        correct_speed = 100;
    else
        correct_speed = speed;

    double relative_speed = correct_speed / 100;
    if(flow > m_test_data.flow_max * relative_speed) {
        result = MAX_FLOW_EXCEEDED;
        return 0;
    } else if (flow < m_test_data.flow_min * relative_speed) {
        result = MIN_FLOW_EXCEEDED;
        return 0;
    }

    /* calculate effi */
    float head = calcuHead(flow, speed, result);
    if (result != NORMAL) {
        return 0;
    }

    float power = calcuPower(flow, speed, result);
    if (result != NORMAL) {
        return 0;
    }

    return effi(flow, head, power);
}

/* 通过拟合计算功率 */
double CPump::calcuPower(double flow, double speed, CalculateResult &result) {
    result = NORMAL;
    double correct_speed;
    if (speed < 0)
        correct_speed = 0;
    else if (speed > 100)
        correct_speed = 100;
    else
        correct_speed = speed;

    double relative_speed = correct_speed / 100;
    if(flow > m_test_data.flow_max * relative_speed) {
        result = MAX_FLOW_EXCEEDED;
        return 0;
    } else if (flow < m_test_data.flow_min * relative_speed) {
        result = MIN_FLOW_EXCEEDED;
        return 0;
    }

    /* calculate head */
    double coeffs[POLYFIT_ORDER + 1];
    for(int i=0; i<POLYFIT_ORDER + 1; i++) {
        coeffs[i] = m_pump_coeffs.coeffs_power_to_flow[i] * qPow(relative_speed, 3 - i);
    }
    double power = 0;
    for(int i=0; i<POLYFIT_ORDER + 1; i++){
        power += qPow(flow, i) * coeffs[i];
    }

    return power;
}

/* 通过拟合计算NPSHr
* 计算的结果加上了进水管路的压降
*
*/
double CPump::calcuNpshr(double flow, double speed, CalculateResult &result) {
    result = NORMAL;
    double correct_speed;
    if (speed < 0)
        correct_speed = 0;
    else if (speed > 100)
        correct_speed = 100;
    else
        correct_speed = speed;

    double relative_speed = correct_speed / 100;
    if(flow > m_test_data.flow_max * relative_speed) {
        result = MAX_FLOW_EXCEEDED;
        return 0;
    } else if (flow < m_test_data.flow_min * relative_speed) {
        result = MIN_FLOW_EXCEEDED;
        return 0;
    }

    /* calculate head */
    double coeffs[POLYFIT_ORDER + 1];
    for(int i=0; i<POLYFIT_ORDER + 1; i++) {
        coeffs[i] = m_pump_coeffs.coeffs_npshr_to_flow[i] * qPow(relative_speed, 2 - i);
    }
    double npshr = 0;
    for(int i=0; i<POLYFIT_ORDER + 1; i++){
        npshr += qPow(flow, i) * coeffs[i];
    }

    return npshr + pressureLoss(flow);
}

/* 计算工作点的入口函数 */
CPump::WorkingPoint CPump::calculateWorkingPoint(const CalculateType &calculate_type, const double &para_1, const double &para_2, CalculateResult &result) {
    WorkingPoint work_point;
    switch(calculate_type) {
    case FROM_FLOW_AND_HEAD:
        work_point = calcuWorkPointfromFlowAndHead(para_1, para_2, result);
        break;
    case FROM_FLOW_AND_SPEED:
        work_point = calcuWorkPointfromFlowAndSpeed(para_1, para_2, result);
        break;
    case FROM_HEAD_AND_SPEED:
        work_point = calcuWorkPointfromHeadAndSpeed(para_1, para_2, result);
        break;
    }
    return work_point;
}

/* 根据流量和profile，计算水泵的压力损失
* flow: 水泵流量(m3/h)
* 返回值: 压力损失(m)
*/
double CPump::pressureLoss(double flow) {
    Q_ASSERT(isProfileSet());
    float head_ol = m_profile.out_pipe_coeff * SQUARE(flow) / 9.8 / 1000;    //出口沿程损失
    float head_il = m_profile.in_pipe_coeff * SQUARE(flow) / 9.8 / 1000;     //进口沿程损失
    return head_ol + head_il;
}

/* 根据流量和转速计算工作点 */
CPump::WorkingPoint CPump::calcuWorkPointfromFlowAndSpeed(const double &flow, const double &speed, CalculateResult &result) {
    WorkingPoint work_point = {0,0,0,0,0,0};
    result = NORMAL;
    double correct_speed;
    if (speed < 0)
        correct_speed = 0;
    else if (speed > 100)
        correct_speed = 100;
    else
        correct_speed = speed;

    work_point.flow = flow;
    work_point.speed = correct_speed;

    double relative_speed = work_point.speed / 100;
    if(flow > m_test_data.flow_max * relative_speed) {
        result = MAX_FLOW_EXCEEDED;
        return work_point;
    } else if (flow < m_test_data.flow_min * relative_speed) {
        result = MIN_FLOW_EXCEEDED;
        return work_point;
    }

    /* calculate head */
    double coeffs[POLYFIT_ORDER + 1];
    for(int i=0; i<POLYFIT_ORDER + 1; i++) {
        coeffs[i] = m_pump_coeffs.coeffs_head_to_flow[i] * qPow(relative_speed, 2 - i);
    }
    work_point.head = 0;
    for(int i=0; i<POLYFIT_ORDER + 1; i++){
        work_point.head += qPow(flow, i) * coeffs[i];
    }

    /* calculate power */
    for(int i=0; i<POLYFIT_ORDER + 1; i++) {
        coeffs[i] = m_pump_coeffs.coeffs_power_to_flow[i] * qPow(relative_speed, 3 - i);
    }
    work_point.power = 0;
    for(int i=0; i<POLYFIT_ORDER + 1; i++){
        work_point.power += qPow(flow, i) * coeffs[i];
    }

    /* calculate npshr */
    for(int i=0; i<POLYFIT_ORDER + 1; i++) {
        coeffs[i] = m_pump_coeffs.coeffs_npshr_to_flow[i] * qPow(relative_speed, 2 - i);
    }
    work_point.npshr = 0;
    for(int i=0; i<POLYFIT_ORDER + 1; i++){
        work_point.npshr += qPow(flow, i) * coeffs[i];
    }

    /* calculate efficiency */
    work_point.effi = effi(work_point.flow, work_point.head, work_point.power);

    return work_point;
}

/* 根据流量和扬程计算工作点 */
CPump::WorkingPoint CPump::calcuWorkPointfromFlowAndHead(const double &flow, const double &head, CalculateResult &result) {
    WorkingPoint work_point {0,0,0,0,0,0};
    result = NORMAL;

    /* 超过最大扬程 */
    double head_at_max_speed = calcuHead(flow, 100, result);
    if (result!=NORMAL) {
        return work_point;
    } else if (head_at_max_speed < head) {
        result = MAX_HEAD_EXCEEDED;
        return work_point;
    }

    /* 在中间 */
    double low_speed = 0;
    double high_speed = 100;
    double middle_speed = (high_speed + low_speed) / 2;
    int cycles = 0;
    do {
        double head_calcu = calcuHead(flow, middle_speed, result);
        //if (result != NORMAL)
        //    return work_point;
        if (qAbs(head_calcu - head) < (0.0001 * head))
            break;
        else
            if (head_calcu > head)
                high_speed = middle_speed;
            else
                low_speed = middle_speed;

        middle_speed = (high_speed + low_speed) / 2;

    } while (cycles++ < 50);

    work_point = calcuWorkPointfromFlowAndSpeed(flow, middle_speed, result);

    return work_point;
}

/* 根据扬程和转速计算工作点 */
CPump::WorkingPoint CPump::calcuWorkPointfromHeadAndSpeed(const double &head, const double &speed, CalculateResult &result) {
    WorkingPoint work_point = {0,0,0,0,0,0};
    result = NORMAL;

    double correct_speed;
    if (speed < 0)
        correct_speed = 0;
    else if (speed > 100)
        correct_speed = 100;
    else
        correct_speed = speed;

    work_point.head = head;
    work_point.speed = correct_speed;

    double head_at_max_flow = calcuHead(m_test_data.flow_max, speed, result);
    if (result!=NORMAL)
        return work_point;
    else if (head_at_max_flow > head) {
        result = MAX_FLOW_EXCEEDED;
        return work_point;
    } else {
        double head_at_min_flow = calcuHead(m_test_data.flow_min, speed, result);
        if (head_at_min_flow < head) {
            result = MAX_HEAD_EXCEEDED;
            return work_point;
        }
    }

    //曲线上
    double low_flow = m_test_data.flow_min;
    double high_flow = m_test_data.flow_max * correct_speed / 100;
    double middle_flow = high_flow / 2;
    int cycles = 0;

    do {
        double head_calcu = calcuHead(middle_flow, speed, result);
        if (result != NORMAL)
            return work_point;

        if (qAbs(head_calcu - head) <= (0.0002 * head))
            break;
        else
            if (head_calcu > head)
                low_flow = middle_flow;
            else
                high_flow = middle_flow;

        middle_flow = (high_flow + low_flow) / 2;

    } while (cycles++ < 50);

    work_point = calcuWorkPointfromFlowAndSpeed(middle_flow, speed, result);
    return work_point;
}

/* 进出口压力差转换为水泵扬程
* flow: 水泵流量(m3/h)
* outlet_press: 水泵出水压力(MPa)
* inlet_press： 水泵进水压力(MPa)
*/
float CPump::actualPumpHead(const float &flow, const float &outlet_press, const float &inlet_press) {

    /* elevation: mm
    * diameter: mm
    * flow: m3/h
    * press: kPa
    * head: m */

    Q_ASSERT(isModelCreated());
    Q_ASSERT(isProfileSet());

    float head_p = (outlet_press - inlet_press) * 1000 / 998 / 9.8;
    float head_g = m_profile.out_pt_height - m_profile.in_pt_height;
    float head_v;
    if (m_profile.out_pipe_dia > 1 && m_profile.in_pipe_dia > 1) {
        float in_velosity = flow / 3600 / (m_profile.in_pipe_dia/1000 * m_profile.in_pipe_dia/1000 * 3.14 / 4);
        float out_velosity = flow / 3600 / (m_profile.out_pipe_dia/1000 * m_profile.out_pipe_dia/1000 *3.14 / 4);
        head_v = (out_velosity * out_velosity  - in_velosity * in_velosity) / 2/ 9.8;
    } else
        head_v = 0;

    return head_p + head_g + head_v;

}

/* 包含进出口沿程损失及取水池液位，计算水泵扬程
* flow: 水泵流量(m3/h)
* outlet_press: 水泵出口压力(MPa)
* in_tank_level: 水泵取水池液位(m_
*/
float CPump::calcuPumpHead(const float &flow,
                            const float &outlet_press,
                            const float &in_tank_level) {

    /* elevation: mm
    * diameter: mm
    * flow: m3/h
    * press: kPa
    * head: m */

    Q_ASSERT(isModelCreated());
    Q_ASSERT(isProfileSet());

    float head_p = outlet_press * 1000 / 998 / 9.8; //压强水头
    float head_g = m_profile.out_pt_height - in_tank_level; //位置水头
    float head_v;   //速度水头
    if (m_profile.out_pipe_dia > 0.001) {
        float out_velosity = flow / 3600 / (SQUARE(m_profile.out_pipe_dia/1000) *3.14 / 4);
        head_v = SQUARE(out_velosity) / 2/ 9.8;
    } else
        head_v = 0;
    float head_ol = m_profile.out_pipe_coeff * SQUARE(flow) / 9.8 / 1000;    //出口沿程损失
    float head_il = m_profile.in_pipe_coeff * SQUARE(flow) / 9.8 / 1000;     //进口沿程损失

    return head_p + head_g + head_v + head_ol + head_il;
}
