#include "ccommonprofileinput.h"

CCommonProfileInput::CCommonProfileInput(const CommonProfile &profile, QWidget *parent)
    : QWidget{parent}
{
    m_profile = profile;
    QFont font("Microsoft YaHei", 9);

    m_propertyBrowser = new QtTreePropertyBrowser(this);
    m_propertyBrowser->setResizeMode(QtTreePropertyBrowser::ResizeToContents);
    m_propertyBrowser->setGeometry(5,5,270,220);
    m_propertyBrowser->setFont(font);

    m_group_manager = new QtGroupPropertyManager(m_propertyBrowser);

    m_double_manager = new QtDoublePropertyManager(m_propertyBrowser);
    m_double_factory = new QtDoubleSpinBoxFactory(m_propertyBrowser);
    m_propertyBrowser->setFactoryForManager(m_double_manager, m_double_factory);

    m_int_manager = new QtIntPropertyManager(m_propertyBrowser);
    //m_int_factory =  new QtSpinBoxFactory(m_propertyBrowser);
    //m_propertyBrowser->setFactoryForManager(m_int_manager, m_int_factory);

    QtProperty *property_item;
    QtProperty *group_item;

    property_item = m_int_manager->addProperty("水泵数量");
    m_int_manager->setValue(property_item, profile.pump_numbers);
    m_property_map["水泵数量"] = property_item;
    m_propertyBrowser->addProperty(property_item);

    property_item = m_double_manager->addProperty("出口测压点高度H3(m)");
    m_double_manager->setDecimals(property_item, 2);
    m_double_manager->setSingleStep(property_item, 0.01);
    m_double_manager->setRange(property_item, 0, 100);
    m_double_manager->setValue(property_item, profile.out_pt_height);
    m_property_map["出口测压点高度"] = property_item;
    m_propertyBrowser->addProperty(property_item);

    property_item = m_double_manager->addProperty("出口管径D2(mm)");
    m_double_manager->setDecimals(property_item, 0);
    m_double_manager->setSingleStep(property_item, 1);
    m_double_manager->setRange(property_item, 0, 10000);
    m_double_manager->setValue(property_item, profile.out_pipe_dia);
    m_property_map["出口管径"] = property_item;
    m_propertyBrowser->addProperty(property_item);

    /* 取水池液位分组 */
    group_item = m_group_manager->addProperty(QStringLiteral("取水池液位h0(m)"));
    for (int i=0;  i < profile.in_tank_level.size(); i++) {
        property_item = m_double_manager->addProperty("液位" + QString::number(i));
        m_double_manager->setDecimals(property_item, 2);
        m_double_manager->setRange(property_item, 0, 100);
        m_double_manager->setValue(property_item, profile.in_tank_level.at(i));
        group_item->addSubProperty(property_item);
        m_property_map_level["液位" + QString::number(i)] = property_item;
    }
    m_propertyBrowser->addProperty(group_item);
    m_propertyBrowser->setExpanded(m_propertyBrowser->items(group_item).at(0), false);

    /* 流量/压力设定分组 */
    group_item = m_group_manager->addProperty(QStringLiteral("出水压力设定(m3/h, MPa)"));
    for (int i=0;  i < profile.out_press_set.size(); i++) {
        property_item = m_double_manager->addProperty("流量" + QString::number(i));
        m_double_manager->setDecimals(property_item, 1);
        m_double_manager->setRange(property_item, 0, 100000);
        m_double_manager->setValue(property_item, profile.out_press_set.at(i).x());
        group_item->addSubProperty(property_item);
        m_property_map_flow["流量" + QString::number(i)] = property_item;

        property_item = m_double_manager->addProperty("压力" + QString::number(i));
        m_double_manager->setDecimals(property_item, 3);
        m_double_manager->setRange(property_item, 0, 9);
        m_double_manager->setValue(property_item, profile.out_press_set.at(i).y());
        group_item->addSubProperty(property_item);
        m_property_map_press["压力" + QString::number(i)] = property_item;
    }
    m_propertyBrowser->addProperty(group_item);
    m_propertyBrowser->setExpanded(m_propertyBrowser->items(group_item).at(0), false);

    /* 连接插入及删除信号 */
    connect(m_propertyBrowser, &QtTreePropertyBrowser::insertProperty,
            this, &CCommonProfileInput::onPropertyInsertRequired);
    connect(m_propertyBrowser, &QtTreePropertyBrowser::deleteProperty,
            this, &CCommonProfileInput::onPropertyDeleteRequired);
}

