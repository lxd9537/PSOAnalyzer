#ifndef CPUMPDATAMANAGEDIALOG_H
#define CPUMPDATAMANAGEDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <qinputdialog.h>
#include <Database/MySql/cmysqlthread.h>
#include "cpumpdatainputdialog.h"



namespace Ui {
class CPumpDataManageDialog;
}

class CPumpDataManageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CPumpDataManageDialog(CMySqlThread *mysql_thread, QWidget *parent = nullptr);
    ~CPumpDataManageDialog();

private slots:
    void on_modelListRefresh_clicked();

    void on_dataView_clicked();

    void on_dataModify_clicked();

    void on_dataNew_clicked();

    void on_dataDelete_clicked();

private:
    Ui::CPumpDataManageDialog *ui;


    CMySqlThread *m_mysql_thread;
};

#endif // CPUMPDATAMANAGEDIALOG_H
