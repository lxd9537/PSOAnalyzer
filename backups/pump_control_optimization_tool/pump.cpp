#include "pump.h"
#include "qmath.h"
#include "polyfit.h"


bool Pump::ImportPumpMotorModel(PumpPerformTestData pump_perform_data,
                                MotorPerformTestData motor_perform_data,
                                QString* error_message)
{
    if (!PumpPerformDataValidation(pump_perform_data, error_message))
        return false;
    m_pump_test_data = pump_perform_data;

    if (!MotorPerformDataValidation(motor_perform_data, error_message))
        return false;
    m_motor_test_data = motor_perform_data;

    polyfit(pump_perform_data.flow, pump_perform_data.head, POINTS_MAX, CURVE_FIT_ORDER, m_pump_motor_model.Kfactor_head_from_flow);
    polyfit(pump_perform_data.flow, pump_perform_data.NPSHR, POINTS_MAX, CURVE_FIT_ORDER, m_pump_motor_model.Kfactor_NPSHR_from_flow);
    polyfit(pump_perform_data.flow, pump_perform_data.power, POINTS_MAX, CURVE_FIT_ORDER, m_pump_motor_model.Kfactor_power_from_flow);
    polyfit(motor_perform_data.power, motor_perform_data.efficiency, MOTOR_POINTS_MAX, MOTOR_CURVE_FIT_ORDER, m_pump_motor_model.Kfactor_effi_from_power);
    polyfit(pump_perform_data.power, pump_perform_data.flow, POINTS_MAX, CURVE_FIT_ORDER, m_pump_motor_model.Kfactor_flow_from_power);
    polyfit(pump_perform_data.power, pump_perform_data.head, POINTS_MAX, CURVE_FIT_ORDER, m_pump_motor_model.Kfactor_head_from_power);

    m_pump_motor_model.vfd_installed = pump_perform_data.vfd_installed;
    m_pump_motor_model.Hz_max = pump_perform_data.Hz_max;
    m_pump_motor_model.Hz_min = pump_perform_data.Hz_min;
    m_pump_motor_model.flow_max = pump_perform_data.flow_max;
    m_pump_motor_model.flow_min = pump_perform_data.flow_min;
    m_pump_motor_model.power_rated = motor_perform_data.power_rated;

    m_pump_motor_imported = true;
    return true;
}




double Pump::GetPumpHeadFromFlow(double Hz, double flow, bool* EOC_flag)
{
    Q_ASSERT(Hz <= m_pump_motor_model.Hz_max);
    Q_ASSERT(Hz >= m_pump_motor_model.Hz_min);
    Q_ASSERT(m_pump_motor_model.Hz_max > 0);

    if(Hz <= 0)
        return 0;

    double relative_speed = Hz / m_pump_motor_model.Hz_max;

    if(EOC_flag)
    {
        if(flow > m_pump_motor_model.flow_max * relative_speed  )
            *EOC_flag = true;
        else *EOC_flag = false;
    }

    double Kfactor_head_from_flow[CURVE_FIT_ORDER+1];

    for(int i=0; i<CURVE_FIT_ORDER+1;i++)
    {
        Kfactor_head_from_flow[i] =
                m_pump_motor_model.Kfactor_head_from_flow[i] * qPow(relative_speed, 2-i);
    }

    double head = 0;
    for(int i=0;i<CURVE_FIT_ORDER+1;i++)
    {
        head += qPow(flow, i)*Kfactor_head_from_flow[i];
    }

    return head;
}

double Pump::GetPumpNpshrFromFlow(double Hz, double flow, bool* EOC_flag)
{
    Q_ASSERT(Hz <= m_pump_motor_model.Hz_max);
    Q_ASSERT(Hz >= m_pump_motor_model.Hz_min);
    Q_ASSERT(m_pump_motor_model.Hz_max > 0);

    if(Hz <= 0)
        return 0;

    double relative_speed = Hz / m_pump_motor_model.Hz_max;

    if(EOC_flag)
    {
        if(flow > m_pump_motor_model.flow_max * relative_speed  )
            *EOC_flag = true;
        else *EOC_flag = false;
    }

    double Kfactor_NPSHR_from_flow[CURVE_FIT_ORDER+1];
    for(int i=0; i<CURVE_FIT_ORDER+1;i++)
    {
        Kfactor_NPSHR_from_flow[i] =
                m_pump_motor_model.Kfactor_NPSHR_from_flow[i] * qPow(relative_speed, 2-i);
    }

    double npshr = 0;
    for(int i=0;i<CURVE_FIT_ORDER+1;i++)
    {
        npshr += qPow(flow, i)*Kfactor_NPSHR_from_flow[i];
    }

    return npshr;

}

