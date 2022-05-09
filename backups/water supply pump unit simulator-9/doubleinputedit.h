#ifndef DoubleInputEdit_H
#define DoubleInputEdit_H
#include "QLineEdit"
#include "doubleinputdialog.h"
#include "QObject"

class DoubleInputEdit : public QLineEdit
{
public:Q_OBJECT

public:
    explicit DoubleInputEdit(QWidget *parent = nullptr);
    void Initiation(double current_value = 0,double min_value = 0,double max_value = 9999,
                    char format = 'f', int precision = 1);
    double GetCurrentValue();
    double GetMinValue();
    double GetMaxValue();
    char GetDisplayFormat();
    int GetDisplayPrecision();
    void SetCurrentValue(double value);
protected:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
signals:
    void clicked();

private:
    double m_current_value,m_min_value,m_max_value;
    char m_format;
    int m_precision;
    void updateDisplay();

};

#endif // READONLYLINEEDIT_H
