#ifndef MYPUSHBUTTON_H
#define MYPUSHBUTTON_H

#include <qpushbutton.h>

class MyPushButton : public QPushButton
{
Q_OBJECT
public:
    MyPushButton(QWidget *parent=nullptr);
    void SetPumpNumber(int  number);

signals:
    void clicked(int pump_number);
private:
    int m_pump_number;
protected:
    void mouseReleaseEvent(QMouseEvent *e);

};

#endif // MYPUSHBUTTON_H
