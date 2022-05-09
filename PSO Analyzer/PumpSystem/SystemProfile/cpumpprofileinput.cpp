#include "cpumpprofileinput.h"
#include <QClipboard>
#include <QtDebug>


CPumpProfileInput::CPumpProfileInput(const QList<QString> &model_list,  const PumpProfile &profile, QWidget *parent) :
    QWidget(parent),
    ex_model_list(model_list),
    ex_profile(profile)
{
    QFont font("Microsoft YaHei", 9);

    m_propertyBrowser = new QtTreePropertyBrowser(this);
    m_propertyBrowser->setResizeMode(QtTreePropertyBrowser::ResizeToContents);
    m_propertyBrowser->setGeometry(5,5,270,220);
    m_propertyBrowser->setFont(font);

    m_copyAll = new QPushButton(this);
    m_copyAll->setGeometry(QRect(60, 230, 75, 24));
    m_copyAll->raise();
    m_copyAll->setText("复制全部");
    m_copyAll->setFont(font);
    m_pasteAll = new QPushButton(this);
    m_pasteAll->setGeometry(QRect(140, 230, 75, 24));
    m_pasteAll->raise();
    m_pasteAll->setText("粘贴全部");
    m_pasteAll->setFont(font);

    m_int_manager = new QtIntPropertyManager(m_propertyBrowser);
    m_enum_manager = new QtEnumPropertyManager(m_propertyBrowser);
    m_double_manager = new QtDoublePropertyManager(m_propertyBrowser);
    m_bool_manager = new QtBoolPropertyManager(m_propertyBrowser);
    m_group_manager = new QtGroupPropertyManager(m_propertyBrowser);
    m_enum_factory = new QtEnumEditorFactory(m_propertyBrowser);
    m_checkbox_factory = new QtCheckBoxFactory(m_propertyBrowser);
    m_double_factory = new QtDoubleSpinBoxFactory(m_propertyBrowser);
    m_propertyBrowser->setFactoryForManager(m_enum_manager, m_enum_factory);
    m_propertyBrowser->setFactoryForManager(m_bool_manager, m_checkbox_factory);
    m_propertyBrowser->setFactoryForManager(m_double_manager, m_double_factory);

    QtProperty *property_item;
    QtProperty *group_item;

    /* 水泵编号 */
    property_item = m_int_manager->addProperty(QStringLiteral("水泵编号"));
    m_int_manager->setValue(property_item, profile.number);
    m_propertyBrowser->addProperty(property_item);
    m_property_map["水泵编号"] = property_item;

    /* 水泵使能 */
    property_item = m_bool_manager->addProperty(QStringLiteral("水泵使能"));
    m_bool_manager->setValue(property_item, profile.enabled);
    m_propertyBrowser->addProperty(property_item);
    m_property_map["水泵使能"] = property_item;

    /* 水泵型号 */
    property_item = m_enum_manager->addProperty("水泵型号");
    m_enum_manager->setEnumNames(property_item, model_list);
    int index = model_list.indexOf(profile.model);
    m_enum_manager->setValue(property_item, index);
    m_propertyBrowser->addProperty(property_item);
    m_property_map["水泵型号"] = property_item;

    /* 变频控制分组 */
    group_item = m_group_manager->addProperty(QStringLiteral("变频控制"));

    property_item = m_bool_manager->addProperty("已安装变频");
    m_bool_manager->setValue(property_item, profile.vfd_installed);
    group_item->addSubProperty(property_item);
    m_property_map["已安装变频"] = property_item;

    property_item = m_double_manager->addProperty("频率上限(%)");
    m_double_manager->setDecimals(property_item, 1);
    m_double_manager->setRange(property_item, 0, 100);
    m_double_manager->setValue(property_item, profile.vfd_max_speed);
    group_item->addSubProperty(property_item);
    m_property_map["频率上限"] = property_item;

    property_item = m_double_manager->addProperty("频率下限(%)");
    m_double_manager->setDecimals(property_item, 1);
    m_double_manager->setRange(property_item, 0, 100);
    m_double_manager->setValue(property_item, profile.vfd_min_speed);
    group_item->addSubProperty(property_item);
    m_property_map["频率下限"] = property_item;

    m_propertyBrowser->addProperty(group_item);
    m_propertyBrowser->setExpanded(m_propertyBrowser->items(group_item).at(0), false);

    /* 沿程损失分组 */
    group_item = m_group_manager->addProperty(QStringLiteral("沿程损失系数(Pa@1m3/h)"));

    property_item = m_double_manager->addProperty("入口管路K0");
    m_double_manager->setDecimals(property_item, 5);
    m_double_manager->setRange(property_item, 0, 10000);
    m_double_manager->setValue(property_item, profile.in_pipe_coeff);
    group_item->addSubProperty(property_item);
    m_property_map["入口管路K0"] = property_item;

    property_item = m_double_manager->addProperty("出口管路K1");
    m_double_manager->setDecimals(property_item, 5);
    m_double_manager->setRange(property_item, 0, 10000);
    m_double_manager->setValue(property_item, profile.out_pipe_coeff);
    group_item->addSubProperty(property_item);
    m_property_map["出口管路K1"] = property_item;

    m_propertyBrowser->addProperty(group_item);
    m_propertyBrowser->setExpanded(m_propertyBrowser->items(group_item).at(0), false);

    /* 管径分组 */
    group_item = m_group_manager->addProperty(QStringLiteral("进出口测压处管径(mm)"));

    property_item = m_double_manager->addProperty("入口管路D0");
    m_double_manager->setDecimals(property_item, 0);
    m_double_manager->setRange(property_item, 0, 10000);
    m_double_manager->setValue(property_item, profile.in_pipe_dia);
    group_item->addSubProperty(property_item);
    m_property_map["入口管路D0"] = property_item;

    property_item = m_double_manager->addProperty("出口管路D1");
    m_double_manager->setDecimals(property_item, 0);
    m_double_manager->setRange(property_item, 0, 10000);
    m_double_manager->setValue(property_item, profile.out_pipe_dia);
    group_item->addSubProperty(property_item);
    m_property_map["出口管路D1"] = property_item;

    m_propertyBrowser->addProperty(group_item);
    m_propertyBrowser->setExpanded(m_propertyBrowser->items(group_item).at(0), false);

    /* 高程分组 */
    group_item = m_group_manager->addProperty(QStringLiteral("进出口测压处高程(m)"));

    property_item = m_double_manager->addProperty("入口测压H1");
    m_double_manager->setDecimals(property_item, 0);
    m_double_manager->setRange(property_item, 0, 10000);
    m_double_manager->setValue(property_item, profile.in_pt_height);
    group_item->addSubProperty(property_item);
    m_property_map["入口测压H1"] = property_item;

    property_item = m_double_manager->addProperty("出口测压H2");
    m_double_manager->setDecimals(property_item, 0);
    m_double_manager->setRange(property_item, 0, 10000);
    m_double_manager->setValue(property_item, profile.out_pt_height);
    group_item->addSubProperty(property_item);
    m_property_map["出口测压H2"] = property_item;

    m_propertyBrowser->addProperty(group_item);
    m_propertyBrowser->setExpanded(m_propertyBrowser->items(group_item).at(0), false);

    /* 按钮连接 */
    connect(m_copyAll, &QPushButton::clicked, this, &CPumpProfileInput::on_copyAll_clicked);
    connect(m_pasteAll, &QPushButton::clicked, this, &CPumpProfileInput::on_pasteAll_clicked);
}

