#include "global.h"

CurveChoose g_pump_curve_choose = FlowHead;
SensorlessCurveChoose g_sensorless_curve_choose = PowerFlow;

Pump g_pump[10];
int  g_model_qty = 1;
int  g_current_model = 1;
int  g_pump_total_qty = 0;


long smaller_long(long l1, long l2)
{
    return l1<l2 ? l1 : l2;
}

long greater_long(long l1, long l2)
{
    return l1>l2 ? l1 : l2;
}

double smaller_double(double d1, double d2)
{
    return d1<d2 ? d1 : d2;
}

double greater_double(double d1, double d2)
{
    return d1>d2 ? d1 : d2;
}

void CalcuBestQtyWithSameModel(double flow, double head, Pump* pump, EnergySavingResult *result, bool* upper_limit, bool* lower_limit)
{
    double power_minimum = LONG_MAX;
    *upper_limit = false;
    *lower_limit = false;
    result->pump_qty = 0;

    for (int i=1; i<=pump->GetPumpQty(); i++)
    {
        double power;
        bool upper_limit_calcu, lower_limit_calcu;
        double Hz_calcu = pump->CalcuHz(flow / i, head, &upper_limit_calcu, &lower_limit_calcu);
        bool EOC;
        if (lower_limit_calcu==true)
        {
            if (i==1)
            {
                *lower_limit = true;
                *upper_limit = false;
                result->power = power_minimum;
                return;
            }
            else
                break;
        }
        if (upper_limit_calcu==true)
        {
            if (i<pump->GetPumpQty())
                continue;
            else {
                *upper_limit = true;
                *lower_limit = false;
                result->power = power_minimum;
                return;
            }
        }

        power = pump->GetAssemblyPowerFromFlow(Hz_calcu, flow / i, &EOC) * i;
        if (EOC == true)
        {
            if (i<pump->GetPumpQty())
                continue;
            else {
                *upper_limit = true;
                *lower_limit = false;
                result->power = power_minimum;
                return;
            }
        }
        if (power < power_minimum)
        {
            power_minimum = power;
            result->power = power_minimum / i;
            result->Hz = Hz_calcu;
            result->pump_qty = i;
            result->flow = flow / i;

        }
    }
}
/*
 * calculation a best pump combination with a target flow
 *
 * total_flow: tartget system flow
 * head: head of the pumps
 * final_result: calculation result, 4 element array
 *
*/

