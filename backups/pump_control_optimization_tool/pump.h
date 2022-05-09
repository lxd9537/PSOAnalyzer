#ifndef PUMP_H
#define PUMP_H
#define POINTS_MAX 10
#define MOTOR_POINTS_MAX 5

#include <QObject>
#include <polyfit.h>

struct  PumpPerformTestData{
    double flow[POINTS_MAX];
    double head[POINTS_MAX];
    double power[POINTS_MAX];
    double NPSHR[POINTS_MAX];
    bool   vfd_installed;
    double Hz_max;
    double Hz_min;
    double flow_max;
    double flow_min;
};

struct  MotorPerformTestData{
    double efficiency[MOTOR_POINTS_MAX];
    double power[MOTOR_POINTS_MAX];
    double power_rated;
};

struct PumpMotorModel{
    double Kfactor_head_from_flow[CURVE_FIT_ORDER+1];
    double Kfactor_NPSHR_from_flow[CURVE_FIT_ORDER+1];
    double Kfactor_power_from_flow[CURVE_FIT_ORDER+1];
    double Kfactor_effi_from_power[MOTOR_CURVE_FIT_ORDER+1];
    double Kfactor_flow_from_power[CURVE_FIT_ORDER+1];
    double Kfactor_head_from_power[CURVE_FIT_ORDER+1];
    bool   vfd_installed;
    double Hz_max;
    double Hz_min;
    double flow_max;
    double flow_min;
    double power_rated;
};



class Pump : public QObject
{
Q_OBJECT

private:
    PumpMotorModel          m_pump_motor_model;
    PumpPerformTestData     m_pump_test_data;
    MotorPerformTestData    m_motor_test_data;
    bool                    m_pump_motor_imported = 0;
    int                     m_pump_qty;

public:
    bool                    IsPumpMotorImported() {return m_pump_motor_imported;};
    PumpMotorModel          GetPumpMotorModel() {return m_pump_motor_model;};
    PumpPerformTestData     GetPumpPerformTestData() {return m_pump_test_data;};
    MotorPerformTestData    GetMotorPerformTestData() {return m_motor_test_data;};

    int                     GetPumpQty() {return m_pump_qty;};
    void                    SetPumpQty(int n) {m_pump_qty = n;};

public:
    bool        ImportPumpMotorModel(PumpPerformTestData pump_perform_data,
                                     MotorPerformTestData motor_perform_data,
                                     QString* error_message);

    double      GetPumpHeadFromFlow(double Hz, double flow, bool* EOC_flag=nullptr);
    double      GetPumpNpshrFromFlow(double Hz, double flow, bool* EOC_flag=nullptr);
    double      GetPumpPowerFromFlow(double Hz, double flow, bool* EOC_flag=nullptr);
    double      GetPumpEffiFromFlow(double Hz, double flow, bool* EOC_flag=nullptr);
    double      GetMotorEffiFromPower(double Hz, double power, bool* EOC_flag=nullptr);
    double      GetAssemblyPowerFromFlow(double Hz, double flow, bool* EOC_flag=nullptr);
    double      GetAssemblyEffiFromFlow(double Hz, double flow, bool* EOC_flag=nullptr);
    double      GetAssemblyFlowFromPower(double Hz, double power, bool* EOC_flag=nullptr);
    double      GetAssemblyHeadFromPower(double Hz, double power, bool* EOC_flag=nullptr);

    double      GetFlowAtZeroHead(double Hz);
    double      GetHeadAtZeroFlow(double Hz);
    double      GetFlowFromHead(double Hz, double head, bool* EOC_flag=nullptr);
    double      CalcuHz(double flow, double head, bool* upper_limit_flag=nullptr, bool* lower_limit_flag=nullptr);

private:
    bool        PumpPerformDataValidation(PumpPerformTestData perform_data, QString* error_message=nullptr);
    bool        MotorPerformDataValidation(MotorPerformTestData perform_data, QString* error_message=nullptr);
};

#endif // PUMP_H