CPumpProfileInput::~CPumpProfileInput()
{
    delete m_propertyBrowser;
}

CPumpProfileInput::PumpProfile CPumpProfileInput::getPumpProfile() {
    PumpProfile pump_profile;

    pump_profile.number = m_property_map["水泵编号"]->valueText().toInt();
    pump_profile.enabled = m_property_map["水泵使能"]->valueText() == "True" ? true : false;
    pump_profile.model = m_property_map["水泵型号"]->valueText();
    pump_profile.vfd_installed = m_property_map["已安装变频"]->valueText() == "True" ? true : false;
    pump_profile.vfd_max_speed = m_property_map["频率上限"]->valueText().toFloat();
    pump_profile.vfd_min_speed = m_property_map["频率下限"]->valueText().toFloat();
    pump_profile.in_pipe_coeff = m_property_map["入口管路K0"]->valueText().toFloat();
    pump_profile.out_pipe_coeff = m_property_map["出口管路K1"]->valueText().toFloat();
    pump_profile.in_pipe_dia = m_property_map["入口管路D0"]->valueText().toFloat();
    pump_profile.out_pipe_dia = m_property_map["出口管路D1"]->valueText().toFloat();
    pump_profile.in_pt_height = m_property_map["入口测压H1"]->valueText().toFloat();
    pump_profile.out_pt_height = m_property_map["出口测压H2"]->valueText().toFloat();

    return pump_profile;
}
void CPumpProfileInput::putPumpProfile(CPumpProfileInput::PumpProfile pump_profile) {
    int index = ex_model_list.indexOf(pump_profile.model);
    m_enum_manager->setValue(m_property_map["水泵型号"], index);
    m_bool_manager->setValue(m_property_map["水泵使能"], pump_profile.enabled);
    m_bool_manager->setValue(m_property_map["已安装变频"], pump_profile.vfd_installed);
    m_double_manager->setValue(m_property_map["频率上限"], pump_profile.vfd_max_speed);
    m_double_manager->setValue(m_property_map["频率下限"], pump_profile.vfd_min_speed);
    m_double_manager->setValue(m_property_map["入口管路K0"], pump_profile.in_pipe_coeff);
    m_double_manager->setValue(m_property_map["出口管路K1"], pump_profile.out_pipe_coeff);
    m_double_manager->setValue(m_property_map["入口管路D0"], pump_profile.in_pipe_dia);
    m_double_manager->setValue(m_property_map["出口管路D1"], pump_profile.out_pipe_dia);
    m_double_manager->setValue(m_property_map["入口测压H1"], pump_profile.in_pt_height);
    m_double_manager->setValue(m_property_map["出口测压H2"], pump_profile.out_pt_height);
}
void CPumpProfileInput::on_copyAll_clicked()
{
    PumpProfile pump_profile;
    pump_profile = getPumpProfile();

    QString text;
    text.append("pumpprofile,");
    text.append(pump_profile.model + ",");
    text.append(pump_profile.enabled ? "true," : "false,");
    text.append(pump_profile.vfd_installed ? "true," : "false,");
    text.append(QString::number(pump_profile.vfd_max_speed, 'f', 1) + ",");
    text.append(QString::number(pump_profile.vfd_min_speed, 'f', 1) + ",");
    text.append(QString::number(pump_profile.in_pipe_coeff, 'f', 5) + ",");
    text.append(QString::number(pump_profile.out_pipe_coeff, 'f', 5) + ",");
    text.append(QString::number(pump_profile.in_pipe_dia, 'f', 0) + ",");
    text.append(QString::number(pump_profile.out_pipe_dia, 'f', 0) + ",");
    text.append(QString::number(pump_profile.in_pt_height, 'f', 2) + ",");
    text.append(QString::number(pump_profile.out_pt_height, 'f', 2));

    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(text);
}


