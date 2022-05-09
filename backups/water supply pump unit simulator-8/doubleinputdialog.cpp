#include "doubleinputdialog.h"
#include "ui_doubleinputdialog.h"
#include "QMessageBox"

DoubleInputDialog::DoubleInputDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DoubleInputDialog)
{
    ui->setupUi(this);
}

DoubleInputDialog::~DoubleInputDialog()
{
    delete ui;
}
void DoubleInputDialog::Initiation(double current_value,double min_value,double max_value,
                                   char format, int precision)
{
    m_current_value=current_value;
    m_min_value=min_value;
    m_max_value=max_value;

    QString str = QString::number(current_value,format,precision);
    ui->lineEdit->setText(str);
    ui->lineEdit->setFocus(Qt::ShortcutFocusReason);


}
bool DoubleInputDialog::CheckLimitation(double value)
{
    if(value < m_min_value||value > m_max_value)
        return true;
    else return false;
}
void DoubleInputDialog::accept()
{

    double value = ui->lineEdit->text().toDouble();

    if(CheckLimitation(value))
    {
       QMessageBox::warning(this,"Warning","The value exeeds the limitation!");
       return;
    }
    else
    {
        m_current_value = value;
        emit ValueChanged(m_current_value);
        QDialog::accept();
    }
 }

double DoubleInputDialog::GetCurrentValue()
{
    return m_current_value;
}


