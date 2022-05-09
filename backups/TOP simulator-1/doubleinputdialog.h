#ifndef DOUBLEINPUTDIALOG_H
#define DOUBLEINPUTDIALOG_H

#include <QDialog>

namespace Ui {
class DoubleInputDialog;
}

class DoubleInputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DoubleInputDialog(QWidget *parent = nullptr);
    ~DoubleInputDialog();
    void Initiation(double current_value = 0,double min_value = 0,double max_value = 9999,
                    char format = 'f', int precision = 1);
    bool CheckLimitation(double value);
    double GetCurrentValue();

signals:
    void ValueChanged(double current_value);

private:
    Ui::DoubleInputDialog *ui;
    double m_current_value,m_min_value,m_max_value;

protected:
    virtual void accept();
};

#endif // INPUTDIALOG_H
