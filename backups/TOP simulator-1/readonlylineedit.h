#ifndef READONLYLINEEDIT_H
#define READONLYLINEEDIT_H
#include "QLineEdit"

class ReadOnlyLineEdit : public QLineEdit
{
public:
    explicit ReadOnlyLineEdit(QWidget *parent = nullptr);
protected:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void painEvent(QPaintEvent *e);
};

#endif // READONLYLINEEDIT_H
