#ifndef PUMPUNIT_H
#define PUMPUNIT_H

#include <QString>
#include "pump.h"
#include "global.h"
#include "QObject"
#include <polyfit.h>
#include <controlvalve.h>
#include <qxtcsvmodel.h>
#include "QMessageBox"
#include "QVector"

class PumpUnit : public QObject
{
Q_OBJECT
public:
    typedef enum{FC102,ATV610,ACQ580,Hydrovar}VfdModel;
    typedef struct{
        int      duty_pumps;
        int      standby_pumps;
        double   duty_flow_total;
        double   pressure_at_inlet;
        double   pressure_at_outlet;
        double   pressure_static;
        double   pressure_at_use_point;
        VfdModel vfd_model;
    }RawSysProfile;
    typedef struct{
        int      duty_pumps;
        int      standby_pumps;
        double   duty_flow_total;
        double   pressure_at_inlet;
        double   pressure_at_outlet;
        double   pressure_static;
        double   pressure_at_use_point;
        VfdModel vfd_model;
        double   Kfactor_head_from_flow[2];
        int      total_installed;
    }SysProfile;
    typedef struct{
        double pressure_at_inlet;
        double pressure_at_outlet;
        double pressure_at_use_point;
        double flow_total;
    }RoData;
    typedef struct{
        bool   level_switch_low;
        bool   pressure_switch_high;
    }RwData;

public:
    PumpUnit();
    ~PumpUnit();
    bool                    ImportPumpUnitConfiguration(QString filename, QString* ErrorMessage);
    bool                    WriteSysProfile(RawSysProfile system_profile, int* error_code=nullptr, QString* error_message=nullptr);
    bool                    ReadImportedFlag();
    SysProfile              ReadSysProfile();
    RoData                  ReadRoData();
    RwData                  ReadRwData();
    void                    WriteRWData(RwData &rw_data);
    Pump::PerformData       ReadOnePumpPerformData(int pump_number);
    bool                    WriteOnePumpPerformData(int pump_number, Pump::RawPerformData performance_data,
                                                int* error_code=nullptr, QString* error_message=nullptr);
    Pump::RoData            ReadOnePumpRoData(int pump_number);
    Pump::RwData            ReadOnePumpRwData(int pump_number);
    void                    WriteOnePumpRwData(int pump_number, Pump::RwData &rw_data);
    Pump*                   GetOnePump(int pump_number);
    bool                    WriteOneValveCharacter(int valve_number,ControlValve::ValveCharacter &valve_char,
                                               int* error_code=nullptr, QString* error_message=nullptr);
    void                    WriteOneValveRwData(int valve_number, ControlValve::RwData &rw_data);
    ControlValve::RoData    ReadOneValveRoData(int valve_number);
    ControlValve::RwData    ReadOneValveRwData(int valve_number);
    bool                    ReadOneValveImportedFlag(int valve_number);
    bool                    ReadOnePumpImportedFlag(int pump_number);
    double                  CalcuSystemFlow(double *balanced_pump_head);
    void                    Reset();
    void                    Run_cycle(int period_ms);
    void                    StatusSnapShot(QVector<QStringList> *csvData);

private:
    bool                ImportDataValidation(RawSysProfile system_profile, int* error_code=nullptr, QString* error_message=nullptr);
    void                ImportErrorMessage(int errors,int* error_code,QString* error_message);
    double              GetValveDpFromFlow(double flow);
    double              GetPumpHeadFromFlow(double flow);

    bool                m_prorfile_imported  = false;
    SysProfile          m_system_profile     = {1,1,0,0,0,0,0,FC102,{0,0},0};
    RoData              m_ro_data            = {0,0,0,0};
    RwData              m_rw_data            = {false,false};
    Pump                m_pumps[6];
    ControlValve        m_valves[3];

signals:
    void                AnyPumpStatusChanged(int pump_number,Pump::RwData rw_data);

public Q_SLOT:
    void                on_AnyPumpStatus_changed(int pump_number);
};

#endif // PUMPUNIT_H
