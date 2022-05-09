#ifndef CONTROLVALVE_H
#define CONTROLVALVE_H

#include <QObject>
#include <QtMath>

class ControlValve : public QObject
{
    Q_OBJECT
public:
    typedef enum {Linear,Equal,Quick}Character; //linear:Q=k*l,Equal:Q=k1*e^(k2*l),Quick:Q=k*l^(1/2)
    typedef struct{
        Character valve_char;
        double Kv;
        double open_max;
        double open_min;
        int    open_time;
        int    close_time;
    }ValveCharacter;
    typedef struct{
        double open_percent;
        double flow;
        double Kv;
    }RoData;
    typedef struct{
        double open_percent_set;
        double pressure_drop;
    }RwData;

    explicit    ControlValve(QObject *parent = nullptr);
    bool        WriteValveCharacter(ValveCharacter, int* error_code=nullptr, QString* error_message=nullptr);
    bool        ReadImportedFlag();
    RoData      ReadRoData();
    RwData      ReadRwData();
    void        WriteRWData(RwData &rw_data);
    double      GetDpFromFlow(double flow);
    void        SetDifferentialPressure(double dp);
    void        Reset();
    void        Run_cycle(int period_ms);
private:
    ValveCharacter  m_valve_character   = {Linear,1,100,0,60,60};
    RoData          m_ro_data           = {0,0,0};
    RwData          m_rw_data           = {0,0};
    bool            m_imported_flag     = false;

signals:

public slots:
};

#endif // CONTROLVALVE_H
