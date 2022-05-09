#include "pumpunit.h"
#include <QMessageBox>
#include <QObject>
#include <QString>
#include <QtMath>
#include <polyfit.h>
#include <QDebug>

PumpUnit::PumpUnit()
{
    m_prorfile_imported = false;

    for(int i=0;i<6;i++)
        m_pumps[i].SetPumpNumber(i);

    for(int i=0;i<6;i++)
        connect(&m_pumps[i],&Pump::StatusChanged,this,&PumpUnit::on_AnyPumpStatus_changed);
}
PumpUnit::~PumpUnit()
{

}

bool PumpUnit::ImportPumpUnitConfiguration(QString filename, QString* ErrorMessage)
{
    int error_code;

    if(filename=="")
    {
        *ErrorMessage = "File name is blank!";
        return false;
    }

    QxtCsvModel csvModel(this);
    csvModel.setSource(filename,false,',',nullptr);

    if(csvModel.rowCount() < 59 || csvModel.columnCount() < 7)
    {
        *ErrorMessage= "Please check CSV file integrity!";
        return false;
    }

    //import pump performance data
    Pump::RawPerformData pump_data_1,pump_data_n;

    //import pump 1
    for(int i=0;i<5;i++)
    {
        pump_data_1.flow[i] = csvModel.text(i+2,0).toDouble();
        pump_data_1.head[i] = csvModel.text(i+2,1).toDouble();
        pump_data_1.power[i] = csvModel.text(i+2,2).toDouble();
    }
    pump_data_1.vfd_ascending_time = csvModel.text(2,3).toInt();
    pump_data_1.vfd_descending_time = csvModel.text(2,4).toInt();
    pump_data_1.Hz_max = csvModel.text(2,5).toDouble();
    pump_data_1.Hz_min = csvModel.text(2,6).toDouble();

    if(!WriteOnePumpPerformData(0,pump_data_1,&error_code,ErrorMessage))
        return false;

    //import pump 2-6
    for(int j=1;j<6;j++)
    {
        if(csvModel.text(j*7+2,0)=="")
            WriteOnePumpPerformData(j,pump_data_1,&error_code,ErrorMessage);
        else
        {
           for(int i=0;i<5;i++)
           {
                pump_data_n.flow[i] = csvModel.text(j*7+i+2,0).toDouble();
                pump_data_n.head[i] = csvModel.text(j*7+i+2,1).toDouble();
                pump_data_n.power[i] = csvModel.text(j*7+i+2,2).toDouble();
           }
           pump_data_n.vfd_ascending_time = csvModel.text(j*7+2,3).toInt();
           pump_data_n.vfd_descending_time = csvModel.text(j*7+2,4).toInt();
           pump_data_n.Hz_max = csvModel.text(j*7+2,5).toDouble();
           pump_data_n.Hz_min = csvModel.text(j*7+2,6).toDouble();

           if(!WriteOnePumpPerformData(j,pump_data_n,&error_code,ErrorMessage))
               return false;
        }
    }

    //import system profile
    PumpUnit::RawSysProfile system_data;

    system_data.duty_pumps = csvModel.text(44,1).toInt();
    system_data.standby_pumps = csvModel.text(45,1).toInt();
    system_data.duty_flow_total = csvModel.text(46,1).toDouble();
    system_data.pressure_at_inlet = csvModel.text(47,1).toDouble();
    system_data.pressure_at_outlet = csvModel.text(48,1).toDouble();
    system_data.pressure_static = csvModel.text(49,1).toDouble();
    system_data.pressure_at_use_point = csvModel.text(50,1).toDouble();

    if(csvModel.text(51,1)=="FC102")
        system_data.vfd_model = PumpUnit::FC102;
    else if (csvModel.text(51,1)=="ACQ580")
        system_data.vfd_model = PumpUnit::ACQ580;
    else if (csvModel.text(51,1)=="ATV610")
        system_data.vfd_model = PumpUnit::ATV610;
    else if (csvModel.text(51,1)=="Hydrovar")
        system_data.vfd_model = PumpUnit::Hydrovar;
    else{
        *ErrorMessage = "Cannot identify the VFD model!";
        return false;
    }
    qDebug()<<"vfd model:"<<QString::number(system_data.vfd_model);

    if(!WriteSysProfile(system_data,&error_code,ErrorMessage))
        return false;

    //import valve characteristic
    ControlValve::ValveCharacter valve_char;

    if(csvModel.text(53,1)=="linear")
        valve_char.valve_char = ControlValve::Linear;
    else if (csvModel.text(53,1)=="equal")
        valve_char.valve_char = ControlValve::Equal;
    else if (csvModel.text(53,1)=="quick")
        valve_char.valve_char = ControlValve::Quick;
    else{
        *ErrorMessage = "Cannot identify the valve characteristic!";
        return false;
    }

    qDebug()<<"valve character:"<<QString::number(valve_char.valve_char);

    valve_char.Kv = csvModel.text(54,1).toDouble();
    valve_char.open_max = csvModel.text(55,1).toDouble();
    valve_char.open_min = csvModel.text(56,1).toDouble();
    valve_char.open_time = csvModel.text(57,1).toInt();
    valve_char.close_time = csvModel.text(58,1).toInt();

    QString error_message;
    for(int i = 0; i<3 ; i++)
    {
        if(!WriteOneValveCharacter(i,valve_char,&error_code,ErrorMessage))
            return false;
    }

    return true;
}

