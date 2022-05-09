#ifndef PUMPUNIT_H
#define PUMPUNIT_H

#include <QString>
#include "pump.h"
#include "global.h"
#include "QObject"
#include <polyfit.h>
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
        double   design_flow_total;
        double   design_head;
        double   static_head;
        double   well_diameter;
        double   well_height;
        double   height_of_high_float;
        double   height_of_low_float;
        VfdModel vfd_model;
    }RawSysProfile;
    typedef struct{
        int      duty_pumps;
        int      standby_pumps;
        double   design_flow_total;
        double   design_head;
        double   static_head;
        double   well_diameter;
        double   well_height;
        double   height_of_high_float;
        double   height_of_low_float;
        VfdModel vfd_model;
        double   Kfactor_head_from_flow[2];
        int      total_installed;
    }SysProfile;
    typedef struct{
        double  in_flow;
        double  level;
        double  outlet_pressure;
        double  flow_total;
        bool    high_float;
        bool    low_float;
    }RoData;
    typedef struct{
        double flow_percent[24];
    }InFlowProfile;

public:
    PumpUnit();
    ~PumpUnit();
    bool                    ImportPumpUnitConfiguration(QString filename, QString* ErrorMessage);
    bool                    WriteSysProfile(RawSysProfile system_profile, int* error_code=nullptr, QString* error_message=nullptr);
    bool                    ReadImportedFlag();
    SysProfile              ReadSysProfile();
    RoData                  ReadRoData();
    Pump::PerformData       ReadOnePumpPerformData(int pump_number);
    bool                    WriteOnePumpPerformData(int pump_number, Pump::RawPerformData performance_data,
                                                int* error_code=nullptr, QString* error_message=nullptr);
    Pump::RoData            ReadOnePumpRoData(int pump_number);
    Pump::RwData            ReadOnePumpRwData(int pump_number);
    void                    WriteOnePumpRwData(int pump_number, Pump::RwData &rw_data);
    Pump*                   GetOnePump(int pump_number);
    bool                    ReadOnePumpImportedFlag(int pump_number);
    double                  CalcuSystemFlow(double *balanced_pump_head);
    void                    Reset();
    void                    Run_cycle(int period_ms,int speed_up, int system_hour);
    void                    StatusSnapShot(QVector<QStringList> *csvData);

private:
    bool                ImportDataValidation(RawSysProfile system_profile, int* error_code=nullptr, QString* error_message=nullptr);
    void                ImportErrorMessage(int errors,int* error_code,QString* error_message);
    double              GetPumpHeadFromFlow(double flow);

    bool                m_prorfile_imported  = false;
    bool                m_lack_water         = false;
    SysProfile          m_system_profile     = {2,1,0,0,0,0,0,0,0,FC102,{0,0},0};
    InFlowProfile       m_inflow_profile     = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    RoData              m_ro_data            = {0,0,0,0,false,false};
    Pump                m_pumps[6];

signals:
    void                AnyPumpStatusChanged(int pump_number,Pump::RwData rw_data);

public Q_SLOT:
    void                on_AnyPumpStatus_changed(int pump_number);
};

#endif // PUMPUNIT_H
