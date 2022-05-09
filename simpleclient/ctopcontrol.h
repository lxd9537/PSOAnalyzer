#ifndef CTOPCONTROL_H
#define CTOPCONTROL_H
#include "QVariant"
#include "caqtmessage.h"

class CTopControl
{


    typedef struct {
        QVariant value;


    } IoVar;


public:
    CTopControl();
    void inputUpdate(const QVector<bool> &bool_list,
                     const QVector<float> &float_list,
                     const QVector<int> &int_list);
    void execute();
    void outputUpdate(QVector<bool> bool_list,
                      QVector<long> &long_list);

private:
    /* input */
    bool DI_high_float;
    bool DI_power_float;
    bool DI_intrusion;
    bool DI_redun_LC;
    bool DI_flow_pulse;
    bool DI_p_auto[6];
    bool DI_p_run[6];
    bool DI_p_fault[6];
    bool DI_g_auto[2];
    bool DI_g_run[2];
    bool DI_g_fault[2];
    float AI_level;
    float SET_start_level[6];
    float SET_stop_level[6];
    float SET_high_level;
    float SET_low_level;
    int CMD_p_operate[6];

    /* output */
    bool ALM_intrusion;
    bool ALM_p_fault[6];
    bool ALM_g_fault[2];
    bool ALM_high_level;
    bool ALM_low_level;
    bool ALM_high_level_switch;
    bool INFOR_station_alarm;
    bool INFOR_station_normal;
    bool INFOR_station_failure;
    bool INFOR_p_auto[6];
    bool INFOR_p_hand[6];
    bool INFOR_p_remote[6];
    bool INFOR_p_running[6];
    bool INFOR_p_stopped_fault[6];
    bool INFOR_g_auto[2];
    bool INFOR_g_hand[2];
    bool INFOR_g_running[2];
    long STAT_p_energy[6];
    long STAT_p_starts[6];
    long STAT_p_runtime[6];
    long STAT_g_runtime[2];
    long STAT_flow_accu;

    /* interval */


};

#endif // CTOPCONTROL_H
