#ifndef GLOBAL_H
#define GLOBAL_H
#include "pump.h"
#include "piping.h"
#include "QFile"

enum  CurveChoose{
    FlowHead,
    FlowPower,
    FlowEffi,
    FlowNpshr,
    PowerEffi
};

enum  SensorlessCurveChoose{
    PowerFlow,
    PowerHead
};

struct EnergySavingResult{
    int pump_qty;
    double Hz;
    double flow;
    double power;
};


long smaller_long(long l1, long l2);
long greater_long(long l1, long l2);
double smaller_double(double l1, double l2);
double greater_double(double l1, double l2);

void CalcuBestQtyWithSameModel(double flow, double head, Pump* pump, EnergySavingResult *result, bool *upper_limit, bool *lower_limit);
bool BestEnergyCombination(double total_flow, double head, EnergySavingResult *final_result);

void EnergyWithSameModel(double begin_flow, double end_flow, double *flow, double *head, double *power, double *Hz);
void EnergyWithDiffModel(double begin_flow, double end_flow, double *flow, double *head, double *power, double *Hz);
void EnergyWithDiffFreq(double begin_flow, double end_flow, double *flow, double *head, double *power, double *Hz, double model_1_Hz);
void EnergyWithDiffModel2(double begin_flow, double end_flow, double *flow, double *head, double *power, double *Hz);

#endif // GLOBAL_H
