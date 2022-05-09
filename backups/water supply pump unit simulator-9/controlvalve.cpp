#include "controlvalve.h"

ControlValve::ControlValve(QObject *parent) : QObject(parent)
{

}

bool ControlValve::WriteValveCharacter(ValveCharacter valve_char,int* error_code, QString* error_message)
{

    if(valve_char.open_max < 0 || valve_char.open_min < 0 ||
            valve_char.open_max >100 || valve_char.open_min > 100 || valve_char.open_min >= valve_char.open_max)
        {*error_code = 1;*error_message="max open and min open are values between 0 and 100 and max open is "
                                        "greater than min open";return false;}

    if(valve_char.open_time < 5 || valve_char.close_time < 5 ||
            valve_char.open_time >120 || valve_char.close_time > 120)
        {*error_code = 2;*error_message="open time and close time are values between 5 and 120";return false;}

    if(valve_char.Kv < 0 || valve_char.Kv > 9999)
        {*error_code = 3;*error_message="Kv is a value between 0 and 9999";return false;}

    m_valve_character.open_max      = valve_char.open_max;
    m_valve_character.open_min      = valve_char.open_min;
    m_valve_character.open_time     = valve_char.open_time;
    m_valve_character.close_time    = valve_char.close_time;
    m_valve_character.Kv            = valve_char.Kv;
    m_valve_character.valve_char    = valve_char.valve_char;

    m_imported_flag = true;
    return true;

}

bool ControlValve::ReadImportedFlag()
{
    return m_imported_flag;
}
ControlValve::RoData ControlValve::ReadRoData()
{
    return m_ro_data;
}
ControlValve::RwData ControlValve::ReadRwData()
{
    return m_rw_data;
}
void ControlValve::WriteRWData(RwData &rw_data)
{

    if(rw_data.open_percent_set < 0)
        rw_data.open_percent_set = 0;
    if(rw_data.open_percent_set > 100)
        rw_data.open_percent_set = 100;
    if(rw_data.pressure_drop < 0)
        rw_data.pressure_drop = 0;
    m_rw_data = rw_data;
}
double ControlValve::GetDpFromFlow(double flow)
{
    if(m_ro_data.Kv < 0.1)
        return 9999;

    return qPow(flow / m_ro_data.Kv,2);
}
void ControlValve::SetDifferentialPressure(double dp)
{
    m_rw_data.pressure_drop = dp;
}

void ControlValve::Reset()
{
    m_ro_data = {0,0,0};
    m_rw_data = {0,0};
}

void ControlValve::Run_cycle(int period_ms)
{

    //valve open
    if (m_rw_data.open_percent_set < m_valve_character.open_min)
        m_rw_data.open_percent_set = m_valve_character.open_min;

    if (m_rw_data.open_percent_set > m_valve_character.open_max)
        m_rw_data.open_percent_set = m_valve_character.open_max;

    if(m_ro_data.open_percent < m_rw_data.open_percent_set)
    {
        m_ro_data.open_percent += 100.0/static_cast<double>(m_valve_character.open_time)  * period_ms /1000;
        if (m_ro_data.open_percent > m_rw_data.open_percent_set) m_ro_data.open_percent = m_rw_data.open_percent_set;
    }
    else if (m_ro_data.open_percent > m_rw_data.open_percent_set)
    {
        m_ro_data.open_percent -= 100/static_cast<double>(m_valve_character.close_time)  * period_ms /1000;
        if (m_ro_data.open_percent < m_rw_data.open_percent_set) m_ro_data.open_percent = m_rw_data.open_percent_set;
    }

    //Kv
    if(m_valve_character.valve_char==Linear)
        m_ro_data.Kv =  m_ro_data.open_percent /100.0 * m_valve_character.Kv;
    else if (m_valve_character.valve_char==Equal)
         {
            if(m_ro_data.open_percent < 3.3)
                m_ro_data.Kv = 0;
            else m_ro_data.Kv = qPow(30,m_ro_data.open_percent /100.0-1) * m_valve_character.Kv;
         }
         else m_ro_data.Kv = qPow(m_ro_data.open_percent /100.0, 0.5) * m_valve_character.Kv;

    //flow
    m_ro_data.flow = m_ro_data.Kv * qPow(m_rw_data.pressure_drop, 0.5);

}
