#include "ctopcontrol.h"

CTopControl::CTopControl()
{

}
void CTopControl::inputUpdate(const QVector<bool> &bool_list,
                              const QVector<float> &float_list,
                              const QVector<int> &int_list) {

    /* inputs */
    DI_high_float = bool_list[0];
    DI_power_float = bool_list[1];
    DI_intrusion = bool_list[2];
    DI_flow_pulse = bool_list[4];
    for (int i=0; i<6; i++) {
        DI_p_auto[i] = bool_list[5 + i*3];
        DI_p_run[i] = bool_list[6 + i*3];
        DI_p_fault[i] = bool_list[7 + i*3];
    }

    AI_level = float_list[0];
    for (int i=0; i<6; i++) {
        SET_start_level[i] = float_list[1 + i];
        SET_stop_level[i] = float_list[7 + i];
    }
    SET_high_level = float_list[13];
    SET_low_level = float_list[14];

    for (int i=0; i<6; i++) {
        CMD_p_operate[i] = int_list[i];
    }
}

void CTopControl::outputUpdate(QVector<bool> bool_list,
                               QVector<long> &long_list) {
    /* output for bool */
    bool_list[0] = ALM_intrusion;
    bool_list[1] =  ALM_high_level;
    bool_list[2] =  ALM_low_level;
    bool_list[3] =  ALM_high_level_switch;
    for (int i=0; i<6; i++) {
       bool_list[4 + i] =  ALM_p_fault[i];
    }
    for (int i=0; i<2; i++) {
       bool_list[10 + i] =  ALM_g_fault[i];
    }
    bool_list[12] =  INFOR_station_alarm;
    bool_list[13] =  INFOR_station_normal;
    bool_list[14] =  INFOR_station_failure;
    for (int i=0; i<6; i++) {
        bool_list[15 + i*5] =  INFOR_p_auto[i];
        bool_list[16 + i*5] =  INFOR_p_hand[i];
        bool_list[17 + i*5] =  INFOR_p_remote[i];
        bool_list[18 + i*5] =  INFOR_p_running[i];
        bool_list[19 + i*5] =  INFOR_p_stopped_fault[i];
    }
    for (int i=0; i<2; i++) {
        bool_list[45 + i*3] =  INFOR_g_auto[i];
        bool_list[46 + i*3] =  INFOR_g_hand[i];
        bool_list[47 + i*3] =  INFOR_g_running[i];
    }

    /* output for long */
    long_list[0] = STAT_flow_accu;
    for (int i=0; i<6; i++) {
        long_list[1 + i*3] = STAT_p_energy[i];
        long_list[2 + i*3] = STAT_p_starts[i];
        long_list[3 + i*3] = STAT_p_runtime[i];
    }
    long_list[19] = STAT_g_runtime[0];
    long_list[20] = STAT_g_runtime[1];

}