bool PumpUnit::WriteSysProfile(RawSysProfile system_profile, int* error_code, QString* error_message)
{
    if(!ImportDataValidation(system_profile,error_code,error_message))
        return false;

    m_system_profile.duty_pumps                     = system_profile.duty_pumps;
    m_system_profile.standby_pumps                  = system_profile.standby_pumps;
    m_system_profile.duty_flow_total                = system_profile.duty_flow_total;
    m_system_profile.pressure_at_inlet              = system_profile.pressure_at_inlet;
    m_system_profile.pressure_at_outlet             = system_profile.pressure_at_outlet;
    m_system_profile.pressure_static                = system_profile.pressure_static;
    m_system_profile.pressure_at_use_point          = system_profile.pressure_at_use_point;
    m_system_profile.vfd_model                      = system_profile.vfd_model;
    m_system_profile.Kfactor_head_from_flow[0]      = m_system_profile.pressure_static;
    if(m_system_profile.duty_flow_total>0)
        m_system_profile.Kfactor_head_from_flow[1]  = (m_system_profile.pressure_at_outlet
                                                        - m_system_profile.pressure_at_inlet - m_system_profile.pressure_at_use_point
                                                        - m_system_profile.pressure_static)/pow(m_system_profile.duty_flow_total,2);
    m_system_profile.total_installed                = m_system_profile.duty_pumps + m_system_profile.standby_pumps;
    m_prorfile_imported = true;

    return true;
}

bool PumpUnit::ReadImportedFlag()
{
    return m_prorfile_imported;
}
PumpUnit::SysProfile PumpUnit::ReadSysProfile()
{
    return m_system_profile;
}
PumpUnit::RoData PumpUnit::ReadRoData()
{
    return m_ro_data;
}
PumpUnit::RwData PumpUnit::ReadRwData()
{
    return m_rw_data;
}
void PumpUnit::WriteRWData(RwData &rw_data)
{
    m_rw_data = rw_data;
}
Pump::PerformData PumpUnit::ReadOnePumpPerformData(int pump_number)
{
    if(pump_number < 6 && pump_number >= 0)
        return m_pumps[pump_number].ReadPerformance();
    else return m_pumps[0].ReadPerformance();
}
bool PumpUnit::WriteOnePumpPerformData(int pump_number, Pump::RawPerformData performance_data, int* error_code,QString* error_message)
{
    bool result;
    if(pump_number < 6 && pump_number >= 0)
        result = m_pumps[pump_number].WritePerformance(performance_data, error_code, error_message);
    else result = false;
    return result;
}