void CPumpProfileInput::on_pasteAll_clicked()
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    QString text;
    QStringList split_text;
    text = clipboard->text();
    split_text = text.split(',', Qt::KeepEmptyParts);
    PumpProfile pump_profile;
    bool all_ok = true;
    if (split_text.size() == 12) {
        if (split_text.at(0) == "pumpprofile") {
            bool ok;
            pump_profile.model = split_text.at(1);
            pump_profile.enabled = (split_text.at(2) == "true" ? true : false);;
            pump_profile.vfd_installed = (split_text.at(3) == "true" ? true : false);
            pump_profile.vfd_max_speed = split_text.at(4).toFloat(&ok);
            all_ok = all_ok && ok;
            pump_profile.vfd_min_speed = split_text.at(5).toFloat(&ok);
            all_ok = all_ok && ok;
            pump_profile.in_pipe_coeff = split_text.at(6).toFloat(&ok);
            all_ok = all_ok && ok;
            pump_profile.out_pipe_coeff = split_text.at(7).toFloat(&ok);
            all_ok = all_ok && ok;
            pump_profile.in_pipe_dia = split_text.at(8).toFloat(&ok);
            all_ok = all_ok && ok;
            pump_profile.out_pipe_dia = split_text.at(9).toFloat(&ok);
            all_ok = all_ok && ok;
            pump_profile.in_pt_height = split_text.at(10).toFloat(&ok);
            all_ok = all_ok && ok;
            pump_profile.out_pt_height = split_text.at(11).toFloat(&ok);
            all_ok = all_ok && ok;
            if (all_ok)
                putPumpProfile(pump_profile);
        }
    }
}

