#include "pump.h"
#include "polyfit.h"
#include "qmath.h"

Pump::Pump()
{

}

bool Pump::WritePerformance(RawPerformData perform_data, int* error_code,QString* error_message)
{
    if(!ImportDataValidation(perform_data,error_code,error_message))
        return false;

    polyfit(perform_data.flow, perform_data.head, 5, 3, m_perform_data.Kfactor_head_from_flow);
    polyfit(perform_data.flow, perform_data.power, 5, 3, m_perform_data.Kfactor_power_from_flow);

    m_perform_data.Hz_min = perform_data.Hz_min;
    m_perform_data.Hz_max = perform_data.Hz_max;

    m_perform_data.flow_min = perform_data.flow[0];
    m_perform_data.flow_max = perform_data.flow[4];


    m_perform_data.head_min = perform_data.head[4];
    m_perform_data.head_max = perform_data.head[0];
    m_perform_data.power_min = perform_data.power[0];
    m_perform_data.power_max = perform_data.power[4];

    for(int i=1; i<5; i++)
    {
        m_perform_data.head_min = qMin(m_perform_data.head_min,perform_data.head[i]);
        m_perform_data.head_max = qMax(m_perform_data.head_max, perform_data.head[0]);
        m_perform_data.power_min = qMin(m_perform_data.power_min, perform_data.power[0]);
        m_perform_data.power_max = qMax(m_perform_data.power_max, perform_data.power[0]);
   }

    m_perform_data.vfd_ascending_time = perform_data.vfd_ascending_time;
    m_perform_data.vfd_descending_time = perform_data.vfd_descending_time;

    m_perform_data.flow_at_zero_head = GetFlowFromHead(50,1);
    m_perform_data.head_at_zero_flow = GetHeadFromFlow(50,0);

    m_imported_flag = true;

    return true;
}
bool Pump::ReadImportedFlag()
{
    return m_imported_flag;
}
Pump::PerformData Pump::ReadPerformance()
{
    return m_perform_data;
}
Pump::RoData Pump::ReadRoData()
{
    return m_ro_data;
}
Pump::RwData Pump::ReadRwData()
{
    return m_rw_data;
}
void Pump::WriteRWData(RwData &rw_data)
{
    if(rw_data.man_Hz > m_perform_data.Hz_max)
        rw_data.man_Hz = m_perform_data.Hz_max;

    if(rw_data.auto_Hz > m_perform_data.Hz_max)
        rw_data.auto_Hz = m_perform_data.Hz_max;

    if(rw_data.man_Hz < m_perform_data.Hz_min)
        rw_data.man_Hz = m_perform_data.Hz_min;

    if(rw_data.auto_Hz < m_perform_data.Hz_min)
        rw_data.auto_Hz = m_perform_data.Hz_min;

    m_rw_data = rw_data;
}
void Pump::SetPumpNumber(int pump_number)
{
    m_pump_number = pump_number;
}
double Pump::GetHeadFromFlow(double Hz, double flow, bool* EOC_flag)
{
    //if(Hz > m_perform_data.Hz_max)
      //  Hz = m_perform_data.Hz_max;
    //else if(Hz < m_perform_data.Hz_min)
      //  Hz = m_perform_data.Hz_min;

    double relative_speed = Hz / m_perform_data.Hz_max;

    if(EOC_flag)
    {
        if(flow*relative_speed > m_perform_data.flow_max)
            *EOC_flag = true;
        else *EOC_flag = false;
    }

    double Kfactor_head_from_flow[4];
    Kfactor_head_from_flow[3] = m_perform_data.Kfactor_head_from_flow[3] * qPow(relative_speed, -1);
    Kfactor_head_from_flow[2] = m_perform_data.Kfactor_head_from_flow[2] * qPow(relative_speed, 0);
    Kfactor_head_from_flow[1] = m_perform_data.Kfactor_head_from_flow[1] * qPow(relative_speed, 1);
    Kfactor_head_from_flow[0] = m_perform_data.Kfactor_head_from_flow[0] * qPow(relative_speed, 2);

    double head = Kfactor_head_from_flow[0]
            + qPow(flow, 1) * Kfactor_head_from_flow[1]
            + qPow(flow, 2) * Kfactor_head_from_flow[2]
            + qPow(flow, 3) * Kfactor_head_from_flow[3];

    //if(head<0) head = 0;

    return head;
}
double Pump::GetFlowFromHead(double Hz, double head, bool* EOC_flag)
{
    //if Hz is very low, the flow should be very low
    if(Hz<1)
        return 0;

    //double relative_speed = Hz / m_perform_data.Hz_max;
    if(EOC_flag)*EOC_flag = false;

    //超曲线
    /*if (head < GetHeadFromFlow(Hz,m_perform_data.flow_max))
    {
        if(EOC_flag) *EOC_flag = true;
        return m_perform_data.flow_max * relative_speed;
    }*/

    //闭死点
    if (head > GetHeadFromFlow(Hz, 0))
        return 0;

    //曲线上
    double low_flow = 0;
    double high_flow = m_perform_data.flow_at_zero_head < 0.1 ? m_perform_data.flow_max * 5 : m_perform_data.flow_at_zero_head;
    double middle_flow = high_flow / 2;
    int cycles = 0;

    if(head<0.01)
        head = 0.01;

    do {
        double head_calcu = GetHeadFromFlow(Hz, middle_flow);

        if (qAbs(head_calcu - head) <= (0.0002 * head))
            return middle_flow;
        else
            if (head_calcu > head)
                low_flow = middle_flow;
            else
                high_flow = middle_flow;

        middle_flow = (high_flow + low_flow) / 2;

    } while (cycles++ < 30);

    return middle_flow;
}
double Pump::GetPowerFromFlow(double Hz, double flow, bool* EOC_flag)
{
    /*if(Hz > m_perform_data.Hz_max)
        Hz = m_perform_data.Hz_max;
    else if(Hz < m_perform_data.Hz_min)
        Hz = m_perform_data.Hz_min;*/

    double relative_speed = Hz / m_perform_data.Hz_max;

    if(EOC_flag)
    {
        if(flow*relative_speed > m_perform_data.flow_max)
            *EOC_flag = true;
        else *EOC_flag = false;
    }

    double Kfactor_power_from_flow[4];
    Kfactor_power_from_flow[3] = m_perform_data.Kfactor_power_from_flow[3] * qPow(relative_speed, 0);
    Kfactor_power_from_flow[2] = m_perform_data.Kfactor_power_from_flow[2] * qPow(relative_speed, 1);
    Kfactor_power_from_flow[1] = m_perform_data.Kfactor_power_from_flow[1] * qPow(relative_speed, 2);
    Kfactor_power_from_flow[0] = m_perform_data.Kfactor_power_from_flow[0] * qPow(relative_speed, 3);

    double power = Kfactor_power_from_flow[0]
            + qPow(flow, 1) * Kfactor_power_from_flow[1]
            + qPow(flow, 2) * Kfactor_power_from_flow[2]
            + qPow(flow, 3) * Kfactor_power_from_flow[3];

    //if(power<0) power = 0;
    return power;
}
double Pump::CalcuHz(double flow, double head, bool* upper_limit_flag, bool* lower_limit_flag)
{
    *upper_limit_flag = false;
    *lower_limit_flag = false;

    //达上限
    if (GetHeadFromFlow(m_perform_data.Hz_max, flow) < head)
    {
        if(!upper_limit_flag) *upper_limit_flag = true;
        return m_perform_data.Hz_max;
    }

    //达下限
    if (GetHeadFromFlow(m_perform_data.Hz_min, flow) > head)
    {
        if(!lower_limit_flag) *lower_limit_flag = true;
        return m_perform_data.Hz_min;
    }

    //在中间
    double low_Hz = m_perform_data.Hz_min;
    double high_Hz = m_perform_data.Hz_max;
    double middle_Hz = (m_perform_data.Hz_min + m_perform_data.Hz_max) / 2;
    int cycles = 0;
    do {
        double head_calcu = GetHeadFromFlow(middle_Hz, flow);

        if (qAbs(head_calcu - head) < (0.01 * head))
            return middle_Hz;
        else
            if (head_calcu > head)
                high_Hz = middle_Hz;
            else
                low_Hz = middle_Hz;

        middle_Hz = (high_Hz + low_Hz) / 2;

    } while (cycles++ < 20);

    return -1;
}
double Pump::GetFlowAtZeroHead()
{
    return m_ro_data.Hz / m_perform_data.Hz_max * m_perform_data.flow_at_zero_head;
}
double Pump::GetHeadAtZeroFlow()
{
    return pow(m_ro_data.Hz / m_perform_data.Hz_max,2) * m_perform_data.head_at_zero_flow;
}
void Pump::SetDifferentialPressure(double dp)
{
    m_diffrential_pressure = dp;
    if(m_ro_data.running)
        m_ro_data.head = dp;
}

