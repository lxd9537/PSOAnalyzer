#include "mainwindow.h"
#include <QApplication>

//
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlQuery>
//


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QString sDbNm = "A.accdb";//
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");//设置数据库驱动

    QString dsn = QString("DRIVER={Microsoft Access Driver (*.mdb, *.accdb)}; FIL={MS Access};DBQ=%1;").arg(sDbNm);//连接字符串
    db.setDatabaseName(dsn);//设置连接字符串
    db.setUserName("");//设置登陆数据库的用户名
    db.setPassword("");//设置密码
    bool ok = db.open();
     if (!ok) {
     QMessageBox messageBox;
     messageBox.setText("Database error");
     messageBox.exec();
     db.close();




    return a.exec();
}
