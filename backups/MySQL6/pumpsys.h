#ifndef PUMPSYS_H
#define PUMPSYS_H

#include <QObject>
#include <pump.h>
#include <qmath.h>

class PumpSys
{
public:
    typedef struct{
        int     dutyPumps;
        double  designTotalFlow;
        double  headAtZeroFlow;
        double  headAtTotalFlow;
        double  controlCurveShape;
    }PumpSysPara;

    PumpSys();
    PumpSysPara ReadSysPara();
    void        WriteSysPara(PumpSysPara pumpSysPara);
    bool        SysParaValidation(PumpSysPara pumpSysPara, Pump* pump, QString* error_message);
    double      GetControlCurve(double totalFlow);

private:
    PumpSysPara m_pumpSysPara;

};

#endif // PUMPSYS_H
