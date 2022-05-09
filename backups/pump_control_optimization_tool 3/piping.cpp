#include "piping.h"

PipingTestData          g_piping_test_data;
PipingModel             g_piping_model;
bool                    g_piping_imported = 0;

PipingModel             GetPipingModel() {return g_piping_model;};
PipingTestData          GetPipingTestData() {return g_piping_test_data;};
bool ImportPipingModel(PipingTestData piping_data)
{
    g_piping_test_data = piping_data;
    g_piping_model.curve_type = piping_data.curve_type;
    g_piping_model.point_numbers = piping_data.point_numbers;
    g_piping_model.flow_max = piping_data.flow[piping_data.point_numbers - 1];
    switch(g_piping_model.curve_type) {
    case PipeCurveType::STEP_LINE:
    case PipeCurveType::PIECEWISE_LINEAR:
        for (int i=0; i<piping_data.point_numbers; i++) {
            g_piping_model.flow[i] = piping_data.flow[i];
            g_piping_model.head[i] = piping_data.head[i];
        }
        break;
    case PipeCurveType::QUADRATIC_CURVE:
        g_piping_model.quatratic_coefficient = (piping_data.head[1] - piping_data.head[0])
                / (piping_data.flow[1] * piping_data.flow[1] - piping_data.flow[0] * piping_data.flow[0]);
        g_piping_model.static_head = piping_data.head[0] - g_piping_model.quatratic_coefficient * (piping_data.flow[0] * piping_data.flow[0]);
        break;
    }
    g_piping_imported = true;
    return true;
}

bool IsPipingImported() {return g_piping_imported;};

double GetPipingHeadlossFromFlow(double flow)
{
    double headloss = 0;
    switch(g_piping_model.curve_type) {
    case PipeCurveType::STEP_LINE:
        if (flow <= g_piping_model.flow[0]) {
            headloss = g_piping_model.head[0];
        } else if (flow >= g_piping_model.flow[g_piping_model.point_numbers-1]) {
            headloss = g_piping_model.head[g_piping_model.point_numbers-1];
        } else {
            for (int i=1; i<g_piping_model.point_numbers; i++) {
                if (flow < g_piping_model.flow[i]) {
                    headloss = g_piping_model.head[i-1];
                    break;
                }
            }
        }
        break;
    case PipeCurveType::PIECEWISE_LINEAR:
        if (flow <= g_piping_model.flow[0]) {
            headloss = g_piping_model.head[0];
        } else if (flow >= g_piping_model.flow[g_piping_model.point_numbers-1]) {
                headloss = g_piping_model.head[g_piping_model.point_numbers-1];
        } else {
            for (int i=1; i<g_piping_model.point_numbers; i++) {
                if (flow <= g_piping_model.flow[i]) {
                    Q_ASSERT(g_piping_model.flow[i] > g_piping_model.flow[i-1]);
                    headloss = g_piping_model.head[i-1]
                            + (flow - g_piping_model.flow[i-1]) /
                            (g_piping_model.flow[i] - g_piping_model.flow[i-1])
                            * (g_piping_model.head[i] - g_piping_model.head[i-1]);
                    break;
                }
            }
        }
        break;
    case PipeCurveType::QUADRATIC_CURVE:
        headloss = g_piping_model.static_head + g_piping_model.quatratic_coefficient * flow * flow;
        break;
    }
    return headloss;
}
