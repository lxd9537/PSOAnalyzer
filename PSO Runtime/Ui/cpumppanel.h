#ifndef CPUMPPANEL_H
#define CPUMPPANEL_H

#include <QFrame>

namespace Ui {
class CPumpPanel;
}

class CPumpPanel : public QFrame
{
    Q_OBJECT
public:
    typedef struct {
        bool running;
        bool fault;
        bool opened;
        bool closed;
        uint freq;
        uint speed;
        uint voltage;
        uint current;
        uint power;
        int pressure_in;
        int pressure_out;
        uint flow;
        uint effi;
    } PumpPanelValue;

public:
    explicit CPumpPanel(QWidget *parent = nullptr);
    ~CPumpPanel();
    void valueUpdate(const PumpPanelValue&);
    void setValidity(bool validity);
    void setPumpNum(int num);
private:
    Ui::CPumpPanel *ui;
    bool m_value_avai_flag = false;
    bool m_init_flag = false;
    PumpPanelValue m_values;
};

#endif // CPUMPPANEL_H
