/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the QtSerialBus module.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "modifydialog.h"
#include "ui_modifydialog.h"
#include "pumpunit.h"
#include "qxtcsvmodel.h"
#include "QMessageBox"

ModifyDialog::ModifyDialog(QWidget *parent):
    QDialog(parent),
    ui(new Ui::ModifyDialog)
{
    ui->setupUi(this);
    m_input_dialog_int = new IntInputDialog(this);
    m_input_dialog_double = new DoubleInputDialog(this);

    //connections
    connect(ui->cancelButton, &QPushButton::clicked, [this]() {hide();});
    connect(ui->applyButton, &QPushButton::clicked, this, &ModifyDialog::on_applyButton_clicked);

    connect(ui->Edit_duty_pumps,&IntInputEdit::clicked,this,&ModifyDialog::on_IntInputEdit_clicked);
    connect(ui->Edit_standby_pumps,&IntInputEdit::clicked,this,&ModifyDialog::on_IntInputEdit_clicked);
    connect(m_input_dialog_int,&IntInputDialog::ValueChanged,this,&ModifyDialog::on_IntInputEdit_changed);

    connect(ui->Edit_design_flow,&DoubleInputEdit::clicked,this,&ModifyDialog::on_DoubleInputEdit_clicked);
    connect(ui->Edit_design_head,&DoubleInputEdit::clicked,this,&ModifyDialog::on_DoubleInputEdit_clicked);
    connect(ui->Edit_static_head,&DoubleInputEdit::clicked,this,&ModifyDialog::on_DoubleInputEdit_clicked);
    connect(ui->Edit_well_diameter,&DoubleInputEdit::clicked,this,&ModifyDialog::on_DoubleInputEdit_clicked);
    connect(ui->Edit_well_height,&DoubleInputEdit::clicked,this,&ModifyDialog::on_DoubleInputEdit_clicked);
    connect(ui->Edit_high_float,&DoubleInputEdit::clicked,this,&ModifyDialog::on_DoubleInputEdit_clicked);
    connect(ui->Edit_low_float,&DoubleInputEdit::clicked,this,&ModifyDialog::on_DoubleInputEdit_clicked);
    connect(m_input_dialog_double,&DoubleInputDialog::ValueChanged,this,&ModifyDialog::on_DoubleInputEdit_changed);
}

ModifyDialog::~ModifyDialog()
{
    delete m_input_dialog_int;
    delete m_input_dialog_double;
    delete ui;
}
void ModifyDialog::Initiation(PumpUnit* pump_unit)
{
    m_pump_unit = pump_unit;
    PumpUnit::SysProfile system_profile = m_pump_unit->ReadSysProfile();

    ui->setmodelCombo->clear();
    ui->setmodelCombo->addItem("FC102");
    ui->setmodelCombo->addItem("ATV610");
    ui->setmodelCombo->addItem("ACQ580");
    ui->setmodelCombo->addItem("Hydrovar");
    ui->setmodelCombo->setCurrentIndex(system_profile.vfd_model-1);
    ui->Edit_duty_pumps->Initiation(system_profile.duty_pumps,0,6);
    ui->Edit_standby_pumps->Initiation(system_profile.standby_pumps,0,5);
    ui->Edit_design_flow->Initiation(system_profile.design_flow_total,1,9999);
    ui->Edit_design_head->Initiation(system_profile.design_head,0,999);
    ui->Edit_static_head->Initiation(system_profile.static_head,0,999);
    ui->Edit_well_diameter->Initiation(system_profile.well_diameter,0,10);
    ui->Edit_well_height->Initiation(system_profile.well_height,0,20);
    ui->Edit_high_float->Initiation(system_profile.height_of_high_float,0,20);
    ui->Edit_low_float->Initiation(system_profile.height_of_low_float,0,20);

}
void ModifyDialog::on_applyButton_clicked()
{
    PumpUnit::RawSysProfile profile;

    profile.vfd_model = static_cast<PumpUnit::VfdModel>(ui->setmodelCombo->currentIndex());
    profile.duty_pumps = ui->Edit_duty_pumps->GetCurrentValue();
    profile.standby_pumps = ui->Edit_standby_pumps->GetCurrentValue();
    profile.design_flow_total = ui->Edit_design_flow->GetCurrentValue();
    profile.design_head = ui->Edit_design_head->GetCurrentValue();
    profile.static_head = ui->Edit_static_head->GetCurrentValue();
    profile.well_diameter = ui->Edit_well_diameter->GetCurrentValue();
    profile.well_height = ui->Edit_well_height->GetCurrentValue();
    profile.height_of_high_float = ui->Edit_high_float->GetCurrentValue();
    profile.height_of_low_float = ui->Edit_low_float->GetCurrentValue();

    int error_code;
    QString error_message;

    if(m_pump_unit->WriteSysProfile(profile,&error_code,&error_message))
        hide();
    else QMessageBox::warning(nullptr, QObject::tr("Modify pump and system parameters"),error_message);
}

void ModifyDialog::on_IntInputEdit_clicked()
{
    m_current_int_edit = static_cast<IntInputEdit*>(sender());
    m_input_dialog_int->Initiation(
                m_current_int_edit->GetCurrentValue(),
                m_current_int_edit->GetMinValue(),
                m_current_int_edit->GetMaxValue());

    m_input_dialog_int->show();
}
void ModifyDialog::on_IntInputEdit_changed(int value)
{
    m_current_int_edit->SetCurrentValue(value);
}
void ModifyDialog::on_DoubleInputEdit_clicked()
{
    m_current_double_edit = static_cast<DoubleInputEdit*>(sender());
    m_input_dialog_double->Initiation(
                m_current_double_edit->GetCurrentValue(),
                m_current_double_edit->GetMinValue(),
                m_current_double_edit->GetMaxValue(),
                m_current_double_edit->GetDisplayFormat(),
                m_current_double_edit->GetDisplayPrecision());

    m_input_dialog_double->show();
}
void ModifyDialog::on_DoubleInputEdit_changed(double value)
{
    m_current_double_edit->SetCurrentValue(value);
}