double Pump::GetPumpPowerFromFlow(double Hz, double flow, bool* EOC_flag)
{
    Q_ASSERT(Hz <= m_pump_motor_model.Hz_max);
    Q_ASSERT(Hz >= m_pump_motor_model.Hz_min);
    Q_ASSERT(m_pump_motor_model.Hz_max > 0);

    double relative_speed = Hz / m_pump_motor_model.Hz_max;

    if(EOC_flag)
    {
        if(flow > m_pump_motor_model.flow_max * relative_speed  )
            *EOC_flag = true;
        else *EOC_flag = false;
    }

    double Kfactor_power_from_flow[CURVE_FIT_ORDER+1];
    for(int i=0;i<CURVE_FIT_ORDER+1;i++)
    {
        Kfactor_power_from_flow[i] = m_pump_motor_model.Kfactor_power_from_flow[i] * qPow(relative_speed, 3-i);
    }

    double power = 0;
    for(int i=0; i<CURVE_FIT_ORDER+1;i++)
    {
        power +=  qPow(flow, i) * Kfactor_power_from_flow[i];
    }

    return power;
}

double Pump::GetPumpEffiFromFlow(double Hz, double flow, bool* EOC_flag)
{
    double power = GetPumpPowerFromFlow(Hz, flow, EOC_flag);
    double head = GetPumpHeadFromFlow(Hz, flow, EOC_flag);
    double effi = flow * head * 9.8 / 36 / power;
    return effi;
}

double Pump::GetMotorEffiFromPower(double Hz, double power, bool* EOC_flag)
{
    Q_ASSERT(Hz <= m_pump_motor_model.Hz_max);
    Q_ASSERT(Hz >= m_pump_motor_model.Hz_min);
    Q_ASSERT(m_pump_motor_model.Hz_max > 0);

    if(EOC_flag)
    {
        if(power > m_pump_motor_model.power_rated * 1.25)
            *EOC_flag = true;
        else *EOC_flag = false;
    }

    double relative_speed = Hz / m_pump_motor_model.Hz_max;

    double effi = 0;
    double powerConverted;
    if(relative_speed >0)
        powerConverted = power / qPow(relative_speed, 3);
    else {
        powerConverted = power;
    }

    for(int i=0; i<MOTOR_CURVE_FIT_ORDER+1;i++)
        effi += m_pump_motor_model.Kfactor_effi_from_power[i] * qPow(powerConverted, i);

    return effi;
}

double Pump::GetAssemblyPowerFromFlow(double Hz, double flow, bool* EOC_flag)
{
    double input_power, output_power;
    output_power = GetPumpPowerFromFlow(Hz, flow, EOC_flag);
    double effi = 0;
    for(int i=0; i<MOTOR_CURVE_FIT_ORDER+1;i++)
    {
        effi +=  qPow(output_power, i) * m_pump_motor_model.Kfactor_effi_from_power[i];
    }
    Q_ASSERT(effi > 0);
    input_power = output_power / effi * 100;
    return input_power;
}

double Pump::GetAssemblyEffiFromFlow(double Hz, double flow, bool* EOC_flag)
{
    double power = GetAssemblyPowerFromFlow(Hz, flow, EOC_flag);
    double head = GetPumpHeadFromFlow(Hz, flow, EOC_flag);
    double effi = flow * head * 9.8 / 36 / power;
    return effi;
}

double Pump::GetAssemblyFlowFromPower(double Hz, double power, bool* EOC_flag)
{
    Q_ASSERT(Hz <= m_pump_motor_model.Hz_max);
    Q_ASSERT(Hz >= m_pump_motor_model.Hz_min);
    Q_ASSERT(m_pump_motor_model.Hz_max > 0);

    double relative_speed = Hz / m_pump_motor_model.Hz_max;

    if(EOC_flag)
    {
        if(power > m_pump_motor_model.power_rated * relative_speed)
            *EOC_flag = true;
        else *EOC_flag = false;
    }

    double Kfactor_flow_from_power[CURVE_FIT_ORDER+1];
    for(int i=0; i<CURVE_FIT_ORDER+1; i++)
    {
        int m = 1 - i*3;
        Kfactor_flow_from_power[i] = m_pump_motor_model.Kfactor_flow_from_power[i]
                * qPow(relative_speed, m);
    }

    double flow = 0;
    for(int i=0; i<CURVE_FIT_ORDER+1; i++)
    {
        flow +=  qPow(power, i) * Kfactor_flow_from_power[i];
    }

    if (flow > 0)
        return flow;
    else
        return 0;
}

