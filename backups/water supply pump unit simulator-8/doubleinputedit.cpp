#include "doubleinputedit.h"
#include "doubleinputdialog.h"
#include "QStyleOption"
#include "QPainter"
#include "QMessageBox"

DoubleInputEdit::DoubleInputEdit(QWidget *parent):
    QLineEdit (parent)
{

}

void DoubleInputEdit::Initiation(double current_value,double min_value,double max_value,
                char format, int precision)
{
    m_current_value=current_value;
    m_min_value=min_value;
    m_max_value=max_value;
    m_format=format;
    m_precision=precision;
    updateDisplay();
}

void DoubleInputEdit::mousePressEvent(QMouseEvent *e)
{
    emit clicked();
}
void DoubleInputEdit::mouseDoubleClickEvent(QMouseEvent *e)
{
    mousePressEvent(e);
}
void DoubleInputEdit::mouseMoveEvent(QMouseEvent *e)
{}
void DoubleInputEdit::mouseReleaseEvent(QMouseEvent *e)
{}
double DoubleInputEdit::GetCurrentValue()
{
    return m_current_value;
}
void DoubleInputEdit::SetCurrentValue(double value)
{
    m_current_value = value;
    updateDisplay();
}
void DoubleInputEdit::updateDisplay()
{
    QString str = QString::number(m_current_value,m_format,m_precision);
    setText(str);
}
double DoubleInputEdit::GetMinValue()
{
    return m_min_value;
}
double DoubleInputEdit::GetMaxValue()
{
    return m_max_value;
}
char DoubleInputEdit::GetDisplayFormat()
{
    return m_format;
}
int DoubleInputEdit::GetDisplayPrecision()
{
    return m_precision;
}
