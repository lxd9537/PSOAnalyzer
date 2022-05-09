#include "piping.h"

PipingTestData          g_piping_test_data;
PipingModel             g_piping_model;
bool                    g_piping_imported = 0;

PipingModel             GetPipingModel() {return g_piping_model;};
PipingTestData          GetPipingTestData() {return g_piping_test_data;};
bool ImportPipingModel(PipingTestData piping_data)
{
    g_piping_test_data = piping_data;

    polyfit(piping_data.flow, piping_data.head, POINTS_MAX, CURVE_FIT_ORDER, g_piping_model.Kfactor_headloss_from_flow);
    g_piping_model.static_head = g_piping_model.Kfactor_headloss_from_flow[CURVE_FIT_ORDER];

    double flow_max = 0;
    for (int i=0; i<POINTS_MAX; i++)
        if (piping_data.flow[i] > flow_max)
            flow_max = piping_data.flow[i];
    g_piping_model.flow_max = flow_max;

    g_piping_imported = true;
    return true;
}

bool IsPipingImported() {return g_piping_imported;};

double GetPipingHeadlossFromFlow(double flow)
{
    double headloss = 0;
    for(int i=0; i<CURVE_FIT_ORDER+1; i++)
    {
        headloss +=  qPow(flow, i) * g_piping_model.Kfactor_headloss_from_flow[i];
    }
    return headloss;
}