Pump::RoData PumpUnit::ReadOnePumpRoData(int pump_number)
{
    if(pump_number < 6 && pump_number >= 0)
        return m_pumps[pump_number].ReadRoData();
    else return m_pumps[0].ReadRoData();
}
Pump::RwData PumpUnit::ReadOnePumpRwData(int pump_number)
{
    if(pump_number < 6 && pump_number >= 0)
        return m_pumps[pump_number].ReadRwData();
    else return m_pumps[0].ReadRwData();
}
void PumpUnit::WriteOnePumpRwData(int pump_number, Pump::RwData &rw_data)
{
    if(pump_number < 6 && pump_number >= 0)
        m_pumps[pump_number].WriteRWData(rw_data);
}
Pump* PumpUnit::GetOnePump(int pump_number)
{
    if(pump_number>=0 && pump_number<6)
        return &m_pumps[pump_number];
    else return &m_pumps[0];
}
bool PumpUnit::WriteOneValveCharacter(int valve_number,ControlValve::ValveCharacter &valve_char,
                                      int* error_code, QString* error_message)
{
    bool result;
    if(valve_number < 3 && valve_number >= 0)
        result = m_valves[valve_number].WriteValveCharacter(valve_char,error_code,error_message);
    else result = false;
    return result;
}
void PumpUnit::WriteOneValveRwData(int valve_number, ControlValve::RwData &rw_data)
{
    if(valve_number < 3 && valve_number >= 0)
        m_valves[valve_number].WriteRWData(rw_data);
}
ControlValve::RoData PumpUnit::ReadOneValveRoData(int valve_number)
{
    if(valve_number < 3 && valve_number >= 0)
        return m_valves[valve_number].ReadRoData();
    else return m_valves[0].ReadRoData();
}
ControlValve::RwData PumpUnit::ReadOneValveRwData(int valve_number)
{
    if(valve_number < 3 && valve_number >= 0)
        return m_valves[valve_number].ReadRwData();
    else return m_valves[0].ReadRwData();
}
bool PumpUnit::ReadOneValveImportedFlag(int valve_number)
{
    if(valve_number < 3 && valve_number >= 0)
        return m_valves[valve_number].ReadImportedFlag();
    else return false;
}
bool PumpUnit::ReadOnePumpImportedFlag(int pump_number)
{
    if(pump_number < 6 && pump_number >= 0)
        return m_pumps[pump_number].ReadImportedFlag();
    else return false;
}
double PumpUnit::CalcuSystemFlow(double *balanced_pump_head, double *valve_dps, double *valve_flows)
{
    //calculate system flow
    //using method-disection
    //for a given flow,if pump head is equal to valve dp then the flow is balanced flow, so we update the current flow
    //if pump head is greater, then inrease the given flow, otherwise, if pump head is lesser, then decrease the given flow
    //flow_low is initialized as 0, flow_middle is initialized as current flow and flow_high is initialized as maximum capbility
    double max_possible_flow = 0;
    for(int i=0; i<m_system_profile.total_installed; i++)
        max_possible_flow += m_pumps[i].GetFlowAtZeroHead();

    double flow_low = 0;
    double flow_high = max_possible_flow * 1.2;
    double flow_middle = (flow_high + flow_low)/2;
    double pump_head;
    double valve_dp;

    *balanced_pump_head = 0;
    int cycles=0;
    double calcu_error=0;
    do
    {
        pump_head = GetPumpHeadFromFlow(flow_middle);
        valve_dp = GetValveDpFromFlow(flow_middle,valve_dps, valve_flows);

        if(valve_dp > 9990)
        {
            flow_high = flow_middle;
            flow_middle = (flow_high + flow_low) / 2;
            continue;
        }
        else
            if(valve_dp <= 0.0001)
             {
                flow_low = flow_middle;
                flow_middle = (flow_high + flow_low) / 2;
                continue;
             }

        if(flow_middle >= max_possible_flow)
        {
            *balanced_pump_head = 0;
            valve_dps[0] = 0;
            valve_dps[1] = 0;
            valve_dps[2] = 0;
            valve_flows[0] = max_possible_flow;
            valve_flows[1] = 0;
            valve_flows[2] = 0;
            return max_possible_flow;
        }
        else
            if(flow_middle <= 0.001)
            {
                *balanced_pump_head = pump_head;
                valve_dps[0] = pump_head;
                valve_dps[1] = valve_dps[0] - m_system_profile.pressure_static / 2;
                if(valve_dps[1] < 0)
                    valve_dps[1] = 0;
                valve_dps[2] = valve_dps[0] - m_system_profile.pressure_static;
                if(valve_dps[2] < 0)
                    valve_dps[2] = 0;
                valve_flows[0] = 0;
                valve_flows[1] = 0;
                valve_flows[2] = 0;
                return 0;
            }
            else
            {
                calcu_error=qAbs(pump_head - valve_dp*10);
                if(calcu_error < (pump_head * 0.002))
                    break;
                else
                    if (pump_head > valve_dp*10)
                        flow_low = flow_middle;
                    else
                        flow_high = flow_middle;
                flow_middle = (flow_high + flow_low) / 2;
            }

    }while(cycles++<20);
    qDebug()<<"cycles="<<QString::number(cycles)<<"error:"<<QString::number(calcu_error);
    *balanced_pump_head = pump_head;
    return flow_middle;
}