bool BestEnergyCombination(double total_flow, double head, EnergySavingResult *final_result) {

    bool successful = false;
    /* calculaion flow capobility of each model*/
    double flow_max[4];
    double flow_min[4];
    for (int i=0; i<4; i++){
        if (g_pump[i+1].IsPumpMotorImported() && g_pump[i+1].GetPumpQty() > 0 && i<g_model_qty) {
            flow_max[i] = smaller_double(g_pump[i+1].GetPumpMotorModel().flow_max * g_pump[i+1].GetPumpQty(), total_flow);
        } else {
            flow_max[i] = 0;
        }
        flow_min[i] = 0;
    }

    /* calculation best combination */
    double sub_flow[4] = {0,0,0,0};
    double flow_step = total_flow / 1000;
    EnergySavingResult sub_result[4];
    bool high_limit[4], low_limit[4];
    double power_minimum = LONG_MAX;
    double total_power;
    bool stop_cycle[4] = {0, 0, 0, 0};

    for (sub_flow[0] = flow_min[0]; sub_flow[0] <= flow_max[0] && !stop_cycle[0]; sub_flow[0] += flow_step) {
        bool flow_equal_1 = false;
        int  flow_equal_qty_1 = 0;
        double flow_equal_calcu_1;
        double avai_flow_1 = sub_flow[0];
        if(!g_pump[1].GetPumpMotorModel().vfd_installed) {
            for (int i=1; i<=g_pump[1].GetPumpQty(); i++ ) {
                bool EOC;
                flow_equal_calcu_1 = g_pump[1].GetFlowFromHead(g_pump[1].GetPumpMotorModel().Hz_max, head, &EOC);
                if (i<g_pump[1].GetPumpQty()) {
                    if (EOC == false && flow_equal_calcu_1 * i <= sub_flow[0] && flow_equal_calcu_1 * (i+1) > sub_flow[0]) {
                        flow_equal_1 = true;
                        flow_equal_qty_1 = i;
                        break;
                    }
                } else {
                    if (EOC == false && flow_equal_calcu_1 * i <= sub_flow[0]) {
                        flow_equal_1 = true;
                        flow_equal_qty_1 = i;
                    }
                }
            }
            if (flow_equal_1)
                avai_flow_1 = flow_equal_calcu_1 * flow_equal_qty_1;
            else
                avai_flow_1 = 0;
        }

        for (sub_flow[1] = flow_min[1]; sub_flow[1] <= smaller_double(flow_max[1], total_flow - avai_flow_1) && !stop_cycle[1]; sub_flow[1] += flow_step) {
            bool flow_equal_2 = false;
            int  flow_equal_qty_2;
            double flow_equal_calcu_2;
            double avai_flow_2 = sub_flow[1];
            if(!g_pump[2].GetPumpMotorModel().vfd_installed) {
                for (int i=1; i<=g_pump[2].GetPumpQty(); i++ ) {
                    bool EOC;
                    flow_equal_calcu_2 = g_pump[2].GetFlowFromHead(g_pump[2].GetPumpMotorModel().Hz_max, head, &EOC);
                    if (i<g_pump[2].GetPumpQty()) {
                        if (EOC == false && flow_equal_calcu_2 * i <= sub_flow[1] && flow_equal_calcu_2 * (i+1) > sub_flow[1]) {
                            flow_equal_2 = true;
                            flow_equal_qty_2 = i;
                            break;
                        }
                    } else {
                        if (EOC == false && flow_equal_calcu_2 * i <= sub_flow[1]) {
                            flow_equal_2 = true;
                            flow_equal_qty_2 = i;
                        }
                    }
                    if (flow_equal_2)
                        avai_flow_2 = flow_equal_calcu_2 * g_pump[2].GetPumpQty();
                    else
                        avai_flow_2 = 0;
                }
            }
            for (sub_flow[2] = flow_min[2]; sub_flow[2] <= smaller_double(flow_max[2], total_flow - avai_flow_1 - avai_flow_2) && !stop_cycle[2]; sub_flow[2] += flow_step) {
                bool flow_equal_3 = false;
                int  flow_equal_qty_3;
                double flow_equal_calcu_3;
                double avai_flow_3 = sub_flow[2];
                if(!g_pump[3].GetPumpMotorModel().vfd_installed) {
                    for (int i=1; i<=g_pump[3].GetPumpQty(); i++ ) {
                        bool EOC;
                        flow_equal_calcu_3 = g_pump[3].GetFlowFromHead(g_pump[3].GetPumpMotorModel().Hz_max, head, &EOC);
                        if (i<g_pump[3].GetPumpQty()) {
                            if (EOC == false && flow_equal_calcu_3 * i <= sub_flow[2] && flow_equal_calcu_3 * (i+1) > sub_flow[2]) {
                                flow_equal_3 = true;
                                flow_equal_qty_3 = i;
                                break;
                            }
                            else {
                                if (EOC == false && flow_equal_calcu_3 * i <= sub_flow[2]) {
                                    flow_equal_3 = true;
                                    flow_equal_qty_3 = i;
                                }
                            }
                            if (flow_equal_3)
                                avai_flow_3 = flow_equal_calcu_3 * g_pump[3].GetPumpQty();
                            else
                                avai_flow_3 = 0;
                        }
                    }
                }
                for (sub_flow[3] = flow_min[3]; sub_flow[3] <= smaller_double(flow_max[3], total_flow - avai_flow_1 - avai_flow_2 - avai_flow_3) && !stop_cycle[3]; sub_flow[3] += flow_step) {
                    bool flow_equal_4 = false;
                    int  flow_equal_qty_4;
                    double flow_equal_calcu_4;
                    double avai_flow_4 = sub_flow[3];
                    if(!g_pump[4].GetPumpMotorModel().vfd_installed) {
                        for (int i=1; i<=g_pump[4].GetPumpQty(); i++ ) {
                            bool EOC;
                            flow_equal_calcu_4 = g_pump[4].GetFlowFromHead(g_pump[4].GetPumpMotorModel().Hz_max, head, &EOC);
                            if (i<g_pump[4].GetPumpQty()) {
                                if (EOC == false && flow_equal_calcu_4 * i <= sub_flow[3] && flow_equal_calcu_4 * (i+1) > sub_flow[3]) {
                                    flow_equal_4 = true;
                                    flow_equal_qty_4 = i;
                                    break;
                                }
                                else {
                                    if (EOC == false && flow_equal_calcu_4 * i <= sub_flow[3]) {
                                        flow_equal_4 = true;
                                        flow_equal_qty_4 = i;
                                    }
                                }
                                if (flow_equal_4)
                                    avai_flow_4 = flow_equal_calcu_4 * g_pump[4].GetPumpQty();
                                else
                                    avai_flow_4 = 0;
                            }
                        }
                    }

                    if (abs(avai_flow_1 + avai_flow_2 + avai_flow_3 + avai_flow_4 - total_flow) > flow_step)
                        continue;

                    if(!g_pump[1].GetPumpMotorModel().vfd_installed) {
                        if (flow_equal_1) {
                            sub_result[0].power = flow_equal_qty_1 * g_pump[1].GetAssemblyPowerFromFlow(g_pump[1].GetPumpMotorModel().Hz_max, avai_flow_1 / flow_equal_qty_1);
                            sub_result[0].Hz = g_pump[1].GetPumpMotorModel().Hz_max;
                            sub_result[0].flow = avai_flow_1 / flow_equal_qty_1;
                            sub_result[0].pump_qty = flow_equal_qty_1;
                        }
                        else
                            memset(&sub_result[0],0,sizeof(sub_result[0]));
                    }
                    else {
                        if(sub_flow[0] > 1) {
                            CalcuBestQtyWithSameModel(sub_flow[0], head, &g_pump[1], &sub_result[0], &high_limit[0], &low_limit[0]);
                            if (low_limit[0])
                                goto cycle_1;
                            if (high_limit[0])
                                goto cycle_out;
                        }
                        else
                            memset(&sub_result[0],0,sizeof(sub_result[0]));
                    }
                    if(!g_pump[2].GetPumpMotorModel().vfd_installed) {
                        if (flow_equal_2) {
                            sub_result[1].power = flow_equal_qty_2 * g_pump[2].GetAssemblyPowerFromFlow(g_pump[2].GetPumpMotorModel().Hz_max, avai_flow_2 / flow_equal_qty_2);
                            sub_result[1].Hz = g_pump[2].GetPumpMotorModel().Hz_max;
                            sub_result[1].flow = avai_flow_2 / flow_equal_qty_2;
                            sub_result[1].pump_qty = flow_equal_qty_2;
                        }
                        else
                            memset(&sub_result[1],0,sizeof(sub_result[1]));
                    }
                    else {
                        if(sub_flow[1] > 1) {
                            CalcuBestQtyWithSameModel(sub_flow[1], head, &g_pump[2], &sub_result[1], &high_limit[1], &low_limit[1]);
                            if (low_limit[1])
                                goto cycle_2;
                            if (high_limit[1])
                                goto cycle_1;
                        }
                        else
                            memset(&sub_result[1],0,sizeof(sub_result[1]));
                    }

                    if(!g_pump[3].GetPumpMotorModel().vfd_installed) {
                        if (flow_equal_3) {
                            sub_result[2].power = flow_equal_qty_3 * g_pump[3].GetAssemblyPowerFromFlow(g_pump[3].GetPumpMotorModel().Hz_max, avai_flow_3 / flow_equal_qty_3);
                            sub_result[2].Hz = g_pump[3].GetPumpMotorModel().Hz_max;
                            sub_result[2].flow = avai_flow_3 / flow_equal_qty_3;
                            sub_result[2].pump_qty = flow_equal_qty_3;
                        }
                        else
                            memset(&sub_result[2],0,sizeof(sub_result[2]));
                    } else {
                        if(sub_flow[2] > 1) {
                            CalcuBestQtyWithSameModel(sub_flow[2], head, &g_pump[3], &sub_result[2], &high_limit[2], &low_limit[2]);
                            if (low_limit[2])
                                goto cycle_3;
                            if (high_limit[2])
                                goto cycle_2;
                        }
                        else
                            memset(&sub_result[2],0,sizeof(sub_result[2]));
                    }

                    if(!g_pump[4].GetPumpMotorModel().vfd_installed) {
                        if (flow_equal_4) {
                            sub_result[3].power = flow_equal_qty_4 * g_pump[4].GetAssemblyPowerFromFlow(g_pump[4].GetPumpMotorModel().Hz_max, avai_flow_4 / flow_equal_qty_4);
                            sub_result[3].Hz = g_pump[4].GetPumpMotorModel().Hz_max;
                            sub_result[3].flow = avai_flow_4 / flow_equal_qty_4;
                            sub_result[3].pump_qty = flow_equal_qty_4;
                        }
                        else
                            memset(&sub_result[3],0,sizeof(sub_result[3]));
                    } else {
                        if(sub_flow[3] > 1) {
                            CalcuBestQtyWithSameModel(sub_flow[3], head, &g_pump[4], &sub_result[3], &high_limit[3], &low_limit[3]);
                            if (low_limit[3])
                                goto cycle_4;
                            if (high_limit[3])
                                goto cycle_3;
                        }
                        else
                            memset(&sub_result[3],0,sizeof(sub_result[3]));
                    }

                    total_power = 0;
                    for (int i=0; i<4; i++) {
                        total_power += sub_result[i].power * sub_result[i].pump_qty;
                    }
                    /*
                    qDebug("tp=%f,mp=%f,p1=%f,p2=%f,p3=%f,p4=%f", total_power, power_minimum, sub_result[0].power,sub_result[1].power,sub_result[2].power,sub_result[3].power);
                    qDebug("Hz1=%f,Hz2=%f,Hz3=%f,Hz4=%f", sub_result[0].Hz,sub_result[1].Hz,sub_result[2].Hz,sub_result[3].Hz);
                    qDebug("f1=%f,f2=%f,f3=%f,f4=%f", sub_result[0].flow,sub_result[1].flow,sub_result[2].flow,sub_result[3].flow);
                    qDebug("n1=%d,n2=%d,n3=%d,n4=%d", sub_result[0].pump_qty,sub_result[1].pump_qty,sub_result[2].pump_qty,sub_result[3].pump_qty);
                    qDebug("----------------------------------------------------------");
                    */

                    if (total_power < power_minimum) {
                        power_minimum = total_power;
                        for (int n=0; n<4; n++)
                            memcpy(&final_result[n], &sub_result[n], sizeof(sub_result));
                        successful = true;
                    }


cycle_4:;
                }
cycle_3:;
            }
cycle_2:;
        }
cycle_1:;
    }
cycle_out:;

    return successful;
}


