/*#include "cpump1.h"

CPump::CPump()
{

}

//初始化（内部清零）
void CPump::Init()
{
    m_name = "pump";
    m_serialNum = "0";

    for(int i=0; i < PUMP_FP_ORDER + 1; i++)
    {
        m_pumpPerform.headPolyFactor[i] = 0;
        m_pumpPerform.powerPolyFactor[i] = 0;
        m_pumpPerform.npshPolyFactor[i] = 0;
        m_pumpPerform.effiPolyFactor[i] = 0;
    }

    m_pumpPerform.flowMin = 0;
    m_pumpPerform.flowMax = 0;
    m_pumpPerform.powerMin = 0;
    m_pumpPerform.powerMax = 0;
    m_pumpPerform.headMin = 0;
    m_pumpPerform.headMax = 0;
    m_pumpPerform.speedMin = 0;
    m_pumpPerform.speedMax = 0;
    m_pumpPerform.hzMax = 0;
    m_pumpPerform.ascendTime = 0;
    m_pumpPerform.descendTime = 0;

    m_pumpStatus  = {false,false,false,0,0,0,0,0,0,0,0};
    m_performInitializedFlag = 0;
    m_targetHz = 0;
    m_crossPressure = 0;
}

//设置水泵及电机性能
void CPump::setPerform(const PumpPerform &pumpPerform, const MotorPerform &motorPerform)
{
    m_pumpPerform  = pumpPerform;
    m_motorPerform = motorPerform;
    m_performInitializedFlag = true;
}

//从流量得到扬程
double CPump::headFromFlow(double hz, double flow, bool *eoc)
{
    if(!isPerformInitialized())
        return 0;

    if(hz > m_pumpPerform.hzMax)
        hz = m_pumpPerform.hzMax;
    else if(hz < m_pumpPerform.hzMin)
            hz = m_pumpPerform.hzMin;

    double relativeSpeed = hz / m_pumpPerform.hzMax;

    if(eoc)
    {
        if(flow * relativeSpeed > m_pumpPerform.flowMax)
            *eoc = true;
        else *eoc = false;
    }

    double headPolyFactor[PUMP_FP_ORDER + 1];

    for(int i = 0; i < PUMP_FP_ORDER + 1; i++)
    {
        headPolyFactor[i] = m_pumpPerform.headPolyFactor[i] * qPow(relativeSpeed, 2 - i);
    }

    double head = 0;

    for(int i = 0; i < PUMP_FP_ORDER + 1; i++)
    {
        head += headPolyFactor[i] * qPow(flow, i);
    }

    return head;

}
//从扬程得到流量
double CPump::flowFromHead(double hz, double head, bool *eoc)
{
    if(!isPerformInitialized())
        return 0;

    if(hz < 0.1)
        return 0;

    if(hz > m_pumpPerform.hzMax)
        hz = m_pumpPerform.hzMax;
    else if(hz < 0.1)
            return 0;
    else if(hz < m_pumpPerform.hzMin)
            hz = m_pumpPerform.hzMin;

    //扬程超过闭死点
    if (head > headFromFlow(hz, 0))
        return 0;

    //曲线上
    double low_flow = 0;
    double high_flow = flowAtZeroHead(hz) < 0.1 ? m_pumpPerform.flowMax * 5 : m_pumpPerform.flowAtZeroHead;
    double middle_flow = high_flow / 2;
    int cycles = 0;

    if(head<0.01)
        head = 0.01;

    do {
        double head_calcu = headFromFlow(hz, middle_flow);

        if (qAbs(head_calcu - head) <= (0.0002 * head))
            return middle_flow;
        else
            if (head_calcu > head)
                low_flow = middle_flow;
            else
                high_flow = middle_flow;

        middle_flow = (high_flow + low_flow) / 2;

    } while (cycles++ < 30);

    double relativeSpeed = hz / m_pumpPerform.hzMax;

    if(eoc)
    {
        if(middle_flow * relativeSpeed > m_pumpPerform.flowMax)
            *eoc = true;
        else *eoc = false;
    }

    return middle_flow;
}
//从流量得到功率
double CPump::powerFromFlow(double hz, double flow, bool *eoc)
{
    if(!isPerformInitialized())
        return 0;

    if(hz > m_pumpPerform.hzMax)
        hz = m_pumpPerform.hzMax;
    else if(hz < m_pumpPerform.hzMin)
            hz = m_pumpPerform.hzMin;

    double relativeSpeed = hz / m_pumpPerform.hzMax;

    if(eoc)
    {
        if(flow * relativeSpeed > m_pumpPerform.flowMax)
            *eoc = true;
        else *eoc = false;
    }

    double pumpPowerPolyFactor[PUMP_FP_ORDER + 1];

    for(int i = 0; i < PUMP_FP_ORDER + 1; i++)
    {
        pumpPowerPolyFactor[i] = m_pumpPerform.powerPolyFactor[i] * qPow(relativeSpeed, 3 - i);
    }

    double pumpPower = 0;

    for(int i = 0; i < PUMP_FP_ORDER + 1; i++)
        pumpPower += pumpPowerPolyFactor[i] * qPow(flow, i);

    double motorEffi = 0;
    double powerConverted;
    if(hz > 0)
        powerConverted = pumpPower / qPow(hz / m_pumpPerform.hzMax, 3);
    else {
        powerConverted = pumpPower;
    }

    for(int i=0; i < MOTOR_FP_ORDER + 1; i++)
        motorEffi += motorPerform().effiPolyFactor[i] * qPow(powerConverted, i);

    double motorPower;

    if(motorEffi > 0)
        motorPower = pumpPower / motorEffi;
    else {
        motorPower = pumpPower;
    }

    return motorPower;
}
//从流量和扬程得到频率
double CPump::hzFromFlowAndHead(double flow, double head, bool *touchCeiling, bool *touchFloor)
{
    if(!isPerformInitialized())
        return 0;

    //达上限
    if (headFromFlow(m_pumpPerform.hzMax, flow) < head)
    {
        if(touchCeiling)
            *touchCeiling = true;
        return m_pumpPerform.hzMax;
    }
    else {
        if(touchCeiling)
            *touchCeiling = false;
    }

    //达下限
    if (headFromFlow(m_pumpPerform.hzMin, flow) > head)
    {
        if(touchFloor)
            *touchFloor = true;
        return m_pumpPerform.hzMin;
    }
    else {
        if(touchFloor)
            *touchFloor = false;
    }

    //在中间
    double low_Hz = m_pumpPerform.hzMin;
    double high_Hz = m_pumpPerform.hzMax;
    double middle_Hz = (m_pumpPerform.hzMin + m_pumpPerform.hzMax) / 2;
    int cycles = 0;

    do {
        double head_calcu = headFromFlow(middle_Hz, flow);

        if (qAbs(head_calcu - head) < (0.0002 * head))
            return middle_Hz;
        else
            if (head_calcu > head)
                high_Hz = middle_Hz;
            else
                low_Hz = middle_Hz;

        middle_Hz = (high_Hz + low_Hz) / 2;

    } while (cycles++ < 30);

    return -1;
}
//0扬程时的流量
double CPump::flowAtZeroHead(double hz)
{
    if(!isPerformInitialized())
        return 0;

    if(hz > m_pumpPerform.hzMax)
        hz = m_pumpPerform.hzMax;
    else if(hz < m_pumpPerform.hzMin)
            hz = m_pumpPerform.hzMin;

    return hz / m_pumpPerform.hzMax * m_pumpPerform.flowAtZeroHead;
}
//0流量时的扬程
double CPump::headAtZeroFlow(double hz)
{
    if(!isPerformInitialized())
        return 0;

    if(hz > m_pumpPerform.hzMax)
        hz = m_pumpPerform.hzMax;
    else if(hz < m_pumpPerform.hzMin)
            hz = m_pumpPerform.hzMin;

    return qPow(hz / m_pumpPerform.hzMax,2) * m_pumpPerform.headAtZeroFlow;
}
//水泵运行
void CPump::run(int periodAsMs)
{
    m_pumpStatus.hz；
    m_pumpStatus.eoc；
    m_pumpStatus.flow；
    m_pumpStatus.head；
    m_pumpStatus.fault；
    m_pumpStatus.power；
    m_pumpStatus.speed；
    m_pumpStatus.current；
    m_pumpStatus.efficiency；
    m_pumpStatus.powerFactor；




    //pump head/flow/power
    if(m_pumpStatus.running)
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

    if(checkChange())
        emit statusChanged(m_pumpStatus, m_name);
}

bool CPump::checkChange()
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
*/
