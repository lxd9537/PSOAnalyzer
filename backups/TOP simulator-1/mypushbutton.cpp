#include "mypushbutton.h"

MyPushButton::MyPushButton(QWidget *parent)
    :QPushButton(parent)
{

}

void MyPushButton::SetPumpNumber(int number)
{
    m_pump_number = number;
}

void MyPushButton::mouseReleaseEvent(QMouseEvent *e)
{
    emit clicked(m_pump_number);
    QPushButton::mouseMoveEvent(e);
}
