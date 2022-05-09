#include "pumpunit.h"
#include <QMessageBox>
#include <QObject>
#include <QString>
#include <QtMath>
#include <polyfit.h>
#include <QDebug>
#include <QTime>

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
    system_data.design_flow_total = csvModel.text(46,1).toDouble();
    system_data.design_head = csvModel.text(47,1).toDouble();
    system_data.static_head = csvModel.text(48,1).toDouble();
    system_data.well_diameter = csvModel.text(49,1).toDouble();
    system_data.well_height = csvModel.text(50,1).toDouble();
    system_data.height_of_high_float = csvModel.text(51,1).toDouble();
    system_data.height_of_low_float = csvModel.text(52,1).toDouble();

    if(csvModel.text(53,1)=="FC102")
        system_data.vfd_model = PumpUnit::FC102;
    else if (csvModel.text(53,1)=="ACQ580")
        system_data.vfd_model = PumpUnit::ACQ580;
    else if (csvModel.text(53,1)=="ATV610")
        system_data.vfd_model = PumpUnit::ATV610;
    else if (csvModel.text(53,1)=="Hydrovar")
        system_data.vfd_model = PumpUnit::Hydrovar;
    else{
        *ErrorMessage = "Cannot identify the VFD model!";
        return false;
    }
    if(!WriteSysProfile(system_data,&error_code,ErrorMessage))
        return false;

    //import in flow profile
    for (int i=0;i<24;i++)
        m_inflow_profile.flow_percent[i] = csvModel.text(56+i,1).toDouble();

    return true;
}