double Pump::GetAssemblyHeadFromPower(double Hz, double power, bool* EOC_flag)
{
    Q_ASSERT(Hz <= m_pump_motor_model.Hz_max);
    Q_ASSERT(Hz >= m_pump_motor_model.Hz_min);
    Q_ASSERT(m_pump_motor_model.Hz_max > 0);

    double relative_speed = Hz / m_pump_motor_model.Hz_max;

    if(EOC_flag)
    {
        if(power > m_pump_motor_model.power_rated * relative_speed)
            *EOC_flag = true;
        else *EOC_flag = false;
    }

    double Kfactor_head_from_power[CURVE_FIT_ORDER+1];
    for(int i=0; i<CURVE_FIT_ORDER+1; i++)
    {
        int m = 2 - i*3;
        Kfactor_head_from_power[i] = m_pump_motor_model.Kfactor_head_from_power[i]
                * qPow(relative_speed, m);
    }

    double head = 0;
    for(int i=0; i<CURVE_FIT_ORDER+1; i++)
    {
        head +=  qPow(power, i) * Kfactor_head_from_power[i];
    }

    if (head > 0)
        return head;
    else
        return 0;
}

double Pump::GetFlowAtZeroHead(double Hz)
{
    Q_ASSERT(Hz <= m_pump_motor_model.Hz_max);
    Q_ASSERT(Hz >= m_pump_motor_model.Hz_min);
    Q_ASSERT(m_pump_motor_model.Hz_max > 0);

    //得到0扬程流量
    double  low_flow = 0;
    double  high_flow = m_pump_motor_model.flow_max * 5;
    double  middle_flow = high_flow / 2;
    int     cycles = 0;

    do {
        double head_calcu = 0;
        for(int i=0; i<CURVE_FIT_ORDER+1; i++)
            head_calcu += m_pump_motor_model.Kfactor_head_from_flow[i] * pow(middle_flow, i);

        if (abs(head_calcu) <= 0.1)
            break;
        else
            if (head_calcu > 0.1)
                low_flow = middle_flow;
            else
                high_flow = middle_flow;

        middle_flow = (high_flow + low_flow) / 2;

    } while (cycles++ < 30);

    double flow_at_zero_head = Hz
            / m_pump_motor_model.Hz_max
            * middle_flow;

    return flow_at_zero_head;
}
double Pump::GetHeadAtZeroFlow(double Hz)
{
    /*
    Q_ASSERT(Hz <= m_assembly_model.pump_model.Hz_max);
    Q_ASSERT(Hz >= m_assembly_model.pump_model.Hz_min);
    Q_ASSERT(m_assembly_model.pump_model.Hz_max > 0);

    double head_at_zero_flow = qPow(Hz
            / m_assembly_model.pump_model.Hz_max, 2)
            * m_assembly_model.pump_model.head_at_zero_flow;
    return head_at_zero_flow;
    */
}

double Pump::GetFlowFromHead(double Hz, double head, bool* EOC_flag)
{

    Q_ASSERT(Hz <= m_pump_motor_model.Hz_max);
    Q_ASSERT(Hz >= m_pump_motor_model.Hz_min);
    Q_ASSERT(m_pump_motor_model.Hz_max > 0);

    //if Hz is very low, the flow should be very low
    if(Hz<1)
        return 0;

    double relative_speed = Hz / m_pump_motor_model.Hz_max;

    //超曲线
    if (head < GetPumpHeadFromFlow(Hz, m_pump_motor_model.flow_max, EOC_flag))
    {
        if(EOC_flag) *EOC_flag = true;
        return m_pump_motor_model.flow_max * relative_speed;
    }
    //闭死点
    else
        if (head > GetPumpHeadFromFlow(Hz, 0, EOC_flag))
            return 0;

    //曲线上
    double low_flow = 0;
    double flow_at_zero_head = GetFlowAtZeroHead(Hz);
    double high_flow = flow_at_zero_head < 0.1 ? m_pump_motor_model.flow_max * 5 : flow_at_zero_head;
    double middle_flow = high_flow / 2;
    int cycles = 0;

    if(head<0.01)
        head = 0.01;

    do {
        double head_calcu = GetPumpHeadFromFlow(Hz, middle_flow, EOC_flag);

        if (qAbs(head_calcu - head) <= (0.0002 * head))
            return middle_flow;
        else
            if (head_calcu > head)
                low_flow = middle_flow;
            else
                high_flow = middle_flow;

        middle_flow = (high_flow + low_flow) / 2;

    } while (cycles++ < 50);

    return middle_flow;

}