void PumpUnit::Reset()
{
    for(int i=0; i<m_system_profile.total_installed;i++)
        m_pumps[i].Reset();

    for(int i=0; i<3; i++)
        m_valves[i].Reset();

    m_ro_data = {0,0,0,0};
    m_rw_data = {false,false};
}

void PumpUnit::Run_cycle(int period_ms)
{
    //run pump unit
    //calculate system flow
    double pump_head;
    double valve_dps[3];
    double valve_flows[3];

    m_ro_data.flow_total = CalcuSystemFlow(&pump_head,valve_dps,valve_flows);
    m_ro_data.pressure_at_inlet = m_system_profile.pressure_at_inlet;
    m_ro_data.pressure_at_outlet = pump_head/10
                                    + m_ro_data.pressure_at_inlet;
    //assign head to pumps
    for(int i=0; i<m_system_profile.total_installed; i++)
        m_pumps[i].SetDifferentialPressure(pump_head);

    //assign dp to valves
    m_valves[0].SetDifferentialPressure(valve_dps[0]);
    m_valves[1].SetDifferentialPressure(valve_dps[1]);
    m_valves[2].SetDifferentialPressure(valve_dps[2]);

    //run pumps
    for(int i=0; i<6; i++)
        m_pumps[i].Run_cycle(period_ms);

    //run valves
    for(int i=0; i<3; i++)
        m_valves[i].Run_cycle(period_ms);

}

