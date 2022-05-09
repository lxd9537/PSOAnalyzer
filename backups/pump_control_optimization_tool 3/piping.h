#ifndef PIPING_H
#define PIPING_H
#include "pump.h"
enum PipeCurveType {
    STEP_LINE = 0,
    PIECEWISE_LINEAR = 1,
    QUADRATIC_CURVE = 2
};

struct  PipingTestData{
    double flow[POINTS_MAX];
    double head[POINTS_MAX];
    int point_numbers;
    PipeCurveType curve_type;
};

struct PipingModel{

    PipeCurveType curve_type;
    double flow_max;
    /* quadratic curve */
    double static_head;
    double quatratic_coefficient;
    /* linear curve */
    double flow[POINTS_MAX],head[POINTS_MAX];
    int point_numbers;
};
bool                    ImportPipingModel(PipingTestData piping_data);
bool                    IsPipingImported();
PipingModel             GetPipingModel();
PipingTestData          GetPipingTestData();
double                  GetPipingHeadlossFromFlow(double flow);

#endif // PIPING_H
