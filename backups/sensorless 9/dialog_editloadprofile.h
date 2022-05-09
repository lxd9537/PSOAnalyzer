#ifndef DIALOG_EDITLOADPROFILE_H
#define DIALOG_EDITLOADPROFILE_H

#include <QDialog>
#include <qstandarditemmodel.h>
#include <QStringListModel>

namespace Ui {
class Dialog_EditLoadProfile;
}

class Dialog_EditLoadProfile : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_EditLoadProfile(QWidget *parent = nullptr);
    ~Dialog_EditLoadProfile();
    void setData(QStandardItemModel *loadprofileData, QString &app);

private:
    Ui::Dialog_EditLoadProfile *ui;

};

#endif // DIALOG_EDITLOADPROFILE_H