void PumpUnit::StatusSnapShot(QVector<QStringList> * csvData)
{
    QStringList row;

    if(csvData == nullptr)
        return;

    row.append("pump unit total flow");
    row.append(QString::number(m_ro_data.flow_total));
    csvData->append(row);row.clear();
    row.append( "pressure at inlet of pump unit");
    row.append( QString::number(m_ro_data.pressure_at_inlet));
    csvData->append(row);row.clear();
    row.append( "pressure at outlet of pump unit");
    row.append( QString::number(m_ro_data.pressure_at_outlet));
    csvData->append(row);row.clear();
    row.append( "pressure at use point");
    row.append( QString::number(m_ro_data.pressure_at_use_point));
    csvData->append(row);row.clear();
    row.append( "inlet level switch low");
    row.append( QString::number(m_rw_data.level_switch_low));
    csvData->append(row);row.clear();
    row.append( "outlet pressure switch high");
    row.append( QString::number(m_rw_data.pressure_switch_high));
    csvData->append(row);row.clear();
    row.append( "");
    row.append( QObject::tr(""));
    csvData->append(row);row.clear();
    row.append( "valve 1 Kv");
    row.append( QString::number(m_valves[0].ReadRoData().Kv));
    csvData->append(row);row.clear();
    row.append( "valve 1 open");
    row.append( QString::number(m_valves[0].ReadRoData().open_percent));
    csvData->append(row);row.clear();
    row.append( "valve 1 flow");
    row.append( QString::number(m_valves[0].ReadRoData().flow));
    csvData->append(row);row.clear();
    row.append( "valve 2 Kv");
    row.append( QString::number(m_valves[1].ReadRoData().Kv));
    csvData->append(row);row.clear();
    row.append( "valve 2 open");
    row.append( QString::number(m_valves[1].ReadRoData().open_percent));
    csvData->append(row);row.clear();
    row.append( "valve 2 flow");
    row.append( QString::number(m_valves[1].ReadRoData().flow));
    csvData->append(row);row.clear();
    row.append( "valve 3 Kv");
    row.append( QString::number(m_valves[2].ReadRoData().Kv));
    csvData->append(row);row.clear();
    row.append( "valve 3 open");
    row.append( QString::number(m_valves[2].ReadRoData().open_percent));
    csvData->append(row);row.clear();
    row.append( "valve 3 flow");
    row.append( QString::number(m_valves[2].ReadRoData().flow));
    csvData->append(row);row.clear();
    row.append( "valve 1 DP");
    row.append( QString::number(m_valves[0].ReadRwData().pressure_drop));
    csvData->append(row);row.clear();
    row.append( "valve 1 open set");
    row.append( QString::number(m_valves[0].ReadRwData().open_percent_set));
    csvData->append(row);row.clear();
    row.append( "valve 2 DP");
    row.append( QString::number(m_valves[1].ReadRwData().pressure_drop));
    csvData->append(row);row.clear();
    row.append( "valve 2 open set");
    row.append( QString::number(m_valves[1].ReadRwData().open_percent_set));
    csvData->append(row);row.clear();
    row.append( "valve 3 DP");
    row.append( QString::number(m_valves[2].ReadRwData().pressure_drop));
    csvData->append(row);row.clear();
    row.append( "valve 3 open set");
    row.append( QString::number(m_valves[2].ReadRwData().open_percent_set));
    csvData->append(row);row.clear();

    for(int i=0; i<6; i++)
    {
        row.append( "pump " + QString::number(i+1)+ " Hz");
        row.append( QString::number(m_pumps[i].ReadRoData().Hz));
        csvData->append(row);row.clear();
        row.append( "pump " + QString::number(i+1)+" flow");
        row.append( QString::number(m_pumps[i].ReadRoData().flow));
        csvData->append(row);row.clear();
        row.append( "pump " + QString::number(i+1)+" head");
        row.append( QString::number(m_pumps[i].ReadRoData().head));
        csvData->append(row);row.clear();
        row.append( "pump" + QString::number(i+1)+" power");
        row.append( QString::number(m_pumps[i].ReadRoData().power));
        csvData->append(row);row.clear();
        row.append( "pump " + QString::number(i+1)+" running");
        row.append( QString::number(m_pumps[i].ReadRoData().running));
        csvData->append(row);row.clear();
        row.append( "pump " +QString::number(i+1)+" fault");
        row.append( QString::number(m_pumps[i].ReadRoData().fault));
        csvData->append(row);row.clear();
        row.append( "pump "+ QString::number(i+1)+" MOA");
        if(ReadOnePumpRwData(i).Moa == Pump::Man)
            row.append( QObject::tr("Man"));
        else if(ReadOnePumpRwData(i).Moa == Pump::Auto)
                row.append( QObject::tr("Auto"));
               else  row.append( QObject::tr("Off"));
        csvData->append(row);row.clear();
        row.append( "pump " + QString::number(i+1)+ " trip");
        row.append( QString::number(m_pumps[i].ReadRwData().trip));
        csvData->append(row);row.clear();
        row.append( "pump "+ QString::number(i+1)+ " man Hz");
        row.append( QString::number(m_pumps[i].ReadRwData().man_Hz));
        csvData->append(row);row.clear();
        row.append( "pump "+ QString::number(i+1)+ " auto Hz");
        row.append( QString::number(m_pumps[i].ReadRwData().auto_Hz));
        csvData->append(row);row.clear();
        row.append( "pump "+ QString::number(i+1)+" run request");
        row.append( QString::number(m_pumps[i].ReadRwData().run_request));
        csvData->append(row);row.clear();
    }

}

