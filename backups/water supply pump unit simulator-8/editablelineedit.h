#ifndef EDITABLELINEEDIT_H
#define EDITABLELINEEDIT_H
#include "QLineEdit"

class EditableLineEdit : public QLineEdit
{
public:
    EditableLineEdit(QWidget *parent=nullptr);

protected:
    virtual void focusInEvent(QFocusEvent *e);

};

#endif // EDITABLELINEEDIT_H
