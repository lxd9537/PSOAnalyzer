#include "readonlylineedit.h"
#include "QStyleOption"
#include "QPainter"

ReadOnlyLineEdit::ReadOnlyLineEdit(QWidget *parent):
    QLineEdit (parent)
{

}

void ReadOnlyLineEdit::mousePressEvent(QMouseEvent *e)
{}
void ReadOnlyLineEdit::mouseDoubleClickEvent(QMouseEvent *e)
{}
void ReadOnlyLineEdit::mouseMoveEvent(QMouseEvent *e)
{}
void ReadOnlyLineEdit::mouseReleaseEvent(QMouseEvent *e)
{}
void ReadOnlyLineEdit::painEvent(QPaintEvent *e)
{
     QLineEdit::paintEvent(e);
}