void Pump::Reset()
{
    m_ro_data = {false,false,0,0,0,0};
    m_rw_data = {false,Auto,false,0,0};
    m_diffrential_pressure = 0;
}

void Pump::Run_cycle(int period_ms)
{

    //pump head/flow/power
    if(m_ro_data.running)
    {
        m_ro_data.head = m_diffrential_pressure;
        m_ro_data.flow = GetFlowFromHead(m_ro_data.Hz,m_ro_data.head);
        m_ro_data.power = GetPowerFromFlow(m_ro_data.Hz,m_ro_data.flow);
    }
    else {
        m_ro_data.head = 0;
        m_ro_data.flow = 0;
        m_ro_data.power = 0;
    }
    //emit change signal
    if(CheckChange())
        emit StatusChanged(m_pump_number);

    m_ro_data_old = m_ro_data;
    m_rw_data_old = m_rw_data;

    //pump runnning status
    m_ro_data.running =
            ((m_rw_data.Moa == Man)
            || ((m_rw_data.Moa == Auto)&&m_rw_data.run_request))
            && !m_ro_data.fault;

    //pump speed
    double required_Hz;
    if(m_ro_data.running)
    {
        if(m_rw_data.Moa == Auto)
        {
            required_Hz = m_rw_data.auto_Hz;
            m_rw_data.man_Hz = m_rw_data.auto_Hz;
        }
        else
        {
            required_Hz = m_rw_data.man_Hz;
            m_rw_data.auto_Hz = m_rw_data.man_Hz;
        }

    }
    else required_Hz = 0;

    if(required_Hz < m_perform_data.Hz_min && m_ro_data.running)
        required_Hz = m_perform_data.Hz_min;

    if (required_Hz > m_perform_data.Hz_max)
        required_Hz = m_perform_data.Hz_max;

    if(m_ro_data.Hz < required_Hz)
    {
        m_ro_data.Hz += m_perform_data.Hz_max/m_perform_data.vfd_ascending_time * period_ms /1000;
        if (m_ro_data.Hz > required_Hz) m_ro_data.Hz = required_Hz;
    }
    else if (m_ro_data.Hz > required_Hz)
    {
        m_ro_data.Hz -= m_perform_data.Hz_max/m_perform_data.vfd_descending_time * period_ms /1000;
        if (m_ro_data.Hz < required_Hz) m_ro_data.Hz = required_Hz;
    }

    //pump fault
    m_ro_data.fault = m_rw_data.trip;



}
bool Pump::ImportDataValidation(RawPerformData pump_performance, int* error_code, QString* error_message)
{
    int errors = 0;

    //flow order check
    for(int i=0;i<4;i++)
        for(int j=i+1;j<5;j++)
            if(pump_performance.flow[i] >= pump_performance.flow[j])
            {errors = 1;ImportErrorMessage(errors,error_code,error_message);return false;}

    //sign check
    if(pump_performance.flow[0] < 0)
            {errors = 2;ImportErrorMessage(errors,error_code,error_message);return false;}

    for(int i=0;i<5;i++)
    {
        if(pump_performance.head[i]<=0 || pump_performance.power[i]<=0)
        {errors = 2;ImportErrorMessage(errors,error_code,error_message);return false;}
    }

    //upperlimit check
    for(int i=0;i<5;i++)
        if(pump_performance.flow[i]>9999 || pump_performance.head[i]>9999 || pump_performance.power[i]>9999)
            {errors = 3;ImportErrorMessage(errors,error_code,error_message);return false;}

    //Hz check
    if(pump_performance.Hz_min >= pump_performance.Hz_max || pump_performance.Hz_min < 0 || pump_performance.Hz_max <0)
        {errors = 4;ImportErrorMessage(errors,error_code,error_message);return false;}

    //vfd ascending time and descending time
    if(pump_performance.vfd_ascending_time < 5 || pump_performance.vfd_descending_time < 5)
        {errors = 5;ImportErrorMessage(errors,error_code,error_message);return false;}

    //vfd ascending time and descending time
    if(pump_performance.vfd_ascending_time > 120 || pump_performance.vfd_descending_time > 120)
        {errors = 6;ImportErrorMessage(errors,error_code,error_message);return false;}


    return true;
}
void Pump::ImportErrorMessage(int errors,int* error_code,QString* error_message)
{
    if(error_code)
        *error_code = errors;

    if(error_message)
    {
        switch(errors)
        {
        case 1:  *error_message = QObject::tr("Pump flows must be in ascending order!");break;
        case 2:  *error_message = QObject::tr("Pump flow, head and power should not below 0!");break;
        case 3:  *error_message = QObject::tr("Pump flow or head or power should not be greater than 9999!");break;
        case 4:  *error_message = QObject::tr("Pump Hz should be a positive number,"
                                              "and minimum Hz should be smaller than maximum Hz");break;
        case 5:  *error_message = QObject::tr("VFD ascending time or descending time should not below 5 seconds!");break;
        case 6:  *error_message = QObject::tr("VFD ascending time or descending time should not exceed 120 seconds!");break;
        default:  *error_message = QObject::tr("Imported Failed");
        }
    }
}

bool Pump::CheckChange()
{
    if(m_ro_data.running != m_ro_data_old.running)
        return true;

    if(m_ro_data.fault != m_ro_data_old.fault)
        return true;

    if(qAbs(m_ro_data.Hz - m_ro_data_old.Hz)>0.1)
        return true;

    if(qAbs(m_ro_data.power - m_ro_data_old.power)>0.1)
        return true;

    if(qAbs(m_ro_data.flow - m_ro_data_old.flow)>0.5)
        return true;

    if(qAbs(m_ro_data.head - m_ro_data_old.head)>0.1)
        return true;

    if(m_rw_data.run_request != m_rw_data_old.run_request)
        return true;

    if(m_rw_data.Moa != m_rw_data_old.Moa)
        return true;

    if(m_rw_data.trip != m_rw_data_old.trip)
        return true;

    if(qAbs(m_rw_data.man_Hz - m_rw_data_old.man_Hz)>0.1)
        return true;

    if(qAbs(m_rw_data.auto_Hz - m_rw_data_old.auto_Hz)>0.1)
        return true;

    return false;
}

