#include "mainwindow.h"
#include <QApplication>

#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTextCodec>
#include <QDebug>
#include <QMessageBox>

bool creatConnect();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

/*//////////////////////////////////////////////////////////////////////*/
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);

    if(!creatConnect())
    {
        return 1;
    }

    qDebug() << "Available drivers:";

    QStringList drivers = QSqlDatabase::drivers();

    foreach(QString driver, drivers)
        qDebug() << "\t "<< driver;

    qDebug() << "End";

    QSqlQuery query;
    QString sql = "select * from load_profile;";
    qDebug() << sql;
    query.exec(sql);
    while(query.next())
    {
        qDebug()<<query.value(0).toString()<<query.value(1).toString();
    }
 /*////////////////////////////////////////////////////////////////////////*/
    return a.exec();
}


bool creatConnect()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("192.168.31.84");
    db.setPort(3306);
    db.setDatabaseName("pumpenergycalculation");
    db.setUserName("user_readonly");
    db.setPassword("1234");

    bool ok = db.open();//建立数据库连接
    if(!ok)
    {
        QMessageBox::critical(nullptr,QObject::tr("连接数据库失败！！！"),db.lastError().text());
        return false;
    }
    else
    {
        QMessageBox::information(nullptr,QObject::tr("Tips"),QObject::tr("连接数据库成功！！！"));
        return true;
    }
}
