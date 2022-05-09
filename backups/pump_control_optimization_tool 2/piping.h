#ifndef PIPING_H
#define PIPING_H
#include "pump.h"

struct  PipingTestData{
    double flow[POINTS_MAX];
    double head[POINTS_MAX];
};

struct PipingModel{
    double Kfactor_headloss_from_flow[CURVE_FIT_ORDER+1];
    double static_head;
    double flow_max;
};
bool                    ImportPipingModel(PipingTestData piping_data);
bool                    IsPipingImported();
PipingModel             GetPipingModel();
PipingTestData          GetPipingTestData();
double                  GetPipingHeadlossFromFlow(double flow);

#endif // PIPING_H
