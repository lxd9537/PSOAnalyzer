#ifndef IntInputEdit_H
#define IntInputEdit_H
#include "QLineEdit"
#include "intinputdialog.h"
#include "QObject"

class IntInputEdit : public QLineEdit
{
public:Q_OBJECT

public:
    explicit IntInputEdit(QWidget *parent = nullptr);
    void Initiation(int current_value = 0,int min_value = 0,int max_value = 9999);
    int GetCurrentValue();
    int GetMinValue();
    int GetMaxValue();
    void SetCurrentValue(int value);
protected:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseIntClickEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
signals:
    void clicked();

private:
    int m_current_value,m_min_value,m_max_value;
    void updateDisplay();

};

#endif // READONLYLINEEDIT_H