bool PumpUnit::WriteSysProfile(RawSysProfile system_profile, int* error_code, QString* error_message)
{
    if(!ImportDataValidation(system_profile,error_code,error_message))
        return false;

    m_system_profile.duty_pumps                     = system_profile.duty_pumps;
    m_system_profile.standby_pumps                  = system_profile.standby_pumps;
    m_system_profile.design_flow_total              = system_profile.design_flow_total;
    m_system_profile.design_head                    = system_profile.design_head;
    m_system_profile.static_head                    = system_profile.static_head;
    m_system_profile.well_diameter                  = system_profile.well_diameter;
    m_system_profile.well_height                    = system_profile.well_height;
    m_system_profile.height_of_high_float           = system_profile.height_of_high_float;
    m_system_profile.height_of_low_float            = system_profile.height_of_low_float;
    m_system_profile.vfd_model                      = system_profile.vfd_model;
    m_system_profile.Kfactor_head_from_flow[0]      = m_system_profile.static_head;
    if(m_system_profile.design_flow_total>0)
        m_system_profile.Kfactor_head_from_flow[1]  = (m_system_profile.design_head - m_system_profile.static_head)/pow(m_system_profile.design_flow_total,2);
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

bool PumpUnit::ReadOnePumpImportedFlag(int pump_number)
{
    if(pump_number < 6 && pump_number >= 0)
        return m_pumps[pump_number].ReadImportedFlag();
    else return false;
}
double PumpUnit::CalcuSystemFlow(double *balanced_pump_head)
{
    //calculate system flow
    //using method-disection
    //for a given flow,if pump head is equal to system dp then the flow is balanced flow, so we update the current flow
    //if pump head is greater, then inrease the given flow, otherwise, if pump head is lesser, then decrease the given flow
    //flow_low is initialized as 0, flow_middle is initialized as current flow and flow_high is initialized as maximum capbility
    double max_possible_flow = 0;
    for(int i=0; i<m_system_profile.total_installed; i++)
    {
        if(m_pumps[i].ReadRoData().running)
            max_possible_flow += m_pumps[i].GetFlowAtZeroHead();
    }

    double flow_low = 0;
    double flow_high = max_possible_flow;
    double flow_middle = (flow_high + flow_low)/2;
    double pump_head;
    double outlet_pressure;
    double system_dp;

    *balanced_pump_head = 0;
    int cycles=0;
    double calcu_error=0;
    do
    {
        pump_head =GetPumpHeadFromFlow(flow_middle);
        outlet_pressure = pump_head + m_ro_data.level;

        system_dp = m_system_profile.Kfactor_head_from_flow[0]
                        + m_system_profile.Kfactor_head_from_flow[1]*pow(flow_middle,2);

        if(qAbs(flow_middle - flow_high) <= 0.01)
        {
            *balanced_pump_head = pump_head;
            return flow_middle;
        }
        else
            if(flow_middle <= 0.01)
            {
                *balanced_pump_head = pump_head;
                return flow_middle;
            }
            else
            {
                calcu_error=qAbs(outlet_pressure - system_dp);
                if(calcu_error < (system_dp * 0.002))
                {
                    *balanced_pump_head = pump_head;
                    return flow_middle;
                }
                else
                    if (outlet_pressure > system_dp)
                        flow_low = flow_middle;
                    else
                        flow_high = flow_middle;
                flow_middle = (flow_high + flow_low) / 2;
            }

    }while(cycles++<20);

    *balanced_pump_head = pump_head;
    return flow_middle;
}

void PumpUnit::Reset()
{
    for(int i=0; i<m_system_profile.total_installed;i++)
        m_pumps[i].Reset();

    m_ro_data = {0,0,0,0,false,false};
}

void PumpUnit::Run_cycle(int period_ms, int speed_up, int system_hour)
{
    //run pump unit
    //calculate system flow
    double pump_head;

    if(m_system_profile.well_diameter>0)
        m_ro_data.level += (m_ro_data.in_flow - m_ro_data.flow_total)
                            / m_system_profile.well_diameter / 7200 * speed_up;

    if(m_ro_data.level > m_system_profile.well_height)
        m_ro_data.level = m_system_profile.well_height;
    if(m_ro_data.level < 0.001)
    {
        m_ro_data.level = 0;
        m_lack_water = true;
    }

    if(m_ro_data.level > 0.2)
        m_lack_water = false;

    m_ro_data.flow_total = CalcuSystemFlow(&pump_head);

    if(m_lack_water)
         m_ro_data.flow_total = 0;

    m_ro_data.outlet_pressure           = pump_head + m_ro_data.flow_total;
    m_ro_data.high_float                = (m_ro_data.level >= m_system_profile.height_of_high_float);
    m_ro_data.low_float                 = (m_ro_data.level <= m_system_profile.height_of_low_float);
    m_ro_data.in_flow                   = m_inflow_profile.flow_percent[system_hour] * m_system_profile.design_flow_total / 100;

    //assign head to pumps
    for(int i=0; i<m_system_profile.total_installed; i++)
        m_pumps[i].SetDifferentialPressure(pump_head);

    //run pumps
    for(int i=0; i<6; i++)
    {
        m_pumps[i].Run_cycle(period_ms,m_lack_water,speed_up);
    }

}

void PumpUnit::StatusSnapShot(QVector<QStringList> * csvData)
{
    QStringList row;

    if(csvData == nullptr)
        return;

    row.append("pump unit total flow");
    row.append(QString::number(m_ro_data.flow_total));
    csvData->append(row);row.clear();
    row.append( "well level");
    row.append( QString::number(m_ro_data.level));
    csvData->append(row);row.clear();
    row.append( "pump common head");
    row.append( QString::number(m_ro_data.outlet_pressure));
    csvData->append(row);row.clear();
    row.append( "high float");
    row.append( QString::number(m_ro_data.high_float));
    csvData->append(row);row.clear();
    row.append( "low float");
    row.append( QString::number(m_ro_data.low_float));
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

    //design flow check
    double max_pump_flow = m_pumps[0].ReadPerformance().flow_max;
    if(system_profile.design_flow_total <= 0 || system_profile.design_flow_total >= max_pump_flow * system_profile.duty_pumps)
        {errors = 5;ImportErrorMessage(errors,error_code,error_message);return false;}

    //design head check 1
    if(system_profile.design_head <= 0 )
        {errors = 7;ImportErrorMessage(errors,error_code,error_message);return false;}

    //design head check 2
    double max_pump_head = m_pumps[0].ReadPerformance().head_max;
    if(system_profile.design_head > max_pump_head )
        {errors = 8;ImportErrorMessage(errors,error_code,error_message);return false;}

    //static head check
    if(system_profile.static_head >= system_profile.design_head
            || system_profile.static_head <= 0)
        {errors = 9;ImportErrorMessage(errors,error_code,error_message);return false;}

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
        case 5:  *error_message = QObject::tr("Design total flow should be a value between 0 and maximum flow!");break;
        case 6:  *error_message = QObject::tr("");break;
        case 7:  *error_message = QObject::tr("Design head should not below 0!");break;
        case 8:  *error_message = QObject::tr("Design head should not exeed pump's maximum head!");break;
        case 9:  *error_message = QObject::tr("Static head is too high or below zero!");break;
        default:  *error_message = QObject::tr("Imported Failed");
        }
    }
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

    } while (cycles++ < 30);

    return middle_head;
}

void PumpUnit::on_AnyPumpStatus_changed(int pump_number)
{
    emit AnyPumpStatusChanged(pump_number,ReadOnePumpRwData(pump_number));
}


