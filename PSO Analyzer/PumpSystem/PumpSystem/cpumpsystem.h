#ifndef CPUMPSYSTEM_H
#define CPUMPSYSTEM_H

#include "QHash"
#include <QMessageBox>
#include "PumpSystem/SystemProfile/csystemprofileinputdialog.h"
#include "PumpSystem/Pump/cpump.h"

class CPumpSystem : public QObject
{
    Q_OBJECT
public:
    typedef struct {
        QList<CPump*> group;
        CPump::WorkingPoint workpoint;
        int running_qty;
    } GroupWorkpoint;

    typedef struct {
        QList<GroupWorkpoint> group_workpoint_list;
        float power;
        float head;
        float flow;
    } PumpCombination;

    CPumpSystem();
    ~CPumpSystem();

signals:
    void calculationFinished();

public slots:
    void doCalculation(float flow, float head);

public:
    bool init(const CSystemProfileInputDialog::SystemProfile &system_profile,
              const QList<CPump::PerformTestData> &pump_data_list, QString *error_message);
    const PumpCombination &getCombinationReslut() const {return m_combination_result; };
    QVector<QPointF> getPressureSet(float begin_flow, float end_flow, float step) const;
    float convertPressureSetToHead(float total_flow, float p2, float h0) const;
    static bool isPumpSame(const CPump &pump_1, const CPump &pump_2);
    CCommonProfileInput::CommonProfile getCommonProfile() const {return m_common_profile;};
    QList<QList<CPump*>> getPumpGroup() const {return m_group_list;};
    bool isPumpsGrouped() const {return m_grouped;};
    bool BestQtyWithSameModel(QList<CPump*> group, float flow, float head,
                                           CPump::CalculateResult &result, int &qty,
                                           CPump::WorkingPoint &work_point);
    bool BestCombinationWithDiffModel(QList<QList<CPump*>> group_list,
                                         const float flow, const float head,
                                         PumpCombination &pump_combination);
    bool isInitiated() const { return m_initiated && m_grouped; };
private:
    bool m_initiated = false;
    bool m_grouped = false;
    QList<CPump*> m_pump_list;
    CCommonProfileInput::CommonProfile m_common_profile;
    QList<QList<CPump*>> m_group_list;
    PumpCombination m_combination_result;

private:
    static bool floatSame(float f1, float f2, float diff) {
        if(abs(f1-f2) > diff)
            return false;
        else
            return true;};
    static bool pointCompare(const QPointF &p1, const QPointF &p2) { return p1.x() < p2.x();};
    static float linearConvert(float x1, float x2, float y1, float y2, float x) {
        return (x - x1) / (x2 - x1) * (y2 - y1) + y1;
    }
    void groupPumps();
};

#endif // CPUMPSYSTEM_H
