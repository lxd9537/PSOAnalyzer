#ifndef DIALOG_BARCHART_H
#define DIALOG_BARCHART_H

#include <QDialog>
#include <QStandardItemModel>
#include <qcustomplot.h>

namespace Ui {
class Dialog_ShowLoadProfile;
}

class Dialog_ShowLoadProfile : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_ShowLoadProfile(QWidget *parent = nullptr);
    ~Dialog_ShowLoadProfile();
    void setData(QStandardItemModel *loadprofileData, QString &app);

private:
    Ui::Dialog_ShowLoadProfile *ui;
    QCPBars *m_bar;
    QCPItemText *m_value_text[20];
};

#endif // DIALOG_BARCHART_H