bool PumpUnit::ImportDataValidation(PumpUnit::RawSysProfile system_profile, int* error_code, QString* error_message)
{
    int errors = 0;

    //pump quantity check
    if(system_profile.duty_pumps < 1 || system_profile.duty_pumps + system_profile.standby_pumps > 6)
        {errors = 4;ImportErrorMessage(errors,error_code,error_message);return false;}

    //duty flow check
    double max_pump_flow = m_pumps[0].ReadPerformance().flow_max;
    if(system_profile.duty_flow_total <= 0 || system_profile.duty_flow_total >= max_pump_flow * system_profile.duty_pumps)
        {errors = 5;ImportErrorMessage(errors,error_code,error_message);return false;}

    //inlet pressure check
    if(system_profile.pressure_at_inlet < 0 || system_profile.pressure_at_inlet > 10)
        {errors = 6;ImportErrorMessage(errors,error_code,error_message);return false;}

    //outlet pressure check 1
    if(system_profile.pressure_at_outlet <= system_profile.pressure_at_inlet )
        {errors = 7;ImportErrorMessage(errors,error_code,error_message);return false;}

    //outlet pressure check 2
    double max_pump_head = m_pumps[0].ReadPerformance().head_max;
    if(system_profile.pressure_at_outlet - system_profile.pressure_at_inlet > (max_pump_head*0.1) )
        {errors = 8;ImportErrorMessage(errors,error_code,error_message);return false;}

    //static pressure check
    if(system_profile.pressure_static >= system_profile.pressure_at_outlet - system_profile.pressure_at_inlet
            || system_profile.pressure_static <= 0)
        {errors = 9;ImportErrorMessage(errors,error_code,error_message);return false;}

    //use point pressure check
    if(system_profile.pressure_at_use_point >= system_profile.pressure_at_outlet
            - system_profile.pressure_at_inlet - system_profile.pressure_static)
        {errors = 10;ImportErrorMessage(errors,error_code,error_message);return false;}

    return true;
}
void PumpUnit::ImportErrorMessage(int errors,int* error_code,QString* error_message)
{
    if(error_code)
        *error_code = errors;

    if(error_message)
    {
        switch(errors)
        {
        case 4:  *error_message = QObject::tr("Total pumps should be between 1 and 6!");break;
        case 5:  *error_message = QObject::tr("Duty flow of total should be a value between 0 and maximum flow!");break;
        case 6:  *error_message = QObject::tr("Inlet pressure should be a value between 0 and 10!");break;
        case 7:  *error_message = QObject::tr("Outlet pressure should not below inlet pressure!");break;
        case 8:  *error_message = QObject::tr("Outlet pressure should not exeed pump's maximum head!");break;
        case 9:  *error_message = QObject::tr("Static pressure is too high or below zero!");break;
        case 10:  *error_message = QObject::tr("Use point pressure is too high!");break;
        default:  *error_message = QObject::tr("Imported Failed");
        }
    }
}

