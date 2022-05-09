#include "clicklabel.h"
#include "QMessageBox"

ClickLabel::ClickLabel(QWidget *parent)
    :QLabel(parent)
{

}

void ClickLabel::mousePressEvent(QMouseEvent* event)
{
    emit clicked(m_pump_number);
    QLabel::mousePressEvent(event);
}

void ClickLabel::SetPumpNumber(int number)
{
    m_pump_number = number;
}
