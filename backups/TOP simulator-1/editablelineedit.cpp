#include "editablelineedit.h"
#include "QTimer"

EditableLineEdit::EditableLineEdit(QWidget *parent):
    QLineEdit(parent)
{

}
void EditableLineEdit::focusInEvent(QFocusEvent *e)
{

    QTimer::singleShot( 0, this,SLOT(selectAll()) );
    QLineEdit::focusInEvent(e);
}