/*******************************************************************************
K(2/3*duty_flow_total)^2+K(1/3*duty_flow_total)^2
    = K5/9 duty_flow_total 2
    = pressure_at_outlet- pressure_at_inlet- pressure_static- pressure_at_use_point
flow1/kv1=flow2/kv2+1/2static+K(Q2+Q3)^2=flow3/kv3+static+KQ3^2
flow1+flow2+flow3=flow_total
Q3=(head-Q3^2*k-static)^1/2*kv3
Q2=(head-(Q3+Q2)^2*k-static)^1/2*kv2
Q1=head^1/2*kv1
********************************************************************************/
double PumpUnit::GetValveDpFromFlow(double flow,double *valve_dps, double *valve_flows)
{
    //if(m_valves[0].ReadRoData().Kv < 0.001 && m_valves[1].ReadRoData().Kv < 0.001 && m_valves[2].ReadRoData().Kv < 0.001)
    //    return 9999; //all valve closed

    //suppose that valve 1 is near the pump at the lowest elevation, valve 3 is at the most remote end with highest elevation, and valve 2 is at the middle position between v1 and v3
    //suppose under design condition, flow rates of these 3 valves are same. And pipe's diameter and roughness are constant at any segment.
    //so the total pressure loss is:
    //total pressure loss = (total flow * 2/3)^2 * K1 + (total flow * 1/3)^2 * K2 = K * 5/9 * total flow ^ 2
    //K1=K2=K (because we suppose the pipe is same at every segment.
    //the pressure loss of stage 1 is:
    //pressure loss 1 = (flow 2 + flow 3)^2 * K
    //pressure loss 2 = (flow 3)^2 * K
    //so the pressure at each valve's inlet are:
    //dp3 = (flow 3 / Kv3) ^2
    //dp2 = dp3 + static pressure * 1/2 + pressure loss 2
    //dp1 = dp2 + static pressure * 1/2 + pressure loss 1
    //flow 3 = dp3^1/2 * Kv 3
    //flow 2 = dp2^1/2 * Kv 2
    //flow 1 = dp1^1/2 * Kv 1

    //calculation of pressure loss k factor
    double total_press_loss = m_system_profile.pressure_at_outlet - m_system_profile.pressure_at_inlet
                                 - m_system_profile.pressure_static - m_system_profile.pressure_at_use_point;
    double K = total_press_loss * (9/5) /pow(m_system_profile.duty_flow_total,2);

    //calculation a proper dp that balances the flows and dps among the valves at a given total flow
    //using method of bisection
    //the dp at which the given flow is equal to the sum flow of the three flows, is then the balanced dp
    //minimum pressure is static pressure as we supposer the pipe is always filled
    //maximum pressure is pump's head at zero flow
    double max_possible_head = 0;
    double max_possible_dp;
    for(int i=0; i<m_system_profile.total_installed; i++)
    {
        double head_at_zero_flow;
        head_at_zero_flow = m_pumps[i].GetHeadAtZeroFlow();
        if(head_at_zero_flow > max_possible_head)
            max_possible_head = head_at_zero_flow;
    }
    max_possible_dp = max_possible_head/10;

    //find a proper dp
    double low_dp = 0;
    double high_dp = max_possible_dp * 1.01;
    double middle_dp = (high_dp + low_dp) / 2;
    double dp_1,dp_2,dp_3,flow_1,flow_2,flow_3,ploss_1,ploss_2;
    int cycles=0;
    do {
        //flow of valve 1
        //dp1 = total dp;
        //flow 1 = dp1^1/2 * Kv1
        dp_1 = middle_dp;
        flow_1 = pow(dp_1,0.5) * m_valves[0].ReadRoData().Kv;

        //flow of valve 2
        //pressure loss 1 = (flow 2 + flow 3)^2 * K
        //dp2 = dp1 - static pressure/2 - pressure loss 1
        //flow 2 = dp2^1/2 * Kv2
        ploss_1 = pow((flow-flow_1),2) * K;
        dp_2 = dp_1 - m_system_profile.pressure_static / 2 - ploss_1;
        if(dp_2 < 0)
            dp_2 = 0;
        flow_2 = pow(dp_2,0.5) * m_valves[1].ReadRoData().Kv;

        //flow of pump 3
        //pressure loss 2 = (flow 3)^2 * K
        //dp3 = dp2 - static pressure /2 - pressure loss 2
        //flow 3 = dp3^1/2 * Kv3
        ploss_2 = pow((flow - flow_1 - flow_2),2) * K;
        dp_3 = dp_2 - m_system_profile.pressure_static / 2 -ploss_2;
        if(dp_3 < 0)
            dp_3 = 0;
        flow_3 = pow(dp_3,0.5) * m_valves[2].ReadRoData().Kv;

        //check total flow
        if(middle_dp >= max_possible_dp)
        {
            valve_dps[0]    = max_possible_dp;
            valve_flows[0]  = 0;
            valve_dps[1]    = valve_dps[0] - m_system_profile.pressure_static / 2;
            if(valve_dps[1]<0)
                valve_dps[1] = 0;
            valve_flows[1]  = 0;
            valve_dps[2]    = valve_dps[0] - m_system_profile.pressure_static;
            if(valve_dps[2]<0)
                valve_dps[2] = 0;
            valve_flows[2]  = 0;
            return 9999;
        }
        else
            if(middle_dp < 0.001)
            {
                valve_dps[0]    = 0;
                valve_flows[0]  = flow_1;
                valve_dps[1]    = 0;
                valve_flows[1]  = 0;
                valve_dps[2]    = 0;
                valve_flows[2]  = 0;
                return 0;
            }
            else
                if(qAbs(flow_1 + flow_2 + flow_3 - flow) < (0.001 * flow))
                    break;
                else
                    if(flow_1 + flow_2 + flow_3 < flow)
                        low_dp = middle_dp;
                    else
                        high_dp = middle_dp;

         middle_dp = (high_dp + low_dp) / 2;
    } while (cycles++<30);

    valve_dps[0]    = dp_1;
    valve_flows[0]  = flow_1;

    valve_dps[1]    = dp_2;
    valve_flows[1]  = flow_2;

    valve_dps[2]    = dp_3;
    valve_flows[2]  = flow_3;

    return dp_1;
}

