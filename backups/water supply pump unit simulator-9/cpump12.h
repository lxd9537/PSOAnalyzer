#ifndef CPUMP1_H
#define CPUMP1_H

#include <QObject>
#include <QString>

#define PUMP_FP_ORDER 3 //fit polynominal order
#define PUMP_PERFORM_POINTS 10 //performance data points quantity
#define MOTOR_FP_ORDER 3 //fit polynominal order
#define MOTOR_PERFORM_POINTS 5 //performance data points quantity

class CPump : public QObject
{
    Q_OBJECT

public:
    typedef struct{
        double headPolyFactor[PUMP_FP_ORDER + 1];
        double powerPolyFactor[PUMP_FP_ORDER + 1];
        double npshPolyFactor[PUMP_FP_ORDER + 1];
        double effiPolyFactor[PUMP_FP_ORDER + 1];
        double flowMin;
        double flowMax;
        double powerMin;
        double powerMax;
        double headMin;
        double headMax;
        double speedMin;
        double speedMax;
        double hzMin;
        double hzMax;
        double ascendTime;
        double descendTime;
        double flowAtZeroHead;
        double headAtZeroFlow;
    }PumpPerform;

    typedef struct{
        double effiPolyFactor[MOTOR_FP_ORDER + 1];
        double powerFactorPolyFactor[MOTOR_FP_ORDER + 1];
        double currentPolyFactor[MOTOR_FP_ORDER + 1];
        double speedPolyFactor[MOTOR_FP_ORDER + 1];
    }MotorPerform;

    typedef struct{
        bool    running;
        double  speed;          //rpm
        double  hz;             //
        double  flow;           //m3/h
        double  head;           //m
        double  power;          //kW
        double  current;        //A
        double  powerFactor;    //0-1
        double  efficiency;     //0-1
    }PumpStatus;


public:
    CPump();

    void Init();

    void setName();
    void name();


    void pumpPerform(PumpPerform *perform) const { *perform = m_pumpPerform;}

    bool setMotorPerform(const MotorPerform &perform,int points);
    void motorPerform(MotorPerform &perform, int *points);

    bool isPerformSet() const {return m_performSetFlag;}

    void start();
    void stop();

    bool isAvailable();
    bool isEoc();

    PumpStatus pumpStatus();


    void run();

private:

    bool        setPumpPerform(const PumpPerform &perform);

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

private:
    bool        performDataValidation(PumpPerform *perform, int *error_code=nullptr, QString* error_message=nullptr);
    void        ImportErrorMessage(int errors,int* error_code,QString* error_message);
    bool        CheckChange();

    QString         m_name;                     //水泵名称
    PumpPerform     m_pumpPerform;              //水泵性能，需要初始化时设置
    MotorPerform    m_motorPerform;             //电机性能，需要初始化时设置
    PumpStatus      m_pumpStatus;               //水泵状态
    bool            m_performSetFlag;           //水泵及电机性能已初始化

signals:
    void        statusChanged(PumpStatus m_pumpStatus);    //水泵状态改变消息
};

#endif // CPUMP1_H
