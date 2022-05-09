#ifndef CPUMP1_H
#define CPUMP1_H

#include <QObject>
#include <QString>
#include <qmath.h>

#define PUMP_FP_ORDER           3   //fit polynominal order
#define PUMP_PERFORM_POINTS     10  //performance data points quantity
#define MOTOR_FP_ORDER          3   //fit polynominal order
#define MOTOR_PERFORM_POINTS    5   //performance data points quantity

class CPump : public QObject
{
    Q_OBJECT

public:
    //水泵性能结构
    typedef struct{
        double headPolyFactor[PUMP_FP_ORDER + 1];       //流量-扬程多项式系数
        double powerPolyFactor[PUMP_FP_ORDER + 1];      //流量-功率多项式系数
        double npshPolyFactor[PUMP_FP_ORDER + 1];       //流量-NPSH多项式系数
        double effiPolyFactor[PUMP_FP_ORDER + 1];       //流量-效率多功式系数
        double flowMin;                                 //最小流量
        double flowMax;                                 //最大流量
        double powerMin;                                //最小功率
        double powerMax;                                //最大功率
        double headMin;                                 //最小扬程
        double headMax;                                 //最大扬程
        double speedMin;                                //最小转速
        double speedMax;                                //是大（额定）转速
        double hzMin;                                   //最小频率
        double hzMax;                                   //最大（额定）频率
        double ascendTime;                              //加速时间
        double descendTime;                             //减速时间
        double flowAtZeroHead;                          //零扬程时的流量（曲线延长后）
        double headAtZeroFlow;                          //零流量时的扬程
    }PumpPerform;                                       //水泵性能

    //电机性能结构
    typedef struct{
        double effiPolyFactor[MOTOR_FP_ORDER + 1];          //功率-效率多项式系数
        double powerFactorPolyFactor[MOTOR_FP_ORDER + 1];   //功率-功率因数多项式系数
        double currentPolyFactor[MOTOR_FP_ORDER + 1];       //功率-电流多项式系数
        double speedPolyFactor[MOTOR_FP_ORDER + 1];         //功率-转速多项式系数
    }MotorPerform;

    //水泵实时状态结构
    typedef struct{
        bool    running;        //运行
        bool    fault;          //故障
        bool    eoc;            //超曲线
        double  speed;          //rpm
        double  hz;             //频率
        double  flow;           //m3/h
        double  head;           //m
        double  power;          //kW
        double  current;        //A
        double  powerFactor;    //0-1
        double  efficiency;     //0-1
    }PumpStatus;

public:
            CPump();
    //初始化（内部清零）
    void    Init();
    //设置水泵名字
    void    setName(const QString &name) {m_name = name;}
    //设置水泵序列号
    void    setSerialNum(const QString &serialNum) {m_serialNum = serialNum;}
    //设置水泵及电机性能
    void    setPerform(const PumpPerform &pumpPerform, const MotorPerform &motorPerform);
    //启动水泵
    void    start() {if(isAvailable()) m_pumpStatus.running = true;}
    //停止水泵
    void    stop() {m_pumpStatus.running = false;}
    //设置水泵故障
    void    setFault() {m_pumpStatus.fault = true; m_pumpStatus.running = false;}
    //清除水泵故障
    void    clearFault() {m_pumpStatus.fault = false;}
    //设置水泵频率
    void    setTargetHz(double targetHz) {m_targetHz = targetHz;}
    //设置水泵压差
    void    setCrossPressure(double crossPressure) {m_crossPressure = crossPressure;}
    //获取水泵名字
    QString         name(){return m_name;}
    //水泵及电机是否已经设置了性能
    bool            isPerformInitialized() {return m_performInitializedFlag;}
    //水泵是否故障
    bool            isFaulty() {return m_pumpStatus.fault; }
    //水泵是否可用
    bool            isAvailable() {return isFaulty()&&isPerformInitialized();}
    //获取水泵性能
    PumpPerform     pumpPerform() {return m_pumpPerform;}
    //获取电机性能
    MotorPerform    motorPerform() {return m_motorPerform;}
    //从流量得到扬程
    double          headFromFlow(double hz, double flow, bool *eoc = nullptr);
    //从扬程得到流量
    double          flowFromHead(double hz, double head, bool *eoc = nullptr);
    //从流量得到功率
    double          powerFromFlow(double hz, double flow, bool *eoc = nullptr);
    //从流量和扬程得到频率
    double          hzFromFlowAndHead(double flow, double head, bool *touchCeiling, bool *touchFloor);
    //0扬程时的流量
    double          flowAtZeroHead(double hz);
    //0流量时的扬程
    double          headAtZeroFlow(double hz);
    //获取水泵当前状态
    PumpStatus      pumpStatus() {return m_pumpStatus;}
    //水泵运行
    void            run(int periodAsMs);
    //检查是否有状态改变
    bool            checkChange();
private:
    //水泵名字
    QString         m_name;
    //水泵序列号
    QString         m_serialNum;
    //水泵性能
    PumpPerform     m_pumpPerform;
    //电机性能
    MotorPerform    m_motorPerform;
    //水泵状态
    PumpStatus      m_pumpStatus;
    //水泵性能初始化状态
    bool            m_performInitializedFlag;
    //水泵设定频率
    double          m_targetHz;
    //水泵压差
    double          m_crossPressure;

signals:
    //水泵状态变化信号
    void            statusChanged(const PumpStatus &pumpStatus, QString pumpName);
};

#endif // CPUMP1_H
