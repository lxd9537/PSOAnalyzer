#ifndef CLICKLABLE_H
#define CLICKLABLE_H

#include <QLabel>
#include <pumpunit.h>

class ClickLabel : public QLabel
{
    Q_OBJECT
public:
    ClickLabel(QWidget *parent=nullptr);
    void SetPumpNumber(int  number);

signals:
    void clicked(int pump_number);

protected:
    void mousePressEvent(QMouseEvent* event);

private:
    int m_pump_number;
};

#endif // CLICKLABLE_H
