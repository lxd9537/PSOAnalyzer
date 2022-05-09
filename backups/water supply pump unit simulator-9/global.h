#ifndef GLOBAL_H
#define GLOBAL_H

#define PUMP_PERFORM_POINTS 10
#define MOTOR_PERFORM_POINTS 5

typedef struct{
    double flow[PUMP_PERFORM_POINTS];
    double head[PUMP_PERFORM_POINTS];
    double power[PUMP_PERFORM_POINTS];
    double effi[PUMP_PERFORM_POINTS];
    double npsh[PUMP_PERFORM_POINTS];
}PumpPerformPoints;


typedef struct{
    double power[MOTOR_PERFORM_POINTS];
    double effi[MOTOR_PERFORM_POINTS];
    double powerFactor[MOTOR_PERFORM_POINTS];
    double current[MOTOR_PERFORM_POINTS];
    double speed[MOTOR_PERFORM_POINTS];
}MotorPerformPoints;



#endif // GLOBAL_H
