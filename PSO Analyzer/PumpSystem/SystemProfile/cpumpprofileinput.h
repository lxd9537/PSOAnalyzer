#ifndef CPUMPPROFILEINPUT_H
#define CPUMPPROFILEINPUT_H

#include <QWidget>
#include <qttreepropertybrowser.h>
#include <qtvariantproperty.h>
#include <qtpropertymanager.h>
#include <qteditorfactory.h>
#include <qpushbutton.h>
#include <QGuiApplication>

class CPumpProfileInput : public QWidget
{
    Q_OBJECT
public:
    typedef struct{
        QString model;
        int number;
        bool enabled;
        bool vfd_installed;
        float vfd_max_speed; //%
        float vfd_min_speed; //%
        float in_pipe_coeff; //Pa/(m3/h)^2
        float out_pipe_coeff; //Pa/(m3/h)^2
        float in_pipe_dia; //mm
        float out_pipe_dia; //mm
        float in_pt_height; //m
        float out_pt_height; //m
    }PumpProfile;

public:
    explicit CPumpProfileInput(const QList<QString> &model_list, const PumpProfile &profile, QWidget *parent = nullptr);
    ~CPumpProfileInput();
    PumpProfile getPumpProfile();
    void putPumpProfile(PumpProfile pump_profile);

private slots:
    void on_copyAll_clicked();
    void on_pasteAll_clicked();

private:
    PumpProfile m_pump_profile;
    const QList<QString> &ex_model_list;
    const PumpProfile &ex_profile;

    QPushButton *m_copyAll;
    QPushButton *m_pasteAll;

    QtTreePropertyBrowser *m_propertyBrowser;
    QMap<QString, QtProperty*> m_property_map;

    QtIntPropertyManager *m_int_manager;
    QtEnumPropertyManager *m_enum_manager;
    QtDoublePropertyManager *m_double_manager;
    QtBoolPropertyManager *m_bool_manager;
    QtGroupPropertyManager *m_group_manager;
    QtEnumEditorFactory *m_enum_factory;
    QtCheckBoxFactory *m_checkbox_factory;
    QtDoubleSpinBoxFactory *m_double_factory;
};

#endif // CPUMPPROFILEINPUT_H
