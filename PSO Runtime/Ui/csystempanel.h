#ifndef CSYSTEMPANEL_H
#define CSYSTEMPANEL_H

#include <QFrame>

namespace Ui {
class CSystemPanel;
}

class CSystemPanel : public QFrame
{
    Q_OBJECT
public:
    typedef struct {
        uint level;
        uint press;
        uint press_set;
        uint flow;
    } SystemPanelValue;
public:
    explicit CSystemPanel(QWidget *parent = nullptr);
    ~CSystemPanel();
    void valueUpdate(const SystemPanelValue&);
    void setValidity(bool validity);
    void setSystemNum(int num);

private:
    Ui::CSystemPanel *ui;
    bool m_value_avai_flag = false;
    bool m_init_flag = false;
    SystemPanelValue m_values;
};

#endif // CSYSTEMPANEL_H
