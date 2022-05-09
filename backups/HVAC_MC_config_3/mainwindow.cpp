#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButton,&QPushButton::clicked,this,&MainWindow::on_import_clicked);
    connect(ui->pushButton_2,&QPushButton::clicked,this,&MainWindow::on_export_clicked);
    connect(ui->pushButton_3,&QPushButton::clicked,this,&MainWindow::on_print_clicked);

    connect(ui->pushButton_100,&QPushButton::clicked,this,&MainWindow::on_import2_clicked);
    connect(ui->pushButton_101,&QPushButton::clicked,this,&MainWindow::on_export2_clicked);
    connect(ui->pushButton_102,&QPushButton::clicked,this,&MainWindow::on_print2_clicked);

    QDateTime dt = QDateTime::currentDateTime();
    ui->dateEdit->setDateTime(dt);
    ui->dateEdit_100->setDateTime(dt);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_import_clicked()
{
    QString filename;
    QFileDialog fileDialog(this);
    filename = fileDialog.getOpenFileName(this,"导入配置",QDir::currentPath(),"*.csv");

    if(filename=="")
        return;

    QxtCsvModel csvModel(this);
    csvModel.setSource(filename,false,',',nullptr);

    if(csvModel.rowCount() < 70 || csvModel.columnCount() < 2)
    {
        QMessageBox::warning(nullptr,"导入配置","请检查配置文件完整性!");
        return;
    }

    if(csvModel.text(0,1)!="HVAC")
    {
        QMessageBox::warning(nullptr,"导入配置","不是HVAC的配置文件!");
        return;
    }

    bool checked;

    ui->comboBox->setCurrentText(csvModel.text(1,1));
    ui->lineEdit->setText(csvModel.text(2,1));
    ui->lineEdit_2->setText(csvModel.text(4,1));
    ui->dateEdit->setDate(QDate::fromString(csvModel.text(3,1), "yyyy-MM-dd"));

    ui->comboBox_2->setCurrentText(csvModel.text(6,1));
    ui->comboBox_3->setCurrentText(csvModel.text(7,1));
    ui->comboBox_4->setCurrentText(csvModel.text(8,1));
    ui->comboBox_5->setCurrentText(csvModel.text(9,1));
    ui->comboBox_6->setCurrentText(csvModel.text(10,1));
    ui->comboBox_7->setCurrentText(csvModel.text(11,1));
    ui->comboBox_8->setCurrentText(csvModel.text(12,1));
    ui->comboBox_9->setCurrentText(csvModel.text(13,1));

    checked = (csvModel.text(14,1)=="有"?true:false);	    ui->checkBox->setChecked(checked);
    checked = (csvModel.text(15,1)=="有"?true:false);	    ui->checkBox_2->setChecked(checked);
    checked = (csvModel.text(16,1)=="有"?true:false);	    ui->checkBox_3->setChecked(checked);
    checked = (csvModel.text(17,1)=="有"?true:false);	    ui->checkBox_4->setChecked(checked);
    checked = (csvModel.text(18,1)=="有"?true:false);	    ui->checkBox_5->setChecked(checked);
    checked = (csvModel.text(19,1)=="有"?true:false);	    ui->checkBox_6->setChecked(checked);
    checked = (csvModel.text(20,1)=="有"?true:false);	    ui->checkBox_7->setChecked(checked);

    ui->comboBox_62->setCurrentText(csvModel.text(23,1));
    ui->comboBox_63->setCurrentText(csvModel.text(24,1));
    ui->comboBox_64->setCurrentText(csvModel.text(25,1));
    ui->comboBox_65->setCurrentText(csvModel.text(26,1));

    ui->comboBox_66->setCurrentText(csvModel.text(29,1));
    ui->comboBox_67->setCurrentText(csvModel.text(30,1));
    ui->comboBox_68->setCurrentText(csvModel.text(31,1));
    ui->comboBox_69->setCurrentText(csvModel.text(32,1));
    ui->comboBox_70->setCurrentText(csvModel.text(33,1));

    ui->comboBox_71->setCurrentText(csvModel.text(36,1));
    ui->comboBox_72->setCurrentText(csvModel.text(37,1));
    ui->comboBox_73->setCurrentText(csvModel.text(38,1));
    ui->comboBox_74->setCurrentText(csvModel.text(39,1));
    ui->comboBox_75->setCurrentText(csvModel.text(40,1));
    ui->comboBox_76->setCurrentText(csvModel.text(41,1));
    ui->comboBox_77->setCurrentText(csvModel.text(42,1));
    ui->comboBox_78->setCurrentText(csvModel.text(43,1));
    ui->comboBox_79->setCurrentText(csvModel.text(44,1));
    ui->comboBox_80->setCurrentText(csvModel.text(45,1));
    ui->comboBox_81->setCurrentText(csvModel.text(46,1));
    ui->comboBox_82->setCurrentText(csvModel.text(47,1));
    ui->comboBox_83->setCurrentText(csvModel.text(48,1));
    ui->comboBox_84->setCurrentText(csvModel.text(49,1));
    ui->comboBox_85->setCurrentText(csvModel.text(50,1));


    checked = (csvModel.text(53,1)=="有"?true:false);	    ui->checkBox_20->setChecked(checked);
    checked = (csvModel.text(54,1)=="有"?true:false);	    ui->checkBox_21->setChecked(checked);
    checked = (csvModel.text(55,1)=="有"?true:false);	    ui->checkBox_22->setChecked(checked);

    ui->comboBox_90->setCurrentText(csvModel.text(58,1));
    ui->comboBox_91->setCurrentText(csvModel.text(59,1));
    ui->comboBox_92->setCurrentText(csvModel.text(60,1));
    ui->comboBox_93->setCurrentText(csvModel.text(61,1));
    ui->comboBox_94->setCurrentText(csvModel.text(62,1));
    ui->comboBox_95->setCurrentText(csvModel.text(63,1));

    ui->lineEdit_3->setText(csvModel.text(68,1));
    ui->lineEdit_4->setText(csvModel.text(69,1));

    ui->textEdit->setText(csvModel.text(65,1));


    QMessageBox::information(nullptr,"导入配置","导入配置完成！");

}
void MainWindow::on_import2_clicked()
{
    QString filename;
    QFileDialog fileDialog(this);
    filename = fileDialog.getOpenFileName(this,"导入配置",QDir::currentPath(),"*.csv");

    if(filename=="")
        return;

    QxtCsvModel csvModel(this);
    csvModel.setSource(filename,false,',',nullptr);

    if(csvModel.rowCount() < 70 || csvModel.columnCount() < 2)
    {
        QMessageBox::warning(nullptr,"导入配置","请检查配置文件完整性!");
        return;
    }

    if(csvModel.text(0,1)!="供水")
    {
        QMessageBox::warning(nullptr,"导入配置","不是供水或消防的配置文件!");
        return;
    }

    bool checked;

    ui->comboBox_100->setCurrentText(csvModel.text(1,1));
    ui->lineEdit_100->setText(csvModel.text(2,1));
    ui->lineEdit_101->setText(csvModel.text(3,1));
    ui->dateEdit_100->setDate(QDate::fromString(csvModel.text(4,1), "yyyy-MM-dd"));

    ui->comboBox_110->setCurrentText(csvModel.text(6,1));
    ui->comboBox_111->setCurrentText(csvModel.text(7,1));
    ui->comboBox_112->setCurrentText(csvModel.text(8,1));
    ui->comboBox_113->setCurrentText(csvModel.text(9,1));
    ui->comboBox_114->setCurrentText(csvModel.text(10,1));
    ui->comboBox_115->setCurrentText(csvModel.text(11,1));
    ui->comboBox_116->setCurrentText(csvModel.text(12,1));

    checked = (csvModel.text(14,1)=="有"?true:false);	    ui->checkBox_110->setChecked(checked);
    checked = (csvModel.text(15,1)=="有"?true:false);	    ui->checkBox_111->setChecked(checked);
    checked = (csvModel.text(16,1)=="有"?true:false);	    ui->checkBox_112->setChecked(checked);
    checked = (csvModel.text(17,1)=="有"?true:false);	    ui->checkBox_113->setChecked(checked);
    checked = (csvModel.text(18,1)=="有"?true:false);	    ui->checkBox_114->setChecked(checked);
    checked = (csvModel.text(19,1)=="有"?true:false);	    ui->checkBox_115->setChecked(checked);
    checked = (csvModel.text(20,1)=="有"?true:false);	    ui->checkBox_116->setChecked(checked);
    checked = (csvModel.text(21,1)=="有"?true:false);	    ui->checkBox_117->setChecked(checked);
    checked = (csvModel.text(22,1)=="有"?true:false);	    ui->checkBox_118->setChecked(checked);

    ui->comboBox_120->setCurrentText(csvModel.text(25,1));
    ui->comboBox_121->setCurrentText(csvModel.text(26,1));
    ui->comboBox_122->setCurrentText(csvModel.text(27,1));
    ui->comboBox_123->setCurrentText(csvModel.text(26,1));

    ui->comboBox_130->setCurrentText(csvModel.text(31,1));
    ui->comboBox_131->setCurrentText(csvModel.text(32,1));
    ui->comboBox_132->setCurrentText(csvModel.text(33,1));
    ui->comboBox_133->setCurrentText(csvModel.text(34,1));
    ui->comboBox_134->setCurrentText(csvModel.text(35,1));
    ui->comboBox_135->setCurrentText(csvModel.text(36,1));

    ui->comboBox_140->setCurrentText(csvModel.text(39,1));
    ui->comboBox_141->setCurrentText(csvModel.text(40,1));
    ui->comboBox_142->setCurrentText(csvModel.text(41,1));
    ui->comboBox_143->setCurrentText(csvModel.text(42,1));
    ui->comboBox_144->setCurrentText(csvModel.text(43,1));
    ui->comboBox_145->setCurrentText(csvModel.text(44,1));
    ui->comboBox_146->setCurrentText(csvModel.text(45,1));
    ui->comboBox_147->setCurrentText(csvModel.text(46,1));
    ui->comboBox_148->setCurrentText(csvModel.text(47,1));
    ui->comboBox_149->setCurrentText(csvModel.text(48,1));
    ui->comboBox_150->setCurrentText(csvModel.text(49,1));
    ui->comboBox_151->setCurrentText(csvModel.text(50,1));
    ui->comboBox_152->setCurrentText(csvModel.text(51,1));
    ui->comboBox_153->setCurrentText(csvModel.text(52,1));
    ui->comboBox_154->setCurrentText(csvModel.text(53,1));


    checked = (csvModel.text(56,1)=="有"?true:false);	    ui->checkBox_180->setChecked(checked);
    checked = (csvModel.text(57,1)=="有"?true:false);	    ui->checkBox_181->setChecked(checked);
    checked = (csvModel.text(58,1)=="有"?true:false);	    ui->checkBox_182->setChecked(checked);

    ui->comboBox_160->setCurrentText(csvModel.text(61,1));
    ui->comboBox_161->setCurrentText(csvModel.text(62,1));
    ui->comboBox_162->setCurrentText(csvModel.text(63,1));
    ui->comboBox_163->setCurrentText(csvModel.text(64,1));
    ui->comboBox_164->setCurrentText(csvModel.text(65,1));


    ui->lineEdit_170->setText(csvModel.text(70,1));
    ui->lineEdit_171->setText(csvModel.text(71,1));

    ui->textEdit_100->setText(csvModel.text(67,1));


    QMessageBox::information(nullptr,"导入配置","导入配置完成！");

}
void MainWindow::on_export_clicked()
{
    //放入数据模型
    QVector<QStringList> *csvData;
    csvData = new QVector<QStringList>;
    QStringList row;

    //group 1
    row.clear();
    row.append(ui->groupBox->title());
    row.append("HVAC");
    csvData->append(row);

    row.clear();
    row.append(ui->label->text());
    row.append(ui->comboBox->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_2->text());
    row.append(ui->lineEdit->text());
    csvData->append(row);

    row.clear();
    row.append(ui->label_3->text());
    row.append(ui->dateEdit->date().toString("yyyy-MM-dd"));
    csvData->append(row);

    row.clear();
    row.append(ui->label_12->text());
    row.append(ui->lineEdit_2->text());
    csvData->append(row);

    //group 2
    row.clear();
    row.append(ui->groupBox_2->title());
    row.append("");
    csvData->append(row);

    row.clear();
    row.append(ui->label_4->text());
    row.append(ui->comboBox_2->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_5->text());
    row.append(ui->comboBox_3->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_6->text());
    row.append(ui->comboBox_4->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_7->text());
    row.append(ui->comboBox_5->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_8->text());
    row.append(ui->comboBox_6->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_9->text());
    row.append(ui->comboBox_7->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_10->text());
    row.append(ui->comboBox_8->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_11->text());
    row.append(ui->comboBox_9->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->checkBox->text());
    row.append(ui->checkBox->isChecked()?"有":"无");
    csvData->append(row);

    row.clear();
    row.append(ui->checkBox_2->text());
    row.append(ui->checkBox_2->isChecked()?"有":"无");
    csvData->append(row);

    row.clear();
    row.append(ui->checkBox_3->text());
    row.append(ui->checkBox_3->isChecked()?"有":"无");
    csvData->append(row);

    row.clear();
    row.append(ui->checkBox_4->text());
    row.append(ui->checkBox_4->isChecked()?"有":"无");
    csvData->append(row);

    row.clear();
    row.append(ui->checkBox_5->text());
    row.append(ui->checkBox_5->isChecked()?"有":"无");
    csvData->append(row);

    row.clear();
    row.append(ui->checkBox_6->text());
    row.append(ui->checkBox_6->isChecked()?"有":"无");
    csvData->append(row);

    row.clear();
    row.append(ui->checkBox_7->text());
    row.append(ui->checkBox_7->isChecked()?"有":"无");
    csvData->append(row);

    row.clear();
    row.append("");
    row.append("");
    csvData->append(row);

    //group 3
    row.clear();
    row.append(ui->groupBox_3->title());
    row.append("");
    csvData->append(row);

    row.clear();
    row.append(ui->label_62->text());
    row.append(ui->comboBox_62->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_63->text());
    row.append(ui->comboBox_63->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_64->text());
    row.append(ui->comboBox_64->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_65->text());
    row.append(ui->comboBox_65->currentText());
    csvData->append(row);

    row.clear();
    row.append("");
    row.append("");
    csvData->append(row);

    //group 4
    row.clear();
    row.append(ui->groupBox_4->title());
    row.append("");
    csvData->append(row);

    row.clear();
    row.append(ui->label_66->text());
    row.append(ui->comboBox_66->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_67->text());
    row.append(ui->comboBox_67->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_68->text());
    row.append(ui->comboBox_68->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_69->text());
    row.append(ui->comboBox_69->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_70->text());
    row.append(ui->comboBox_70->currentText());
    csvData->append(row);


    row.clear();
    row.append("");
    row.append("");
    csvData->append(row);

    //group 5
    row.clear();
    row.append(ui->groupBox_5->title());
    row.append("");
    csvData->append(row);

    row.clear();
    row.append(ui->label_71->text());
    row.append(ui->comboBox_71->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_72->text());
    row.append(ui->comboBox_72->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_73->text());
    row.append(ui->comboBox_73->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_74->text());
    row.append(ui->comboBox_74->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_75->text());
    row.append(ui->comboBox_75->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_76->text());
    row.append(ui->comboBox_76->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_77->text());
    row.append(ui->comboBox_77->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_78->text());
    row.append(ui->comboBox_78->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_79->text());
    row.append(ui->comboBox_79->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_80->text());
    row.append(ui->comboBox_80->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_81->text());
    row.append(ui->comboBox_81->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_82->text());
    row.append(ui->comboBox_82->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_83->text());
    row.append(ui->comboBox_83->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_84->text());
    row.append(ui->comboBox_84->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_85->text());
    row.append(ui->comboBox_85->currentText());
    csvData->append(row);

    row.clear();
    row.append("");
    row.append("");
    csvData->append(row);

    //group 6
    row.clear();
    row.append(ui->groupBox_6->title());
    row.append("");
    csvData->append(row);

    row.clear();
    row.append(ui->checkBox_20->text());
    row.append(ui->checkBox_20->isChecked()?"有":"无");
    csvData->append(row);

    row.clear();
    row.append(ui->checkBox_21->text());
    row.append(ui->checkBox_21->isChecked()?"有":"无");
    csvData->append(row);

    row.clear();
    row.append(ui->checkBox_22->text());
    row.append(ui->checkBox_22->isChecked()?"有":"无");
    csvData->append(row);

    row.clear();
    row.append("");
    row.append("");
    csvData->append(row);

    //group 7
    row.clear();
    row.append(ui->groupBox_7->title());
    row.append("");
    csvData->append(row);

    row.clear();
    row.append(ui->label_90->text());
    row.append(ui->comboBox_90->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_91->text());
    row.append(ui->comboBox_91->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_92->text());
    row.append(ui->comboBox_92->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_93->text());
    row.append(ui->comboBox_93->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_94->text());
    row.append(ui->comboBox_94->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_95->text());
    row.append(ui->comboBox_95->currentText());
    csvData->append(row);

    row.clear();
    row.append("");
    row.append("");
    csvData->append(row);

    //group 8
    row.clear();
    row.append(ui->groupBox_8->title());
    row.append(ui->textEdit->toPlainText());
    csvData->append(row);

    row.clear();
    row.append("");
    row.append("");
    csvData->append(row);

    //group 9
    row.clear();
    row.append(ui->groupBox_9->title());
    row.append("");
    csvData->append(row);

    row.clear();
    row.append(ui->label_13->text());
    row.append(ui->lineEdit_3->text());
    csvData->append(row);

    row.clear();
    row.append(ui->label_14->text());
    row.append(ui->lineEdit_4->text());
    csvData->append(row);

    //导出文件
    QString filename;
    QFileDialog fileDialog(this);
    filename = fileDialog.getSaveFileName(this,"导出配置",QDir::currentPath(),"*.csv");
    if(filename=="")
        return;

    QxtCsvModel *csvModel;
    csvModel = new QxtCsvModel(this);

    csvModel->toCSV(filename);
    csvModel->setSource(filename);
    csvModel->insertRows(0,csvData->size());
    csvModel->insertColumns(0,2);

    for(int i=0; i<csvData->size(); i++)
        for(int j=0; j<2; j++)
            csvModel->setText(i,j,csvData->at(i).at(j));

    csvModel->toCSV(filename);

    delete csvData;
    delete csvModel;

    QMessageBox::information(nullptr,"导出配置","导出配置完成！");

}
void MainWindow::on_export2_clicked()
{
    //放入数据模型
    QVector<QStringList> *csvData;
    csvData = new QVector<QStringList>;
    QStringList row;

    //group 1
    row.clear();
    row.append(ui->groupBox_100->title());
    row.append("供水");
    csvData->append(row);

    row.clear();
    row.append(ui->label_100->text());
    row.append(ui->comboBox_100->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_101->text());
    row.append(ui->lineEdit_100->text());
    csvData->append(row);

    row.clear();
    row.append(ui->label_102->text());
    row.append(ui->lineEdit_101->text());
    csvData->append(row);

    row.clear();
    row.append(ui->label_103->text());
    row.append(ui->dateEdit_100->date().toString("yyyy-MM-dd"));
    csvData->append(row);


    //group 2
    row.clear();
    row.append(ui->groupBox_101->title());
    row.append("");
    csvData->append(row);

    row.clear();
    row.append(ui->label_110->text());
    row.append(ui->comboBox_110->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_111->text());
    row.append(ui->comboBox_111->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_112->text());
    row.append(ui->comboBox_112->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_113->text());
    row.append(ui->comboBox_113->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_114->text());
    row.append(ui->comboBox_114->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_115->text());
    row.append(ui->comboBox_115->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_116->text());
    row.append(ui->comboBox_116->currentText());
    csvData->append(row);

    row.clear();
    row.append("");
    row.append("");
    csvData->append(row);

    row.clear();
    row.append(ui->checkBox_110->text());
    row.append(ui->checkBox_110->isChecked()?"有":"无");
    csvData->append(row);

    row.clear();
    row.append(ui->checkBox_111->text());
    row.append(ui->checkBox_111->isChecked()?"有":"无");
    csvData->append(row);

    row.clear();
    row.append(ui->checkBox_112->text());
    row.append(ui->checkBox_112->isChecked()?"有":"无");
    csvData->append(row);

    row.clear();
    row.append(ui->checkBox_113->text());
    row.append(ui->checkBox_113->isChecked()?"有":"无");
    csvData->append(row);

    row.clear();
    row.append(ui->checkBox_114->text());
    row.append(ui->checkBox_114->isChecked()?"有":"无");
    csvData->append(row);

    row.clear();
    row.append(ui->checkBox_115->text());
    row.append(ui->checkBox_115->isChecked()?"有":"无");
    csvData->append(row);

    row.clear();
    row.append(ui->checkBox_116->text());
    row.append(ui->checkBox_116->isChecked()?"有":"无");
    csvData->append(row);

    row.clear();
    row.append(ui->checkBox_117->text());
    row.append(ui->checkBox_117->isChecked()?"有":"无");
    csvData->append(row);

    row.clear();
    row.append(ui->checkBox_118->text());
    row.append(ui->checkBox_118->isChecked()?"有":"无");
    csvData->append(row);

    row.clear();
    row.append("");
    row.append("");
    csvData->append(row);

    //group 3
    row.clear();
    row.append(ui->groupBox_102->title());
    row.append("");
    csvData->append(row);

    row.clear();
    row.append(ui->label_120->text());
    row.append(ui->comboBox_120->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_121->text());
    row.append(ui->comboBox_121->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_122->text());
    row.append(ui->comboBox_122->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_123->text());
    row.append(ui->comboBox_123->currentText());
    csvData->append(row);

    row.clear();
    row.append("");
    row.append("");
    csvData->append(row);

    //group 4
    row.clear();
    row.append(ui->groupBox_103->title());
    row.append("");
    csvData->append(row);

    row.clear();
    row.append(ui->label_130->text());
    row.append(ui->comboBox_130->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_131->text());
    row.append(ui->comboBox_131->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_132->text());
    row.append(ui->comboBox_132->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_133->text());
    row.append(ui->comboBox_133->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_134->text());
    row.append(ui->comboBox_134->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_135->text());
    row.append(ui->comboBox_135->currentText());
    csvData->append(row);


    row.clear();
    row.append("");
    row.append("");
    csvData->append(row);

    //group 5
    row.clear();
    row.append(ui->groupBox_104->title());
    row.append("");
    csvData->append(row);

    row.clear();
    row.append(ui->label_140->text());
    row.append(ui->comboBox_140->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_141->text());
    row.append(ui->comboBox_141->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_142->text());
    row.append(ui->comboBox_142->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_143->text());
    row.append(ui->comboBox_143->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_144->text());
    row.append(ui->comboBox_144->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_145->text());
    row.append(ui->comboBox_145->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_146->text());
    row.append(ui->comboBox_146->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_147->text());
    row.append(ui->comboBox_147->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_148->text());
    row.append(ui->comboBox_148->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_149->text());
    row.append(ui->comboBox_149->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_150->text());
    row.append(ui->comboBox_150->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_151->text());
    row.append(ui->comboBox_151->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_152->text());
    row.append(ui->comboBox_152->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_153->text());
    row.append(ui->comboBox_153->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_154->text());
    row.append(ui->comboBox_154->currentText());
    csvData->append(row);

    row.clear();
    row.append("");
    row.append("");
    csvData->append(row);

    //group 6
    row.clear();
    row.append(ui->groupBox_110->title());
    row.append("");
    csvData->append(row);

    row.clear();
    row.append(ui->checkBox_180->text());
    row.append(ui->checkBox_180->isChecked()?"有":"无");
    csvData->append(row);

    row.clear();
    row.append(ui->checkBox_181->text());
    row.append(ui->checkBox_181->isChecked()?"有":"无");
    csvData->append(row);

    row.clear();
    row.append(ui->checkBox_182->text());
    row.append(ui->checkBox_182->isChecked()?"有":"无");
    csvData->append(row);

    row.clear();
    row.append("");
    row.append("");
    csvData->append(row);

    //group 7
    row.clear();
    row.append(ui->groupBox_106->title());
    row.append("");
    csvData->append(row);

    row.clear();
    row.append(ui->label_160->text());
    row.append(ui->comboBox_160->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_161->text());
    row.append(ui->comboBox_161->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_162->text());
    row.append(ui->comboBox_162->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_163->text());
    row.append(ui->comboBox_163->currentText());
    csvData->append(row);

    row.clear();
    row.append(ui->label_164->text());
    row.append(ui->comboBox_164->currentText());
    csvData->append(row);

    row.clear();
    row.append("");
    row.append("");
    csvData->append(row);

    //group 8
    row.clear();
    row.append(ui->groupBox_108->title());
    row.append(ui->textEdit->toPlainText());
    csvData->append(row);

    row.clear();
    row.append("");
    row.append("");
    csvData->append(row);

    //group 9
    row.clear();
    row.append(ui->groupBox_107->title());
    row.append("");
    csvData->append(row);

    row.clear();
    row.append(ui->label_170->text());
    row.append(ui->lineEdit_170->text());
    csvData->append(row);

    row.clear();
    row.append(ui->label_171->text());
    row.append(ui->lineEdit_171->text());
    csvData->append(row);

    //导出文件
    QString filename;
    QFileDialog fileDialog(this);
    filename = fileDialog.getSaveFileName(this,"导出配置",QDir::currentPath(),"*.csv");
    if(filename=="")
        return;

    QxtCsvModel *csvModel;
    csvModel = new QxtCsvModel(this);

    csvModel->toCSV(filename);
    csvModel->setSource(filename);
    csvModel->insertRows(0,csvData->size());
    csvModel->insertColumns(0,2);

    for(int i=0; i<csvData->size(); i++)
        for(int j=0; j<2; j++)
            csvModel->setText(i,j,csvData->at(i).at(j));

    csvModel->toCSV(filename);

    delete csvData;
    delete csvModel;

    QMessageBox::information(nullptr,"导出配置","导出配置完成！");

}
void MainWindow::on_print_clicked()
{
    QFile myFile("html.fmt");
    if(!myFile.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(nullptr,"打印配置(PDF)","html.fmt " + myFile.errorString());
        return;
    }

    QTextStream textStream;
    textStream.setDevice(&myFile);

    textStream.seek(0);
    QString htmlString = textStream.readAll();

    QString checked;

    htmlString.replace("ZZ01",    ui->comboBox->currentText());
    htmlString.replace("ZZ02",    ui->lineEdit->text());
    htmlString.replace("ZZ03",    ui->dateEdit->date().toString("yyyy-MM-dd"));
    htmlString.replace("ZZ04",    ui->lineEdit_2->text());

    htmlString.replace("ZZ11",    ui->comboBox_2->currentText());
    htmlString.replace("ZZ12",    ui->comboBox_3->currentText());
    htmlString.replace("ZZ13",    ui->comboBox_4->currentText());
    htmlString.replace("ZZ14",    ui->comboBox_5->currentText());
    htmlString.replace("ZZ15",    ui->comboBox_6->currentText());
    htmlString.replace("ZZ16",    ui->comboBox_7->currentText());
    htmlString.replace("ZZ17",    ui->comboBox_8->currentText());
    htmlString.replace("ZZ18",    ui->comboBox_9->currentText());

    checked = ui->checkBox->isChecked()?"有":"无";	htmlString.replace("ZZ19",checked);
    checked = ui->checkBox_2->isChecked()?"有":"无";	htmlString.replace("ZZ20",checked);
    checked = ui->checkBox_3->isChecked()?"有":"无";	htmlString.replace("ZZ21",checked);
    checked = ui->checkBox_4->isChecked()?"有":"无";	htmlString.replace("ZZ22",checked);
    checked = ui->checkBox_5->isChecked()?"有":"无";	htmlString.replace("ZZ23",checked);
    checked = ui->checkBox_6->isChecked()?"有":"无";	htmlString.replace("ZZ24",checked);
    checked = ui->checkBox_7->isChecked()?"有":"无";	htmlString.replace("ZZ25",checked);

    htmlString.replace("ZZ31",    ui->comboBox_62->currentText());
    htmlString.replace("ZZ32",    ui->comboBox_63->currentText());
    htmlString.replace("ZZ33",    ui->comboBox_64->currentText());
    htmlString.replace("ZZ34",    ui->comboBox_65->currentText());

    htmlString.replace("ZZ41",    ui->comboBox_66->currentText());
    htmlString.replace("ZZ42",    ui->comboBox_67->currentText());
    htmlString.replace("ZZ43",    ui->comboBox_68->currentText());
    htmlString.replace("ZZ44",    ui->comboBox_69->currentText());
    htmlString.replace("ZZ45",    ui->comboBox_70->currentText());

    htmlString.replace("ZZ51",    ui->comboBox_71->currentText());
    htmlString.replace("ZZ52",    ui->comboBox_72->currentText());
    htmlString.replace("ZZ53",    ui->comboBox_73->currentText());
    htmlString.replace("ZZ54",    ui->comboBox_74->currentText());
    htmlString.replace("ZZ55",    ui->comboBox_75->currentText());
    htmlString.replace("ZZ56",    ui->comboBox_76->currentText());
    htmlString.replace("ZZ57",    ui->comboBox_77->currentText());
    htmlString.replace("ZZ58",    ui->comboBox_78->currentText());
    htmlString.replace("ZZ59",    ui->comboBox_79->currentText());
    htmlString.replace("ZZ60",    ui->comboBox_80->currentText());
    htmlString.replace("ZZ61",    ui->comboBox_81->currentText());
    htmlString.replace("ZZ62",    ui->comboBox_82->currentText());
    htmlString.replace("ZZ63",    ui->comboBox_83->currentText());
    htmlString.replace("ZZ64",    ui->comboBox_84->currentText());
    htmlString.replace("ZZ65",    ui->comboBox_85->currentText());

    checked = ui->checkBox_20->isChecked()?"有":"无";htmlString.replace("ZZ70",checked);
    checked = ui->checkBox_21->isChecked()?"有":"无";htmlString.replace("ZZ71",checked);
    checked = ui->checkBox_22->isChecked()?"有":"无";htmlString.replace("ZZ72",checked);

    htmlString.replace("ZZ81",    ui->comboBox_90->currentText());
    htmlString.replace("ZZ82",    ui->comboBox_91->currentText());
    htmlString.replace("ZZ83",    ui->comboBox_92->currentText());
    htmlString.replace("ZZ84",    ui->comboBox_93->currentText());
    htmlString.replace("ZZ85",    ui->comboBox_94->currentText());
    htmlString.replace("ZZ86",    ui->comboBox_95->currentText());

    htmlString.replace("ZZ90",    ui->textEdit->toHtml());

    htmlString.replace("ZZ91",    ui->lineEdit_3->text());
    htmlString.replace("ZZ92",    ui->lineEdit_4->text());

    QString filename;
    QFileDialog fileDialog(this);
    filename = fileDialog.getSaveFileName(this,"打印配置(PDF)",QDir::currentPath(),"*.pdf");
    if(filename=="")
        return;

    QPrinter printer_text;
    printer_text.setOutputFormat(QPrinter::PdfFormat);
    printer_text.setOutputFileName(filename);
    printer_text.setPageMargins(0, 0, 0, 0, QPrinter::Millimeter);

    QTextDocument text_document;
    text_document.setHtml(htmlString);
    text_document.print(&printer_text);

    QMessageBox::information(nullptr,"打印配置(PDF","打印配置(PDF完成！");
}
void MainWindow::on_print2_clicked()
{
    QFile myFile("html2.fmt");
    if(!myFile.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(nullptr,"打印配置(PDF)","html2.fmt " + myFile.errorString());
        return;
    }

    QTextStream textStream;
    textStream.setDevice(&myFile);

    textStream.seek(0);
    QString htmlString = textStream.readAll();

    QString checked;

    htmlString.replace("ZZ01",    ui->comboBox_100->currentText());
    htmlString.replace("ZZ02",    ui->lineEdit_100->text());
    htmlString.replace("ZZ03",    ui->dateEdit_100->date().toString("yyyy-MM-dd"));
    htmlString.replace("ZZ04",    ui->lineEdit_101->text());

    htmlString.replace("ZZ11",    ui->comboBox_110->currentText());
    htmlString.replace("ZZ12",    ui->comboBox_111->currentText());
    htmlString.replace("ZZ13",    ui->comboBox_112->currentText());
    htmlString.replace("ZZ14",    ui->comboBox_113->currentText());
    htmlString.replace("ZZ15",    ui->comboBox_114->currentText());
    htmlString.replace("ZZ16",    ui->comboBox_115->currentText());
    htmlString.replace("ZZ10",    ui->comboBox_116->currentText());


    checked = ui->checkBox_110->isChecked()?"有":"无";	htmlString.replace("ZZ17",checked);
    checked = ui->checkBox_111->isChecked()?"有":"无";	htmlString.replace("ZZ18",checked);
    checked = ui->checkBox_112->isChecked()?"有":"无";	htmlString.replace("ZZ19",checked);
    checked = ui->checkBox_113->isChecked()?"有":"无";	htmlString.replace("ZZ20",checked);
    checked = ui->checkBox_114->isChecked()?"有":"无";	htmlString.replace("ZZ21",checked);
    checked = ui->checkBox_115->isChecked()?"有":"无";	htmlString.replace("ZZ22",checked);
    checked = ui->checkBox_116->isChecked()?"有":"无";	htmlString.replace("ZZ23",checked);
    checked = ui->checkBox_117->isChecked()?"有":"无";	htmlString.replace("ZZ24",checked);
    checked = ui->checkBox_118->isChecked()?"有":"无";	htmlString.replace("ZZ25",checked);

    htmlString.replace("ZZ31",    ui->comboBox_120->currentText());
    htmlString.replace("ZZ32",    ui->comboBox_121->currentText());
    htmlString.replace("ZZ33",    ui->comboBox_122->currentText());
    htmlString.replace("ZZ34",    ui->comboBox_123->currentText());

    htmlString.replace("ZZ41",    ui->comboBox_130->currentText());
    htmlString.replace("ZZ42",    ui->comboBox_131->currentText());
    htmlString.replace("ZZ43",    ui->comboBox_132->currentText());
    htmlString.replace("ZZ44",    ui->comboBox_133->currentText());
    htmlString.replace("ZZ45",    ui->comboBox_134->currentText());
    htmlString.replace("ZZ46",    ui->comboBox_135->currentText());

    htmlString.replace("ZZ51",    ui->comboBox_140->currentText());
    htmlString.replace("ZZ52",    ui->comboBox_141->currentText());
    htmlString.replace("ZZ53",    ui->comboBox_142->currentText());
    htmlString.replace("ZZ54",    ui->comboBox_143->currentText());
    htmlString.replace("ZZ55",    ui->comboBox_144->currentText());
    htmlString.replace("ZZ56",    ui->comboBox_145->currentText());
    htmlString.replace("ZZ57",    ui->comboBox_146->currentText());
    htmlString.replace("ZZ58",    ui->comboBox_147->currentText());
    htmlString.replace("ZZ59",    ui->comboBox_148->currentText());
    htmlString.replace("ZZ60",    ui->comboBox_149->currentText());
    htmlString.replace("ZZ61",    ui->comboBox_150->currentText());
    htmlString.replace("ZZ62",    ui->comboBox_151->currentText());
    htmlString.replace("ZZ63",    ui->comboBox_152->currentText());
    htmlString.replace("ZZ64",    ui->comboBox_153->currentText());
    htmlString.replace("ZZ65",    ui->comboBox_154->currentText());

    checked = ui->checkBox_180->isChecked()?"有":"无";htmlString.replace("ZZ70",checked);
    checked = ui->checkBox_181->isChecked()?"有":"无";htmlString.replace("ZZ71",checked);
    checked = ui->checkBox_182->isChecked()?"有":"无";htmlString.replace("ZZ72",checked);

    htmlString.replace("ZZ81",    ui->comboBox_160->currentText());
    htmlString.replace("ZZ82",    ui->comboBox_161->currentText());
    htmlString.replace("ZZ83",    ui->comboBox_162->currentText());
    htmlString.replace("ZZ84",    ui->comboBox_163->currentText());
    htmlString.replace("ZZ85",    ui->comboBox_164->currentText());


    htmlString.replace("ZZ90",    ui->textEdit->toHtml());

    htmlString.replace("ZZ91",    ui->lineEdit_170->text());
    htmlString.replace("ZZ92",    ui->lineEdit_171->text());

    QString filename;
    QFileDialog fileDialog(this);
    filename = fileDialog.getSaveFileName(this,"打印配置(PDF)",QDir::currentPath(),"*.pdf");
    if(filename=="")
        return;

    QPrinter printer_text;
    printer_text.setOutputFormat(QPrinter::PdfFormat);
    printer_text.setOutputFileName(filename);
    printer_text.setPageMargins(0, 0, 0, 0, QPrinter::Millimeter);

    QTextDocument text_document;
    text_document.setHtml(htmlString);
    text_document.print(&printer_text);

    QMessageBox::information(nullptr,"打印配置(PDF","打印配置(PDF完成！");
}
