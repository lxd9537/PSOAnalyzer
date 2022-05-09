#include "csystemprofileinputdialog.h"

CSystemProfileInputDialog::CSystemProfileInputDialog(const QList<QString> &model_list,
                                                     const SystemProfile &system_profile,
                                                     QWidget *parent)
{
    QFont font("Microsoft YaHei", 9);
    m_model_list = model_list;
    m_system_profile = system_profile;
    this->setParent(parent);
    this->setFixedSize(510, 555);
    this->setFont(font);
    this->setWindowIcon(QIcon(":/ico/set"));
    this->setModal(true);
    this->setWindowTitle("系统及水泵参数");
    this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    this->setFixedSize(this->width(), this->height());
    m_button_box = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Close,
                                        Qt::Vertical, this);
    m_button_box->setGeometry(400, 470, 81, 65);

    /* 载入图片 */
    m_img = new QImage();
    m_img->load(":/img/profile");
    m_img_label = new QLabel(this);
    m_img_label->setPixmap(QPixmap::fromImage(*m_img));
    m_img_label->setGeometry(5,5,500,255);
    m_img_label->show();

    /* 添加属性页 */
    m_tab_widget = new QTabWidget(this);
    m_tab_widget->setFont(font);

    /* 添加公共输入画面 */
    m_common_profile_input = new CCommonProfileInput(m_system_profile.common_profile, this);
    m_tab_widget->addTab(m_common_profile_input, "公共参数");

    /* 添加水泵输入画面 */
    for (int i=0; i<m_system_profile.common_profile.pump_numbers; i++) {
        CPumpProfileInput *pump_input = new CPumpProfileInput(
                    m_model_list,
                    m_system_profile.pump_profile_list.at(i),
                    this);
        pump_input->setGeometry(QRect(5, 5, 280, 260));
        m_pump_profile_input.append(pump_input);
        m_tab_widget->addTab(pump_input, "水泵" + QString::number(i+1));
    }
    m_tab_widget->setGeometry(5,265,290,285);

    /* 对话框信号 */
    connect(m_button_box, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_button_box, SIGNAL(rejected()), this, SLOT(reject()));
}

CSystemProfileInputDialog::~CSystemProfileInputDialog() {

}
void CSystemProfileInputDialog::setViewOnly() {
    m_button_box->setStandardButtons(QDialogButtonBox::Close);
    //m_button_box->setCenterButtons(true);
}

CSystemProfileInputDialog::SystemProfile CSystemProfileInputDialog::getSysemProfile() {
    SystemProfile sys_profile;
    sys_profile.common_profile = m_common_profile_input->getCommonProfile();
    foreach(CPumpProfileInput *p_input, m_pump_profile_input) {
        CPumpProfileInput::PumpProfile p_profile = p_input->getPumpProfile();
        sys_profile.pump_profile_list.append(p_profile);
    }
    return sys_profile;
}

void CSystemProfileInputDialog::newSystemProfile(SystemProfile &profile, int pump_numbers) {
    profile.common_profile.pump_numbers = pump_numbers;
    profile.common_profile.out_pipe_dia = 0;
    profile.common_profile.out_pt_height = 0;
    profile.common_profile.in_tank_level.append(0);
    profile.common_profile.out_press_set.append(QPointF(0, 0.3));

    for (int i=0; i<pump_numbers; i++) {
        CPumpProfileInput::PumpProfile pump_profile;
        pump_profile.model = "";
        pump_profile.enabled = true;
        pump_profile.number = i+1;
        pump_profile.vfd_installed = false;
        pump_profile.vfd_max_speed = 100;
        pump_profile.vfd_min_speed = 0;
        pump_profile.in_pipe_coeff = 0;
        pump_profile.out_pipe_coeff = 0;
        pump_profile.in_pipe_dia = 500;
        pump_profile.out_pipe_dia = 500;
        pump_profile.in_pt_height = 0;
        pump_profile.out_pt_height = 0;
        profile.pump_profile_list.append(pump_profile);
    }
}

