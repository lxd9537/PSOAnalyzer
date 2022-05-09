#include "dialog_editloadprofile.h"
#include "ui_dialog_editloadprofile.h"

Dialog_EditLoadProfile::Dialog_EditLoadProfile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_EditLoadProfile)
{
    ui->setupUi(this);

}

Dialog_EditLoadProfile::~Dialog_EditLoadProfile()
{
    delete ui;
}

void Dialog_EditLoadProfile::setData(QStandardItemModel *loadprofileData, QString &app)
{

    ui->tableView->setModel(loadprofileData);
    ui->tableView->resizeRowsToContents();
    ui->tableView->setColumnWidth(1,80);
    ui->tableView->setColumnWidth(2,80);
    ui->tableView->hideColumn(0);

    ui->tableView->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color: rgb(240, 240, 240);}");
    this->setWindowTitle(app+" load profile");
}
