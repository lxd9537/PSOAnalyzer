#ifndef CCOMMONPROFILEINPUT_H
#define CCOMMONPROFILEINPUT_H

#include <QWidget>
#include <qttreepropertybrowser.h>
#include <qtpropertymanager.h>
#include <qteditorfactory.h>
#include <QMap>

class CCommonProfileInput : public QWidget
{
    Q_OBJECT
public:
    typedef struct {
        int pump_numbers;
        float out_pt_height;
        float out_pipe_dia;
        QList<float> in_tank_level;
        QList<QPointF> out_press_set;
    } CommonProfile;
public:
    explicit CCommonProfileInput(const CommonProfile &profile, QWidget *parent = nullptr);
    CommonProfile getCommonProfile();

private slots:
    void onPropertyInsertRequired(QString property_name);
    void onPropertyDeleteRequired(QString property_name);
private:
    CommonProfile m_profile;

    QtTreePropertyBrowser *m_propertyBrowser;
    QMap<QString, QtProperty*> m_property_map;
    QMap<QString, QtProperty*> m_property_map_level;
    QMap<QString, QtProperty*> m_property_map_flow;
    QMap<QString, QtProperty*> m_property_map_press;
    QtDoublePropertyManager *m_double_manager;
    QtIntPropertyManager *m_int_manager;
    QtGroupPropertyManager *m_group_manager;
    QtDoubleSpinBoxFactory *m_double_factory;
    QtSpinBoxFactory *m_int_factory;
};

#endif // CCOMMONPROFILEINPUT_H
