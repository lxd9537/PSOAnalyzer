#ifndef INTINPUTDIALOG_H
#define INTINPUTDIALOG_H

#include <QDialog>

namespace Ui {
class IntInputDialog;
}

class IntInputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit IntInputDialog(QWidget *parent = nullptr);
    ~IntInputDialog();
    void Initiation(int current_value = 0,int min_value = 0,int max_value = 9999);
    bool CheckLimitation(int value);
    int GetCurrentValue();

signals:
    void ValueChanged(int current_value);

private:
    Ui::IntInputDialog *ui;
    int m_current_value,m_min_value,m_max_value;

protected:
    virtual void accept();
};

#endif // INPUTDIALOG_H