double Pump::CalcuHz(double flow, double head, bool* upper_limit_flag, bool* lower_limit_flag)
{
    Q_ASSERT(m_pump_motor_model.Hz_max > 0);
    Q_ASSERT(m_pump_motor_model.Hz_min < m_pump_motor_model.Hz_max);

    if(upper_limit_flag) *upper_limit_flag = false;
    if(lower_limit_flag) *lower_limit_flag = false;

    //达上限
    if (GetPumpHeadFromFlow(m_pump_motor_model.Hz_max, flow) < head)
    {
        if(upper_limit_flag) *upper_limit_flag = true;
        return m_pump_motor_model.Hz_max;
    }

    //达下限
    if (GetPumpHeadFromFlow(m_pump_motor_model.Hz_min, flow) > head)
    {
        if(lower_limit_flag) *lower_limit_flag = true;
        return m_pump_motor_model.Hz_min;
    }

    //在中间
    double low_Hz = m_pump_motor_model.Hz_min;
    double high_Hz = m_pump_motor_model.Hz_max;
    double middle_Hz = (m_pump_motor_model.Hz_min + m_pump_motor_model.Hz_max) / 2;
    int cycles = 0;
    do {
        double head_calcu = GetPumpHeadFromFlow(middle_Hz, flow);

        if (qAbs(head_calcu - head) < (0.0001 * head))
            return middle_Hz;
        else
            if (head_calcu > head)
                high_Hz = middle_Hz;
            else
                low_Hz = middle_Hz;

        middle_Hz = (high_Hz + low_Hz) / 2;

    } while (cycles++ < 50);

    return -1;
}

bool Pump::PumpPerformDataValidation(PumpPerformTestData perform_data, QString* error_message)
{
    //flow order check
    for(int i=0;i<POINTS_MAX-1;i++)
        for(int j=i+1;j<POINTS_MAX;j++)
            if(perform_data.flow[i] >= perform_data.flow[j])
            {*error_message = QObject::tr("Flows must be in ascending order!");return false;}

    //sign check
    if(perform_data.flow[0] < 0)
            {*error_message = QObject::tr("Pump flow, head and power should not below 0!");return false;}

    for(int i=0;i<POINTS_MAX;i++)
         if(perform_data.head[i]<=0 || perform_data.power[i]<=0)
            {*error_message = QObject::tr("Pump flow, head and power should not below 0!");return false;}

    //upperlimit check
    for(int i=0;i<POINTS_MAX;i++)
        if(perform_data.flow[i]>99999 || perform_data.head[i]>99999 || perform_data.power[i]>99999)
            {*error_message = QObject::tr("Pump flow or head or power should not be greater than 99999!");return false;}

    //check hz max/min
    if (perform_data.vfd_installed)
        if(perform_data.Hz_max < 1 || perform_data.Hz_min < 0 || perform_data.Hz_min >= perform_data.Hz_max)
             {*error_message = QObject::tr("Wrong input of pump Hz maximum or Hz minimum!");return false;}

    //check flow max/min
    if(perform_data.flow_max <= 0 || perform_data.flow_min < 0 || perform_data.flow_min >= perform_data.flow_max)
             {*error_message = QObject::tr("Wrong input of pump flow maximum or flow minimum!");return false;}

    return true;
}

bool Pump::MotorPerformDataValidation(MotorPerformTestData perform_data, QString* error_message)
{
    //power order check
    for(int i=0;i<MOTOR_POINTS_MAX-1;i++)
        for(int j=i+1;j<MOTOR_POINTS_MAX;j++)
            if(perform_data.power[i] >= perform_data.power[j])
            {*error_message = QObject::tr("Power must be in ascending order!");return false;}

    //sign check
    if(perform_data.power[0] < 0)
            {*error_message = QObject::tr("Motor power should not below 0!");return false;}

    for(int i=0;i<MOTOR_POINTS_MAX;i++)
         if(perform_data.efficiency[i]<=0)
            {*error_message = QObject::tr("Motor efficiency should not below 0!");return false;}

    //upperlimit check
    for(int i=0;i<MOTOR_POINTS_MAX;i++)
        if(perform_data.power[i]>99999 || perform_data.efficiency[i]>100)
            {*error_message = QObject::tr("Motor efficiency should not be greater than 100!");return false;}

     return true;
}