void EnergyWithSameModel(double begin_flow, double end_flow, double *flow, double *head, double *power, double *Hz) {

    /* calculate the head */
    double flow_step = (end_flow - begin_flow) / 20;

    flow[0] = begin_flow;
    for (int i=0; i<20; i++)
    {
        if (i>0)
            flow[i] = flow[i-1] + flow_step;
        head[i] = GetPipingHeadlossFromFlow(flow[i]);
    }

    /* calculation max 3 pumps */
    bool upper_limit = false;
    bool lower_limit = false;
    bool EOC = false;

    for (int n=0; n<3; n++) {
        for (int i=0; i<20; i++) {
            *(Hz+n*20+i) = g_pump[g_current_model].CalcuHz(flow[i] / (n+1), head[i], &upper_limit, &lower_limit);
            if (lower_limit || upper_limit) {
                *(power+n*20+i) = 0;
                *(Hz+n*20+i) = 0;
            }
            else {
                *(power+n*20+i) = g_pump[g_current_model].GetAssemblyPowerFromFlow(*(Hz+n*20+i), flow[i] / (n+1), &EOC) * (n+1);
                if (EOC) {
                    *(power+n*20+i) = 0;
                    *(Hz+n*20+i) = 0;
                }
            }
        }
    }
}

void EnergyWithDiffModel(double begin_flow, double end_flow, double *flow, double *head, double *power, double *Hz) {

    /* calculate the head */
    double flow_step = (end_flow - begin_flow) / 20;

    flow[0] = begin_flow;
    for (int i=0; i<20; i++)
    {
        if (i>0)
            flow[i] = flow[i-1] + flow_step;
        head[i] = GetPipingHeadlossFromFlow(flow[i]);
    }

    /* calculation two diff pumps */
    bool upper_limit = false;
    bool lower_limit = false;
    bool EOC = false;

    /* calculation of model 1 , two small pumps */
    for (int i=0; i<20; i++) {
        *(Hz+i) = g_pump[1].CalcuHz(flow[i] / 2, head[i], &upper_limit, &lower_limit);
        if (lower_limit || upper_limit) {
            *(power + i) = 0;
            *(Hz + i) = 0;
        }
        else {
            *(power + i) = g_pump[1].GetAssemblyPowerFromFlow(*(Hz+i), flow[i] / 2, &EOC) * 2;
            if (EOC) {
                *(power + i) = 0;
                *(Hz + i) = 0;
            }
        }
    }

    /* calculation of model 2 , one big pump */
    for (int i=0; i<20; i++) {
        *(Hz+20+i) = g_pump[2].CalcuHz(flow[i], head[i], &upper_limit, &lower_limit);
        if (lower_limit || upper_limit) {
            *(power + 20 + i) = 0;
            *(Hz + 20 + i) = 0;
        }
        else {
            *(power +20+ i) = g_pump[2].GetAssemblyPowerFromFlow(*(Hz+20+i), flow[i], &EOC);
            if (EOC) {
                *(power + 20 + i) = 0;
                *(Hz + 20 + i) = 0;
            }
        }
    }
}

