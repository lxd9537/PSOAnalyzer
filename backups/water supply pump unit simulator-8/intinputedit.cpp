#include "intinputedit.h"
#include "intinputdialog.h"
#include "QStyleOption"
#include "QPainter"
#include "QMessageBox"

IntInputEdit::IntInputEdit(QWidget *parent):
    QLineEdit (parent)
{

}

void IntInputEdit::Initiation(int current_value,int min_value,int max_value)
{
    m_current_value=current_value;
    m_min_value=min_value;
    m_max_value=max_value;
    updateDisplay();
}

void IntInputEdit::mousePressEvent(QMouseEvent *e)
{
    emit clicked();
}
void IntInputEdit::mouseIntClickEvent(QMouseEvent *e)
{
    mousePressEvent(e);
}
void IntInputEdit::mouseMoveEvent(QMouseEvent *e)
{}
void IntInputEdit::mouseReleaseEvent(QMouseEvent *e)
{}
int IntInputEdit::GetCurrentValue()
{
    return m_current_value;
}
void IntInputEdit::SetCurrentValue(int value)
{
    m_current_value = value;
    updateDisplay();
}
void IntInputEdit::updateDisplay()
{
    QString str = QString::number(m_current_value);
    setText(str);
}
int IntInputEdit::GetMinValue()
{
    return m_min_value;
}
int IntInputEdit::GetMaxValue()
{
    return m_max_value;
}

