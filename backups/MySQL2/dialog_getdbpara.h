#ifndef DIALOG_GETDBPARA_H
#define DIALOG_GETDBPARA_H

#include <QDialog>
#include <pump_db.h>

namespace Ui {
class Dialog_GetDbPara;
}

class Dialog_GetDbPara : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_GetDbPara(QWidget *parent = nullptr);
    ~Dialog_GetDbPara();

    void Initialization(Pump_db::DbPara database_parameter);
    Pump_db::DbPara GetDbPara();

private:
    Ui::Dialog_GetDbPara *ui;
    Pump_db::DbPara m_database_parameter;

private Q_SLOT:
    void on_applyButton_clicked();
};

#endif // DIALOG_GETDBPARA_H