void EnergyWithDiffFreq(double begin_flow, double end_flow, double *flow, double *head, double *power, double *Hz, double model_1_Hz) {

    /* calculate the head */
    double flow_step = (end_flow - begin_flow) / 20;

    flow[0] = begin_flow;
    for (int i=0; i<20; i++)
    {
        if (i>0)
            flow[i] = flow[i-1] + flow_step;
        head[i] = GetPipingHeadlossFromFlow(flow[i]);
    }

    /* calculation two diff pumps */
    bool upper_limit = false;
    bool lower_limit = false;
    bool EOC = false;
    double flow_calcu[20];

    /* calculation of model 1 , two small pumps */
    for (int i=0; i<20; i++) {
        *(Hz + i) = model_1_Hz;
        flow_calcu[i] = g_pump[1].GetFlowFromHead(*(Hz+i), head[i], &EOC);
        *(power + i) = g_pump[1].GetAssemblyPowerFromFlow(*(Hz+i), flow_calcu[i], &EOC);
        if (EOC) {
            *(power + i) = 0;
            *(Hz + i) = 0;
        }
    }

    /* calculation of model 2 , one big pump */
    for (int i=0; i<20; i++) {
        *(Hz+20+i) = g_pump[2].CalcuHz(flow[i] - flow_calcu[i], head[i], &upper_limit, &lower_limit);
        if (lower_limit || upper_limit) {
            *(power + 20 + i) = 0;
            *(Hz + 20 + i) = 0;
        }
        else {
            *(power +20+ i) = g_pump[2].GetAssemblyPowerFromFlow(*(Hz+20+i), flow[i] - flow_calcu[i], &EOC);
            if (EOC) {
                *(power + 20 + i) = 0;
                *(Hz + 20 + i) = 0;
            }
        }
    }
}
void EnergyWithDiffModel2(double begin_flow, double end_flow, double *flow, double *head, double *power, double *Hz) {

    /* calculate the head */
    double flow_step = (end_flow - begin_flow) / 20;

    flow[0] = begin_flow;
    for (int i=0; i<20; i++)
    {
        if (i>0)
            flow[i] = flow[i-1] + flow_step;
        head[i] = GetPipingHeadlossFromFlow(flow[i]);
    }

    /* calculation two diff pumps */
    bool upper_limit = false;
    bool lower_limit = false;
    bool EOC = false;

    /* calculation of model 1 , two small pumps */
    for (int i=0; i<20; i++) {
        *(Hz+i) = g_pump[1].CalcuHz(flow[i], head[i], &upper_limit, &lower_limit);
        if (lower_limit || upper_limit) {
            *(power + i) = 0;
            *(Hz + i) = 0;
        }
        else {
            *(power + i) = g_pump[1].GetAssemblyPowerFromFlow(*(Hz+i), flow[i], &EOC);
            if (EOC) {
                *(power + i) = 0;
                *(Hz + i) = 0;
            }
        }
    }

    /* calculation of model 2 , one big pump */
    for (int i=0; i<20; i++) {
        *(Hz+20+i) = g_pump[2].CalcuHz(flow[i], head[i], &upper_limit, &lower_limit);
        if (lower_limit || upper_limit) {
            *(power + 20 + i) = 0;
            *(Hz + 20 + i) = 0;
        }
        else {
            *(power +20+ i) = g_pump[2].GetAssemblyPowerFromFlow(*(Hz+20+i), flow[i], &EOC);
            if (EOC) {
                *(power + 20 + i) = 0;
                *(Hz + 20 + i) = 0;
            }
        }
    }
}
