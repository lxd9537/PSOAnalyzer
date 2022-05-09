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
    if(pumpSysPara.dutyPumps < 1 || pumpSysPara.dutyPumps > 9)
    {*errorMessage = "Duty pumps is out of range (1~9)!";return false;}

    //total flow check
    double max_pump_flow = pump->ReadPerformance().flow_max;
    if(pumpSysPara.designTotalFlow <= 0 || pumpSysPara.designTotalFlow >= max_pump_flow * pumpSysPara.dutyPumps)
    {*errorMessage = "Design total flow is out of capacity!";return false;}

    //head at total flow check
    double headAtTotalFlow = pump->GetHeadFromFlow(50,pumpSysPara.designTotalFlow/pumpSysPara.dutyPumps);
    if(pumpSysPara.headAtTotalFlow <= 0 || pumpSysPara.headAtTotalFlow >= headAtTotalFlow)
    {*errorMessage = "Head at total flow is out of capacity!";return false;}

    //head at zero flow check
    if(pumpSysPara.headAtZeroFlow <=0 || pumpSysPara.headAtZeroFlow > pumpSysPara.headAtTotalFlow)
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

double PumpSys::CalcuSystemFlow(Pump *pump,int pumps, double Hz, double *balanced_pump_head)
{
    if(pumps < 1) return 0;

    //calculate system flow
    //using method-disection
    //for a given flow,if pump head is equal to valve dp then the flow is balanced flow, so we update the current flow
    //if pump head is greater, then inrease the given flow, otherwise, if pump head is lesser, then decrease the given flow
    //flow_low is initialized as 0, flow_middle is initialized as current flow and flow_high is initialized as maximum capbility
    double max_possible_flow = 0;
    for(int i=0; i<m_pumpSysPara.dutyPumps; i++)
        max_possible_flow += pump->ReadPerformance().flow_at_zero_head;

    double flow_low      = 0;
    double flow_middle   = max_possible_flow / 2;
    double flow_high     = max_possible_flow;
    double balanced_flow = 0;

    *balanced_pump_head = 0;
    int cycles = 0;

    do
    {
        double pump_head = pump->GetHeadFromFlow(Hz,flow_middle/pumps);
        double sys_dp = GetControlCurve(flow_middle);

        if(qAbs(pump_head - sys_dp) < (pump_head * 0.001))
        {
            balanced_flow = flow_middle;
            *balanced_pump_head = pump_head;
            break;
        }
        else
            if (pump_head > sys_dp)
                flow_low = flow_middle;
            else
                flow_high = flow_middle;

        flow_middle = (flow_high + flow_low) / 2;

    }while(cycles++ < 30);

    return balanced_flow;
}
