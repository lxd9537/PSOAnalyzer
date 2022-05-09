#include "intinputdialog.h"
#include "ui_intinputdialog.h"
#include "QMessageBox"

IntInputDialog::IntInputDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IntInputDialog)
{
    ui->setupUi(this);
}

IntInputDialog::~IntInputDialog()
{
    delete ui;
}
void IntInputDialog::Initiation(int current_value,int min_value,int max_value)
{
    m_current_value=current_value;
    m_min_value=min_value;
    m_max_value=max_value;

    QString str = QString::number(current_value);
    ui->lineEdit->setText(str);
    ui->lineEdit->setFocus(Qt::ShortcutFocusReason);


}
bool IntInputDialog::CheckLimitation(int value)
{
    if(value < m_min_value||value > m_max_value)
        return true;
    else return false;
}
void IntInputDialog::accept()
{

    int value = ui->lineEdit->text().toInt();

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

int IntInputDialog::GetCurrentValue()
{
    return m_current_value;
}