double  GetValveFlowFromDp(double dp)
{

    dp=dp+1;
    return 1;
}
/*******************************************************************************
 * H=f1(Q1)=f2(Q2)=f3(Q3)                                                      *
 * Q=Q1+Q2+Q3                                                                  *
 * Q=Q1+f2inv(f1(Q1))+f3inv(f1(Q1))       inv: inverse function                *
 * H=f1(Q1)                                                                    *
********************************************************************************/
double PumpUnit::GetPumpHeadFromFlow(double flow)
{

    //if all pumps are stopped, return 0
    bool all_pump_stop = true;
    for(int i=0; i<m_system_profile.total_installed; i++)
    {
        if(m_pumps[i].ReadRoData().running && m_pumps[i].ReadRoData().Hz > 0)
            all_pump_stop = false;
    }

    if(all_pump_stop)
        return 0;

    //find a running pump with highest head
    double the_highest_shutoff_head = 0;
    for(int i=0; i<m_system_profile.total_installed; i++)
    {
        if(m_pumps[i].ReadRoData().running)
        {
            double shutoff_head = m_pumps[i].GetHeadFromFlow(m_pumps[i].ReadRoData().Hz, 0);

            if(shutoff_head > the_highest_shutoff_head)
                the_highest_shutoff_head=shutoff_head;
        }
    }

    //if flow is zero or close to zero, the head is equal to the head of the pump which has highest shut off head
    if(flow < 0.01)
        return the_highest_shutoff_head;

    //calculate the head with given flow
    double low_head = 0;
    double high_head = the_highest_shutoff_head;
    double middle_head = the_highest_shutoff_head/2;
    int cycles = 0;

    do {
        double calcu_total_flow=0;
        for(int i=0; i<m_system_profile.total_installed; i++)
            calcu_total_flow += m_pumps[i].GetFlowFromHead(m_pumps[i].ReadRoData().Hz,middle_head);

        if (qAbs(calcu_total_flow - flow) <= (0.01 * flow))
            break;
        else
            if (calcu_total_flow > flow)
                low_head = middle_head;
            else
                high_head = middle_head;

        middle_head = (high_head + low_head) / 2;

    } while (cycles++ < 100);

    return middle_head;
}

void PumpUnit::on_AnyPumpStatus_changed(int pump_number)
{
    emit AnyPumpStatusChanged(pump_number,ReadOnePumpRwData(pump_number));
}


