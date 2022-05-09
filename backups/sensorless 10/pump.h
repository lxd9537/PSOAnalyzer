#ifndef PUMP_H
#define PUMP_H

#include <QObject>
#include <global.h>
#include <polyfit.h>

class Pump : public QObject
{
Q_OBJECT
public:
    typedef struct{
        double flow[5];
        double head[5];
        double power[5];
        double Hz_min;
        double Hz_max;
        int    vfd_ascending_time;
        int    vfd_descending_time;
    }RawPerformData;
    typedef enum {Man,Off,Auto}MOA;
    typedef struct{
        double flow_min;
        double flow_max;
        double power_min;
        double power_max;
        double head_min;
        double head_max;
        double Hz_min;
        double Hz_max;
        double Kfactor_head_from_flow[CURVE_FIT_ORDER+1];
        double Kfactor_power_from_flow[CURVE_FIT_ORDER+1];
        int    vfd_ascending_time;
        int    vfd_descending_time;
        double flow_at_zero_head;
        double head_at_zero_flow;
    }PerformData;
    typedef struct{
        bool   running, fault;
        double Hz;
        double power;
        double flow;
        double head;
    }RoData;
    typedef struct{
        bool   run_request;
        MOA    Moa;
        bool   trip;
        double man_Hz;
        double auto_Hz;
    }RwData;

    typedef struct{
        double power_rate;
        double power;
        int    poles;
        double Kfactor_effi_from_power[CURVE_FIT_ORDER+1];
    }MotorData;

public:
    Pump();
    bool        WritePerformance(RawPerformData perform_data, int* error_code=nullptr, QString* error_message=nullptr);
    void        WritePerformance_2(PerformData perform_data);
    bool        ReadImportedFlag();
    PerformData ReadPerformance();
    RoData      ReadRoData();
    RwData      ReadRwData();
    void        WriteRWData(RwData &rw_data);
    void        SetPumpNumber(int pump_number);
    double      GetHeadFromFlow(double Hz, double flow, bool* EOC_flag=nullptr);
    double      GetFlowFromHead(double Hz, double head, bool* EOC_flag=nullptr);
    double      GetPowerFromFlow(double Hz, double flow, bool* EOC_flag=nullptr);
    double      CalcuHz(double flow, double head, bool* upper_limit_flag=nullptr, bool* lower_limit_flag=nullptr);
    double      GetFlowAtZeroHead();
    double      GetHeadAtZeroFlow();
    void        SetDifferentialPressure(double dp);
    void        Reset();
    void        Run_cycle(int period_ms);
    MotorData   ReadMotorData();
    void        WriteMotorData(MotorData motorData);
    double      GetMotorEffi(double power, double speed);



private:
    bool            ImportDataValidation(RawPerformData pump_performance, int* error_code=nullptr, QString* error_message=nullptr);
    void            ImportErrorMessage(int errors,int* error_code,QString* error_message);
    bool            CheckChange();
    RawPerformData  m_raw_perform_data;
    PerformData     m_perform_data              = {0,0,0,0,0,0,0,0,{0,0,0,0},{0,0,0,0},5,5,0,0};
    int             m_pump_number               = 0;
    RoData          m_ro_data                   = {false,false,0,0,0,0};
    RoData          m_ro_data_old               = {false,false,0,0,0,0};
    RwData          m_rw_data                   = {false,Auto,false,0,0};
    RwData          m_rw_data_old               = {false,Auto,false,0,0};
    bool            m_imported_flag             = false;
    double          m_diffrential_pressure      = 0;
    MotorData       m_motorData                 ={0,0,0,{0,0,0,0}};

signals:
    void StatusChanged(int pump_number);
};

#endif // PUMP_H