void CSystemProfileInputDialog::systemProfileToJSon(
        const CSystemProfileInputDialog::SystemProfile &profile, QByteArray &json) {
    QJsonObject root_obj;
    root_obj.insert("pump_numbers", profile.common_profile.pump_numbers);
    root_obj.insert("out_pt_height", profile.common_profile.out_pt_height);
    root_obj.insert("out_pipe_dia", profile.common_profile.out_pipe_dia);

    QJsonArray level_ary;
    for (int i=0; i<profile.common_profile.in_tank_level.size(); i++) {
        QJsonValue level_val = profile.common_profile.in_tank_level.at(i);
        level_ary.append(level_val);
    }
    root_obj.insert("levels", level_ary);

    QJsonArray press_ary;
    for (int i=0; i<profile.common_profile.out_press_set.size(); i++) {
        QJsonObject pair;
        pair.insert("flow", profile.common_profile.out_press_set.at(i).x());
        pair.insert("press", profile.common_profile.out_press_set.at(i).y());
        press_ary.append(pair);
    }
    root_obj.insert("press_settings", press_ary);

    QJsonArray pump_ary;
    foreach(CPumpProfileInput::PumpProfile pump_profile, profile.pump_profile_list) {
        QJsonObject pump_obj;
        pump_obj.insert("model", pump_profile.model);
        pump_obj.insert("enabled", pump_profile.enabled);
        pump_obj.insert("no", pump_profile.number);
        pump_obj.insert("vfd_installed", pump_profile.vfd_installed);
        pump_obj.insert("vfd_max_speed", pump_profile.vfd_max_speed);
        pump_obj.insert("vfd_min_speed", pump_profile.vfd_min_speed);
        pump_obj.insert("in_pipe_coeff", pump_profile.in_pipe_coeff);
        pump_obj.insert("out_pipe_coeff", pump_profile.out_pipe_coeff);
        pump_obj.insert("in_pipe_dia", pump_profile.in_pipe_dia);
        pump_obj.insert("out_pipe_dia", pump_profile.out_pipe_dia);
        pump_obj.insert("in_pt_height", pump_profile.in_pt_height);
        pump_obj.insert("out_pt_height", pump_profile.out_pt_height);
        pump_ary.append(pump_obj);
    }
    root_obj.insert("pump_profiles", pump_ary);
    QJsonDocument doc;
    doc.setObject(root_obj);
    json = doc.toJson();
}
bool CSystemProfileInputDialog::JSonToSystemProfile(const QJsonObject &root_obj, CSystemProfileInputDialog::SystemProfile &system_profile) {
    system_profile.common_profile.pump_numbers = root_obj.value("pump_numbers").toInt();
    system_profile.common_profile.out_pt_height = root_obj.value("out_pt_height").toDouble();
    system_profile.common_profile.out_pipe_dia = root_obj.value("out_pipe_dia").toDouble();
    if (root_obj.value("levels").isArray()) {
        QJsonArray levels = root_obj.value("levels").toArray();
        if (levels.size() > 0) {
            QList<float> level_list;
            for (int i=0; i<levels.size(); i++) {
                level_list.append(levels.at(i).toDouble());
            }
            system_profile.common_profile.in_tank_level = level_list;
        }
    }
    if (root_obj.value("press_settings").isArray()) {
        QJsonArray press_settings = root_obj.value("press_settings").toArray();
        if (press_settings.size() > 0) {
            QList<QPointF> press_list;
            for (int i=0; i<press_settings.size(); i++) {
                QPointF point;
                if (press_settings.at(i).isObject()) {
                    QJsonObject point_obj = press_settings.at(i).toObject();
                    point.setX(point_obj.value("flow").toDouble());
                    point.setY(point_obj.value("press").toDouble());
                }
                press_list.append(point);
            }
            system_profile.common_profile.out_press_set = press_list;
        }
    }

    if (system_profile.common_profile.pump_numbers >= 1 and system_profile.common_profile.pump_numbers <=8) {
        if (root_obj.value("pump_profiles").isArray()) {
            QJsonArray pumps = root_obj.value("pump_profiles").toArray();
            if (pumps.size() > 0) {
                QList<QJsonObject> pump_obj_list;
                for(QJsonArray::Iterator it = pumps.begin(); it != pumps.end(); it++)
                {
                    if (it[0].isObject()) {
                        pump_obj_list.append(it[0].toObject());
                    }
                }
                foreach(const QJsonObject &pump_obj, pump_obj_list) {
                    CPumpProfileInput::PumpProfile pump_profile;
                    pump_profile.model = pump_obj.value("model").toString();
                    pump_profile.enabled = pump_obj.value("enabled").toBool();
                    pump_profile.number = pump_obj.value("no").toInt();
                    pump_profile.vfd_installed = pump_obj.value("vfd_installed").toBool();
                    pump_profile.vfd_max_speed = pump_obj.value("vfd_max_speed").toDouble();
                    pump_profile.vfd_min_speed = pump_obj.value("vfd_min_speed").toDouble();
                    pump_profile.in_pipe_coeff = pump_obj.value("in_pipe_coeff").toDouble();
                    pump_profile.out_pipe_coeff = pump_obj.value("out_pipe_coeff").toDouble();
                    pump_profile.in_pipe_dia = pump_obj.value("in_pipe_dia").toDouble();
                    pump_profile.out_pipe_dia = pump_obj.value("out_pipe_dia").toDouble();
                    pump_profile.in_pt_height = pump_obj.value("in_pt_height").toDouble();
                    pump_profile.out_pt_height = pump_obj.value("out_pt_height").toDouble();
                    system_profile.pump_profile_list.append(pump_profile);
                }
                if (system_profile.common_profile.pump_numbers == system_profile.pump_profile_list.size()
                        && system_profile.common_profile.pump_numbers > 0) {
                    return true;
                }
            }
        }
    }
    return false;
}
