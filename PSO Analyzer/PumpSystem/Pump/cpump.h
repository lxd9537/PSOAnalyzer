#ifndef CPUMP_H
#define CPUMP_H
#include <qmath.h>
#include <Polyfit/polyfit.h>
#include <QString>
#include "PumpSystem/SystemProfile/cpumpprofileinput.h"

#define POLYFIT_ORDER           3   //fit polynominal order
#define POINTS_MAX              10  //performance data points quantity
#define POINTS_MIN              5  //

class CPump
{

public:
    typedef enum {
        FROM_FLOW_AND_HEAD,
        FROM_FLOW_AND_SPEED,
        FROM_HEAD_AND_SPEED
    } CalculateType;

    typedef enum {
        NORMAL,
        MAX_POWER_EXCEEDED,
        MAX_FLOW_EXCEEDED,
        MIN_FLOW_EXCEEDED,
        MAX_HEAD_EXCEEDED
    } CalculateResult;

    typedef struct {
        QString model;
        int points;
        double flow[POINTS_MAX];
        double head[POINTS_MAX];
        double power[POINTS_MAX];
        double npshr[POINTS_MAX];
        double effi[POINTS_MAX];
        double flow_max;
        double flow_min;
        double motor_power;
    } PerformTestData;

    typedef struct {
        double coeffs_head_to_flow[POLYFIT_ORDER+1];
        double coeffs_npshr_to_flow[POLYFIT_ORDER+1];
        double coeffs_power_to_flow[POLYFIT_ORDER+1];
        double coeffs_effi_to_flow[POLYFIT_ORDER+1];
    } PumpCurveCoeffs;

    typedef struct {
        double flow;
        double head;
        double power;
        double effi;
        double npshr;
        double speed; //%
    } WorkingPoint;

public:
    CPump();
    static bool testDataValidation(const PerformTestData &perform_data, QString* error_message=nullptr);
    bool init(const PerformTestData &test_data,
              const CPumpProfileInput::PumpProfile &profile,
              QString *error_message = nullptr) {
        if (!createModel(test_data, error_message))
            return false;
        setProfile(profile);
        return true;
    }
    bool createModel(const PerformTestData &test_data, QString *error_message = nullptr);
    void setProfile(const CPumpProfileInput::PumpProfile &profile);
    bool isModelCreated() const {return m_pump_model_created;};
    bool isProfileSet() const {return m_pump_profile_set;};
    const PerformTestData getPerformData() const { return m_test_data;};
    const CPumpProfileInput::PumpProfile getProfile() const {return m_profile;};
    static double effi(double flow, double head, double power);
    double calcuHead(double flow, double speed, CalculateResult &result);
    double calcuEffi(double flow, double speed, CalculateResult &result);
    double calcuPower(double flow, double speed, CalculateResult &result);
    double calcuNpshr(double flow, double speed, CalculateResult &result);
    WorkingPoint calculateWorkingPoint(const CalculateType &calculate_type,
                                       const double &para_1,
                                       const double &para_2,
                                       CalculateResult &result);
    float actualPumpHead(const float &flow, const float &outlet_press, const float &inlet_press);
    float calcuPumpHead(const float &flow, const float &outlet_press, const float &in_tank_level);
    void setName(QString name) { m_pump_name = name; };
    QString getName() const { return m_pump_name; };
private:
    QString m_pump_name;
    PerformTestData m_test_data;
    PumpCurveCoeffs m_pump_coeffs;
    CPumpProfileInput::PumpProfile m_profile;
    bool m_pump_model_created = false;
    bool m_pump_profile_set = false;

    double pressureLoss(double flow);
    WorkingPoint calcuWorkPointfromFlowAndSpeed(const double &flow, const double &speed, CalculateResult &result);
    WorkingPoint calcuWorkPointfromFlowAndHead(const double &flow, const double &head, CalculateResult &result);
    WorkingPoint calcuWorkPointfromHeadAndSpeed(const double &head, const double &speed, CalculateResult &result);
};

#endif // CPUMP_H