CCommonProfileInput::CommonProfile CCommonProfileInput::getCommonProfile() {
    CommonProfile profile;
    profile.pump_numbers = m_property_map["水泵数量"]->valueText().toInt();
    profile.out_pt_height = m_property_map["出口测压点高度"]->valueText().toFloat();
    profile.out_pipe_dia = m_property_map["出口管径"]->valueText().toFloat();
    QMap<QString, QtProperty*>::const_iterator iter_1, iter_2;
    for(iter_1 = m_property_map_level.cbegin(); iter_1 != m_property_map_level.cend(); iter_1 ++) {
        profile.in_tank_level.append(iter_1.value()->valueText().toFloat());
    }
    for (iter_1 = m_property_map_flow.cbegin(), iter_2 = m_property_map_press.cbegin();
         iter_1 != m_property_map_flow.cend() && iter_2 != m_property_map_press.cend();
         iter_1 ++, iter_2 ++) {
        float flow = iter_1.value()->valueText().toFloat();
        float press = iter_2.value()->valueText().toFloat();
        profile.out_press_set.append(QPointF(flow, press));
    }
    return profile;
}

void CCommonProfileInput::onPropertyInsertRequired(QString property_name) {
    if (property_name.startsWith("取水池液位")) {
        foreach(QtProperty *prop, m_propertyBrowser->properties()) {
            if (prop->propertyName().startsWith("取水池液位")) {
                QString name;
                QString new_name;
                if (prop->subProperties().size() > 0) {
                    name = prop->subProperties().constLast()->propertyName();
                    name.remove("液位");
                    new_name = "液位" + QString::number(name.toInt() + 1);
                } else {
                    new_name = "液位0";
                }
                QtProperty *property_item;
                property_item = m_double_manager->addProperty(new_name);
                m_double_manager->setDecimals(property_item, 2);
                m_double_manager->setRange(property_item, 0, 100);
                m_double_manager->setValue(property_item, 0);
                prop->addSubProperty(property_item);
                m_property_map_level[new_name] = property_item;
                m_propertyBrowser->setExpanded(m_propertyBrowser->items(prop).at(0), true);
            }
        }
    }
    if (property_name.startsWith("出水压力设定")) {
        foreach(QtProperty *prop, m_propertyBrowser->properties()) {
            if (prop->propertyName().startsWith("出水压力设定")) {
                QString name;
                QString new_name_1, new_name_2;
                if (prop->subProperties().size() > 0) {
                    name = prop->subProperties().constLast()->propertyName();
                    if (name.contains("流量"))
                        name.remove("流量");
                    else
                        name.remove("压力");
                    new_name_1 = "流量" + QString::number(name.toInt() + 1);
                    new_name_2 = "压力" + QString::number(name.toInt() + 1);
                } else {
                    new_name_1 = "流量0";
                    new_name_2 = "压力0";
                }
                QtProperty *property_item;
                property_item = m_double_manager->addProperty(new_name_1);
                m_double_manager->setDecimals(property_item, 1);
                m_double_manager->setRange(property_item, 0, 100000);
                m_double_manager->setValue(property_item, 0);
                prop->addSubProperty(property_item);
                m_property_map_flow[new_name_1] = property_item;

                property_item = m_double_manager->addProperty(new_name_2);
                m_double_manager->setDecimals(property_item, 3);
                m_double_manager->setRange(property_item, 0, 9);
                m_double_manager->setValue(property_item, 0.3);
                prop->addSubProperty(property_item);
                m_property_map_press[new_name_2] = property_item;

                m_propertyBrowser->setExpanded(m_propertyBrowser->items(prop).at(0), true);
            }
        }
    }
}
void CCommonProfileInput::onPropertyDeleteRequired(QString property_name) {
    if (property_name.startsWith("液位")) {
        foreach(QtProperty *prop, m_propertyBrowser->properties()) {
            if (prop->propertyName().startsWith("取水池液位")) {
                prop->removeSubProperty(m_property_map_level[property_name]);
                m_property_map_level.remove(property_name);
            }
        }
    }
    if (property_name.startsWith("流量") || property_name.startsWith("压力")) {
        foreach(QtProperty *prop, m_propertyBrowser->properties()) {
            if (prop->propertyName().startsWith("出水压力设定")) {
                if (property_name.contains("流量")) {
                    prop->removeSubProperty(m_property_map_flow[property_name]);
                    m_property_map_flow.remove(property_name);
                    property_name.replace("流量", "压力");
                    prop->removeSubProperty(m_property_map_press[property_name]);
                    m_property_map_press.remove(property_name);
                } else if (property_name.contains("压力")) {
                    prop->removeSubProperty(m_property_map_press[property_name]);
                    m_property_map_press.remove(property_name);
                    property_name.replace("压力", "流量");
                    prop->removeSubProperty(m_property_map_flow[property_name]);
                    m_property_map_flow.remove(property_name);
                }
            }
        }
    }
}
