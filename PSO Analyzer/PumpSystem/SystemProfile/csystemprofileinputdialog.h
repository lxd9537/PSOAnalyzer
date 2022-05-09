#ifndef CSYSTEMPROFILEINPUTDIALOG_H
#define CSYSTEMPROFILEINPUTDIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QLabel>
#include <QImage>
#include <QPixmap>
#include <QSpinBox>
#include <QDialogButtonBox>
#include "qjsonobject.h"
#include "qjsonarray.h"
#include "qjsonvalue.h"
#include "qjsondocument.h"
#include <PumpSystem/SystemProfile/ccommonprofileinput.h>
#include <PumpSystem/SystemProfile/cpumpprofileinput.h>

class CSystemProfileInputDialog : public QDialog
{
public:
    typedef struct {
        CCommonProfileInput::CommonProfile common_profile;
        QList<CPumpProfileInput::PumpProfile> pump_profile_list;
    } SystemProfile;
public:
    CSystemProfileInputDialog(const QList<QString> &model_list,
                              const SystemProfile &system_profile,
                              QWidget *parent = nullptr);
    ~CSystemProfileInputDialog();
    void setViewOnly();
    SystemProfile getSysemProfile();
    static void newSystemProfile(SystemProfile &profile, int pump_numbers);
    static void systemProfileToJSon(const SystemProfile &profile, QByteArray &json);
    static bool JSonToSystemProfile(const QJsonObject &root_obj, SystemProfile &system_profile);

private:
    QTabWidget *m_tab_widget;
    QLabel *m_img_label;
    QImage *m_img;
    QDialogButtonBox *m_button_box;

    QList<QString> m_model_list;
    SystemProfile m_system_profile;
    QList<CPumpProfileInput*> m_pump_profile_input;
    CCommonProfileInput *m_common_profile_input;

};

#endif // CSYSTEMPROFILEINPUTDIALOG_H
