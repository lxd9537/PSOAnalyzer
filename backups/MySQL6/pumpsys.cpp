#include "pumpsys.h"

PumpSys::PumpSys()
{

}

PumpSys::PumpSysPara PumpSys::ReadSysPara()
{
    return m_pumpSysPara;
}
void PumpSys::WriteSysPara(PumpSysPara pumpSysPara)
{
    m_pumpSysPara = pumpSysPara;
}

bool PumpSys::SysParaValidation(PumpSysPara pumpSysPara, Pump* pump, QString* errorMessage)
{

    //pump quantity check
    if(pumpSysPara.dutyPumps < 1 || pumpSysPara.dutyPumps > 6)
    {*errorMessage = "Duty pumps is out of range (1~6)!";return false;}

    //total flow check
    double max_pump_flow = pump->ReadPerformance().flow_max;
    if(pumpSysPara.designTotalFlow <= 0 || pumpSysPara.designTotalFlow >= max_pump_flow * pumpSysPara.dutyPumps)
    {*errorMessage = "Design total flow is out of capacity!";return false;}

    //head at total flow check
    double headAtTotalFlow = pump->GetHeadFromFlow(50,pumpSysPara.designTotalFlow/pumpSysPara.dutyPumps);
    if(pumpSysPara.headAtTotalFlow <= 0 || pumpSysPara.headAtTotalFlow >= headAtTotalFlow)
    {*errorMessage = "Head at total flow is out of capacity!";return false;}

    //head at zero flow check
    if(pumpSysPara.headAtZeroFlow <=0 || pumpSysPara.headAtZeroFlow >= pumpSysPara.headAtTotalFlow)
    {*errorMessage = "Head at zero flow is out of range (0< Head at zero flow < Head at total flow)!";return false;}

    //control curve shaping check
    if(pumpSysPara.controlCurveShape < -100 || pumpSysPara.controlCurveShape > 100)
    {*errorMessage = "Control curve shaping is out of range (-100~100)!";return false;}

    return true;
}

double PumpSys::GetControlCurve(double totalFlow)
{
    double head=0;

    if(m_pumpSysPara.controlCurveShape >= 0)
    {
        head = (m_pumpSysPara.headAtTotalFlow - m_pumpSysPara.headAtZeroFlow)
           * (m_pumpSysPara.controlCurveShape / 100 * pow(totalFlow / m_pumpSysPara.designTotalFlow , 2)
           + (100-m_pumpSysPara.controlCurveShape) / 100 * pow(totalFlow / m_pumpSysPara.designTotalFlow , 1))
           +  m_pumpSysPara.headAtZeroFlow;
    }
    else {

        head = m_pumpSysPara.headAtTotalFlow -
                ((m_pumpSysPara.headAtTotalFlow - m_pumpSysPara.headAtZeroFlow)
                * (abs(m_pumpSysPara.controlCurveShape) / 100 * pow((m_pumpSysPara.designTotalFlow - totalFlow) / m_pumpSysPara.designTotalFlow , 2)
                + (100-abs(m_pumpSysPara.controlCurveShape)) / 100 * pow((m_pumpSysPara.designTotalFlow - totalFlow) / m_pumpSysPara.designTotalFlow , 1)));
    }

    return head;
}
